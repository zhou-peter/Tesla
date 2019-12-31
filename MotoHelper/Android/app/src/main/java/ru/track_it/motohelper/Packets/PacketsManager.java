package ru.track_it.motohelper.Packets;

import android.os.Build;
import android.util.Log;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentLinkedQueue;

import ru.track_it.motohelper.Executors;

public class PacketsManager implements Runnable, Closeable {

    public static final byte PACKET_START = 0x7C;
    private static final int RECEIVE_TIMEOUT = 3500;
    private static final int TIMER_PERIOD = 10;
    private static final int KEEP_ALIVE_PERIOD = 500;
    private static final int BUF_SIZE_RX = 200;
    private static final int BODY_OFFSET = 4;
    private static final int EMPTY_SIZE = 6;
    private static final String LOG_TAG = "PacketManager";
    private static final Object[] constructorArgs = new Object[]{};
    private static final Map<Integer, Constructor> inPacketsConstructors = new HashMap<>();
    public final ConcurrentLinkedQueue<AbstractInPacket> receivedPackets = new ConcurrentLinkedQueue<>();
    public final ConcurrentLinkedQueue<AbstractOutPacket> packetsToSend = new ConcurrentLinkedQueue<>();

    private InputStream inputStream;
    private OutputStream outputStream;
    private PacketsListener packetsListener;

    private Thread thread = new Thread(this);
    private Thread threadSender;
    private Boolean canRun = true;

    private Timer timer = new Timer();
    private final Object timerLock=new Object();
    private volatile boolean timerEnabled = false;
    private volatile long timerTarget = 0;
    private long timerKeepAlive = 0;
    private volatile long timerCounter = 0;

    ReceiverStates CommState = ReceiverStates.WaitingStart;
    byte[] rxBuf = new byte[BUF_SIZE_RX];
    int rxIndex = 0;
    int rxPackSize = 0;

    public PacketsManager(InputStream inputStream, OutputStream outputStream, PacketsListener packetsListener) {
        this.inputStream = inputStream;
        this.outputStream = outputStream;
        this.packetsListener = packetsListener;

        threadSender = new Thread(packetSender);
        threadSender.start();
        thread.start();
        timer.scheduleAtFixedRate(timerTask, 0, TIMER_PERIOD);
    }

    private void startTimer() {
        synchronized (timerLock) {
            timerTarget = timerCounter + RECEIVE_TIMEOUT;
            timerEnabled = true;
        }
    }

    private void stopTimer() {
        synchronized (timerLock){
            timerEnabled = false;
        }
    }

    TimerTask timerTask = new TimerTask() {
        @Override
        public void run() {
            if (!canRun) {
                timer.purge();
                return;
            }

            timerCounter += TIMER_PERIOD;

            if (timerCounter > timerKeepAlive) {
                if (packetsToSend.size() == 0) {
                    packetsToSend.add(new PacketOut_01());
                    Log.v(LOG_TAG, "Keep alive added");
                }
                timerKeepAlive = timerCounter + KEEP_ALIVE_PERIOD;
            }

            synchronized (timerLock){
                long timespan = timerCounter - timerTarget;
                if (timerEnabled && timespan>0) {
                    Log.v(LOG_TAG, "Таймаут из "+ CommState.toString());
                    CommState = ReceiverStates.ReceivingTimeout;
                    timerEnabled = false;
                }
            }
        }
    };

    private Runnable packetSender = new Runnable() {
        @Override
        public void run() {
            try {
                while (canRun) {
                    if (packetsToSend.size() > 0) {
                        AbstractOutPacket pack = packetsToSend.poll();
                        outputStream.write(pack.ToArray());
                        Log.v(LOG_TAG, "Packet send");
                        outputStream.flush();
                        Log.v(LOG_TAG, "Output stream flush");
                    }
                    Thread.sleep(50);
                }
            } catch (Exception ex) {
                Log.e("packet run problem", ex.toString());
                ex.printStackTrace();
                canRun = false;
            } finally {
                try {
                    close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    };

    @Override
    public void run() {
        try {
            while (canRun) {
                //blocking method
                int availableLength = BUF_SIZE_RX - rxIndex;
                if (availableLength==0){
                    packetFormerCheck();
                    continue;
                }
                int rxBytesNow = inputStream.read(rxBuf, rxIndex, availableLength);
                if (rxBytesNow > 0) {
                    Log.v(LOG_TAG, "Добавлено данных "+ rxBytesNow);
                    rxIndex += rxBytesNow;
                    packetFormerCheck();
                }
            }
        } catch (Exception ex) {
            Log.e("packet run problem", ex.toString());
            ex.printStackTrace();
            canRun = false;
        } finally {
            try {
                close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }


    @Override
    public void close() throws IOException {
        canRun = false;
        try {
            timer.purge();
            thread.join();
        } catch (InterruptedException ex) {
            Log.e("close problem", ex.toString());
        }

    }

    /**
     * @return if true - call me again
     */
    private void packetFormerCheck() {
        int offset = 0;
        while(true) {
            Log.v(LOG_TAG, CommState.toString()+ " "+ offset + " " +rxIndex + " " +
                    rxPackSize);

            //ищем стартовый байт
            if (CommState == ReceiverStates.WaitingStart) {
                if (rxIndex <= offset) {
                    stopTimer();
                    break;
                }
                while (rxIndex > offset && rxBuf[offset] != PACKET_START) {
                    offset++;
                }

                //не нашли старт байт
                if (offset==rxBuf.length){
                    rxIndex=0;
                    stopTimer();
                    break;
                }

                if (rxBuf[offset] == PACKET_START) {
                    CommState = ReceiverStates.ReceivingSize;
                    rxPackSize = 0;
                    int delta = rxIndex - offset;
                    if (offset>0 && delta>0){
                        shift(offset, delta);
                        offset = 0;
                        rxIndex=delta;
                    }
                    startTimer();
                }else{
                    rxIndex=0;
                    stopTimer();
                    break;
                }
            }
            if (CommState == ReceiverStates.ReceivingSize) {
                if (rxIndex > offset+3) {
                    int inPacksize = rxBuf[offset+1] | rxBuf[offset+2] << 8;
                    if (inPacksize < EMPTY_SIZE) {
                        Log.v(LOG_TAG, "Слишком маленький пакет");
                        offset++;
                        CommState = ReceiverStates.WaitingStart;
                        continue;
                    }
                    if (inPacksize > BUF_SIZE_RX - EMPTY_SIZE) {
                        Log.v(LOG_TAG, "Входящий пакет с слишком большим размером");
                        offset++;
                        CommState = ReceiverStates.WaitingStart;
                        continue;
                    }
                    rxPackSize = inPacksize;
                    CommState = ReceiverStates.ReceivingPacket;
                } else {
                    //continue receive data from the stream
                    break;
                }
            }
            if (CommState == ReceiverStates.ReceivingPacket) {
                if (rxIndex-offset < rxPackSize) {
                    //free - receive next bytes from the stream
                    Log.v(LOG_TAG, "not enough data - exit");
                    break;
                }
                else{
                    //Пакет пришел. останавливаем таймер и проверяем его
                    stopTimer();
                    CommState = ReceiverStates.Processing;
                }
            }
            if (CommState == ReceiverStates.ReceivingTimeout) {
                Log.v(LOG_TAG, "Таймаут получения пакета");
                offset++;
                CommState = ReceiverStates.WaitingStart;
                continue;
            }
            if (CommState == ReceiverStates.Processing) {
                //check CRC
                int crc = 0;
                //265-2=263
                for (int i = 0; i < rxPackSize - 2; i++) {
                    int nextByte = (rxBuf[offset+i] & 0xFF);
                    crc += nextByte;
                }
                byte crcXOR = (byte) (crc ^ 0xAA);

                byte incomeCrc = rxBuf[offset+rxPackSize - 2];
                byte incomeCrcXOR = rxBuf[offset+rxPackSize - 1];
                //если контрольная сумма сошлась
                if ((byte)crc ==  incomeCrc && crcXOR == incomeCrcXOR) {
                    enqeueIncomingPacket(offset);
                    //если получили 1.5-2 пакета за раз
                    int delta = rxIndex - (rxPackSize + offset);
                    if (delta > 0) {
                        shift(offset+ rxPackSize, delta);
                    }
                    rxIndex = delta;
                    offset = 0;
                    CommState = ReceiverStates.WaitingStart;
                    //goto start
                    continue;
                } else {//crc error
                    Log.v(LOG_TAG, "Ошибка CRC");
                    offset++;
                    CommState = ReceiverStates.WaitingStart;
                }
            }
        }
    }

    /**
     * копируем буфер в начало
     * @param offset
     * @param count
     */
    private void shift(int offset, int count){
        for (int i = 0; i < count; i++) {
            rxBuf[i] = rxBuf[offset + i];
        }
    }

    private void enqeueIncomingPacket(int offset) {
        int inPacksize = rxBuf[offset+1] | rxBuf[offset+2] << 8;
        int packetNumber = rxBuf[offset+3];

        try {
            Constructor c;
            if (inPacketsConstructors.containsKey(packetNumber)) {
                c = inPacketsConstructors.get(packetNumber);
            } else {
                String packetName = getClass().getPackage().getName() + ".PacketIn_" + String.format("%02X", packetNumber);
                Class t = Class.forName(packetName);
                c = t.getConstructors()[0];
                inPacketsConstructors.put(packetNumber, c);
            }

            AbstractInPacket pack = (AbstractInPacket) c.newInstance(constructorArgs);

            if (pack != null) {
                pack.Command = rxBuf[offset+3];
                int bodySize = inPacksize - EMPTY_SIZE;
                if (bodySize > 0) {
                    pack.BodySize = bodySize;
                    pack.ApplyBody(rxBuf, offset+BODY_OFFSET, bodySize);
                }
                receivedPackets.add(pack);
                Log.v(LOG_TAG, "Packet enqueued " + pack.toString());
                if (packetsListener!=null){
                    Executors.BackGroundThreadPool.execute(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                packetsListener.onNewPacketsCame();
                            }catch (Exception ex){
                                ex.printStackTrace();
                            }
                        }
                    });
                }
            }
        } catch (Exception ex) {
            Log.e(LOG_TAG, "Error instantiate packet " + String.format("%02X", packetNumber));
            ex.printStackTrace();
        }
    }


}
