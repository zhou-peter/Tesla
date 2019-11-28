package ru.track_it.motohelper.Packets;

import android.util.Log;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Executors;

public class PacketsManager implements Runnable, Closeable {

    public static final byte PACKET_START = 0x7C;
    private static final int RECEIVE_TIMEOUT = 500;
    private static final int BUF_SIZE_RX = 1024;
    private static final int BODY_OFFSET = 4;
    private static final int EMPTY_SIZE = 6;
    private static final String LOG_TAG = "PacketManager";
    private static final Object[] constructorArgs=new Object[]{};
    private static final Map<Integer, Constructor> inPacketsConstructors=new HashMap<>();
    public static final ConcurrentLinkedQueue<AbstractInPacket> receivedPackets = new ConcurrentLinkedQueue<>();
    public static final ConcurrentLinkedQueue<AbstractOutPacket> packetsToSend = new ConcurrentLinkedQueue<>();

    private InputStream inputStream;
    private OutputStream outputStream;

    private Thread thread = new Thread(this);
    private Boolean canRun=true;
    private Timer timer=new Timer();

    ReceiverStates CommState = ReceiverStates.WaitingStart;
    byte[] rxBuf = new byte[BUF_SIZE_RX];
    int rxIndex=0;
    int rxPackSize = 0;

    public PacketsManager(InputStream inputStream, OutputStream outputStream) {
        this.inputStream=inputStream;
        this.outputStream=outputStream;


        thread.start();
    }

    private void startTimer(){
        timer.cancel();
        timer.schedule(timerTask,0, RECEIVE_TIMEOUT);
    }

    private void stopTimer(){
        timer.cancel();
    }

    TimerTask timerTask=new TimerTask() {
        @Override
        public void run() {
            CommState = ReceiverStates.ReceivingTimeout;
        }
    }

    @Override
    public void run() {
        try
        {
            while (canRun)
            {
                if (packetsToSend.size() > 0)
                {
                    sendOutPackets();
                }

                //blocking method
                int availableLength = BUF_SIZE_RX - rxIndex;
                int rxBytesNow = inputStream.read(rxBuf, rxIndex, availableLength);
                if (rxBytesNow>0){
                    rxIndex += rxBytesNow;
                    while(packetFormerCheck());
                }
            }
        }
        catch (Exception ex)
        {
            Log.e("packet run problem", ex.toString());
            canRun = false;
        }
    }

    private void sendOutPackets() throws IOException {
        while (packetsToSend.size()>0){
            AbstractOutPacket pack = packetsToSend.poll();
            outputStream.write(pack.ToArray());
            outputStream.flush();
        }
    }

    @Override
    public void close() throws IOException{
        canRun = false;
        try {
            thread.join();
        }catch (InterruptedException ex){
            Log.e("close problem", ex.toString());
        }

    }
    private void create_err(String msg)
    {
        rxIndex = 0;
        CommState = ReceiverStates.WaitingStart;
        timer.cancel();
        Log.d(LOG_TAG, msg);
    }


    /**
     * @return if true - call me again
     */
    private boolean packetFormerCheck()
    {

        if (CommState == ReceiverStates.WaitingStart)
        {
            //ждем новый пакет
            //статус может изменить таймер таймаута
            if (rxIndex == 0)
            {
                return false;
            }
            if (rxIndex > 0 && rxBuf[0] != PACKET_START)
            {
                rxIndex = 0;
                create_err("Неправильный стартовый пакет");
                return false;
            }
            else
            {
                CommState = ReceiverStates.ReceivingSize;
                rxPackSize = 0;
                startTimer();
            }
        }
        if (CommState == ReceiverStates.ReceivingSize)
        {
            if (rxIndex > 3)
            {
                int inPacksize = rxBuf[1] | rxBuf[2] << 8;
                if (inPacksize < EMPTY_SIZE)
                {
                    create_err("Слишком маленький пакет");
                    return false;
                }
                if (inPacksize > BUF_SIZE_RX - EMPTY_SIZE)
                {
                    create_err("Входящий пакет с слишком большим размером");
                    return false;
                }
                rxPackSize = inPacksize;
                CommState = ReceiverStates.ReceivingPacket;
            }
            else
            {
                return false;
            }
        }
        if (CommState == ReceiverStates.ReceivingPacket)
        {
            if (rxIndex >= rxPackSize)
            {
                //Пакет пришел. останавливаем таймер и проверяем его
                stopTimer();
                CommState = ReceiverStates.Processing;
            }

        }
        if (CommState == ReceiverStates.ReceivingTimeout)
        {
            create_err("Таймаут получения пакета");
        }
        if (CommState == ReceiverStates.Processing)
        {
            //останавливаем таймер таймаута так как пришел весь пакет
            stopTimer();
            //check CRC
            byte crc = 0;
            //265-2=263
            for (int i = 0; i < rxPackSize - 2; i++)
            {
                crc += rxBuf[i];
            }
            byte crcXOR = (byte)(crc ^ (byte)0xAA);
            //если контрольная сумма сошлась
            if (crc == rxBuf[rxPackSize - 2] &&
                    crcXOR == rxBuf[rxPackSize - 1])
            {
                enqeueIncomingPacket();
                //если получили 1.5-2 пакета за раз
                int delta = rxPackSize-rxIndex;
                if (delta>0){
                    //копируем буфер в начало
                    for (int i=0;i<delta;i++){
                        rxBuf[i] = rxBuf[rxIndex+i];
                    }
                }
                rxIndex = delta;
                CommState = ReceiverStates.WaitingStart;
                //goto start
                if (rxIndex>0){
                    return true;
                }
            }
            else
            {//crc error
                create_err("Ошибка CRC");
                return false;
            }
        }
        return false;
    }



    private void enqeueIncomingPacket()
    {
        int inPacksize = rxBuf[1] | rxBuf[2] << 8;
        int packetNumber = rxBuf[3];

        try
        {
            Constructor c;
            if (inPacketsConstructors.containsKey(packetNumber)) {
                c=inPacketsConstructors.get(packetNumber);
            }else{
                String packetName = "PacketIn_" + String.format("%02X", packetNumber);
                Class t = Class.forName(packetName);
                c =t.getConstructors()[0];
                inPacketsConstructors.put(packetNumber, c);
            }

            AbstractInPacket pack = (AbstractInPacket)c.newInstance(constructorArgs);

            if (pack != null)
            {
                pack.Command = rxBuf[3];
                int bodySize = inPacksize - EMPTY_SIZE;
                if (bodySize>0)
                {
                    pack.BodySize = bodySize;
                    pack.ApplyBody(rxBuf, BODY_OFFSET, bodySize);
                }
                receivedPackets.add(pack);
            }
        }
        catch (Exception ex)
        {
            Log.e(LOG_TAG, "Error instantiate packet " + packetNumber);
        }
    }












}
