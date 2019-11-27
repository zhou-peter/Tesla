package ru.track_it.motohelper;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothSocket;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import java.io.*;
import java.util.Set;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by winuser on 01.02.2017.
 */
@SuppressWarnings("MissingPermission")
public class BluetoothSPPClient
        implements Runnable {

    public BluetoothSPPClient(TheService service){
        super();
        theService=service;
    }

    TheService theService;
    final String LOG_TAG="Bluetoothclient";
    final UUID sppUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    BluetoothAdapter btAdapter=null;
    BluetoothSocket btSocket=null;
    InputStream btInputStream=null;
    OutputStream btOutputStream=null;
    boolean shouldStop = false;


    public boolean isReady(){
        if (btSocket==null
            || btInputStream==null
            || btOutputStream==null)return false;
        //if (!btSocket.isConnected())return false;
        /*By the way, I found that on some android devices isConnected()
        always returns false. In such case just try to write something to socket
        and check if there is no exception.*/
        if (socketClosed)return false;
        return true;
    }
    boolean socketClosed=false;
    void closeSocket(){
        try{
            socketClosed=true;
            if (btInputStream!=null)btInputStream.close();
            if(btOutputStream!=null)btOutputStream.close();
            if (btSocket!=null)btSocket.close();
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    void LogDebug(String text){
        Log.d(LOG_TAG,text);
        Message msg=new Message();
        msg.what=TheService.MSG_BLUETOOTH_LOG;
        msg.obj=text;
        theService.sendMessage(msg);
    }
    void Init(){
        btAdapter=BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) {
            LogDebug("Bluetooth adapter is not available.");
            return;
        }
        Log.d(LOG_TAG,"Bluetooth adapter is found.");

        if (!btAdapter.isEnabled()) {
            LogDebug("Bluetooth is disabled. Check configuration.");
            theService.sendEmptyMessage(TheService.MSG_BLUETOOTH_SHOULD_ENABLE);
            return;
        }
        Log.d(LOG_TAG,"Bluetooth is enabled.");

        if (!btAdapter.isDiscovering())btAdapter.startDiscovery();
        BluetoothDevice btDevice = null;
        Set<BluetoothDevice> bondedDevices = btAdapter.getBondedDevices();

        Pattern namePattern=Pattern.compile("(Sinushkin.*)|(BT04-A)|(HC-05)|(HC-06)");
        if (btDevice==null){
            for (BluetoothDevice dev : bondedDevices) {
                String name=dev.getName();
                Log.d(LOG_TAG,"Paired device: " + name+ " (" + dev.getAddress() + ")");
                Matcher m = namePattern.matcher(name);
                if (m.matches()) {
                    btDevice = dev;
                    break;
                }
            }
        }
        if (btDevice == null) {
            LogDebug("Target bounded Bluetooth device is not found.");
            theService.sendEmptyMessage(TheService.MSG_BLUETOOTH_SHOULD_PAIR);
            return;
        }
        LogDebug("Target Bluetooth device is found.");


        try {

            btAdapter.cancelDiscovery();
            btSocket = btDevice.createRfcommSocketToServiceRecord(sppUuid);
            btSocket.connect();

            Thread.sleep(200);
        } catch (IOException ex) {
            LogDebug("Failed to create RfComm socket: " + ex.toString());
            return;
        }catch (Exception ex){
            ex.printStackTrace();
            Log.e(LOG_TAG, "bluetooth socket", ex);
            return;
        }
        Log.d(LOG_TAG,"Created a bluetooth socket.");


        for (int i = 0; i < 5 ; i++) {
            try {
                if (btSocket==null)
                {
                    LogDebug("Bluetooth socket connect fail.");
                    return;
                }
                btInputStream=btSocket.getInputStream();
                btOutputStream=btSocket.getOutputStream();
                break;
            } catch (IOException ex) {
                LogDebug("Failed to connect. Retrying: " + ex.toString());
            }
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (i<4)continue;
            LogDebug("Failed to connect to " + btDevice.getName());
            return;
        }

        socketClosed=false;
        LogDebug("Connected to the bluetooth socket.");
        createOutPacketAndSend(0x10,1,new byte[]{1});

    }


    @Override
    public void run() {

        try {

            Looper.prepare();

            while (!shouldStop) {
                try {
                    if (isReady()) {
                        if (btInputStream.available() > 0) {
                            int rxBytesNow = btInputStream.read(rxBuf, rxIndex, MAX_BUF_SIZE_RX - rxIndex);
                            rxIndex += rxBytesNow;
                            packetFormerCheck();
                        }else{
                            btOutputStream.flush();
                        }
                    } else {
                        Init();
                        Thread.sleep(1000);
                    }
                }catch(IOException ex){
                    closeSocket();
                    Init();
                }

                if (timerEnabled) {//эмитация таймера
                    timerCounter++;
                    if (timerCounter > RECEIVE_TIMEOUT) {
                        PacketTimeOut();
                    }
                }
                Thread.sleep(10);
            }
            //dispose
            closeSocket();
            Log.d(LOG_TAG, "SPP main exit");
        } catch (Exception ex) {
            ex.printStackTrace();
            Log.e(LOG_TAG, "SPP main run", ex);
            shouldStop=true;
        }
    }

    //пакет находится в rxBuf
    //с адреса 0
    void processIncomingPacket(){
        int packetType=rxBuf[3];

        if (packetType==0x01){//Подтверждение команды
            //createOutPacketAndSend(1,1,&Env.rxBuf[3]);


        } else {
            //create_err(0x05);
            theService.onPacket(rxBuf);
        }
    }

    //region packet former

    final int MAX_BUF_SIZE_RX=300;
    final int MAX_BUF_SIZE_TX=1100;
    final int RECEIVE_TIMEOUT=500;
    byte[] rxBuf=new byte[MAX_BUF_SIZE_RX];
    byte[] txBuf=new byte[MAX_BUF_SIZE_TX];
    int rxIndex=0;
    int txIndex=0;
    int timerCounter=0;
    boolean timerEnabled=false;
    final byte PACKET_START=(byte)0xB1;
    final int WaitingStart=1;
    final int ReceivingSize=2;
    final int ReceivingPacket=3;
    final int ReceivingTimeout=4;
    final int Processing=5;
    int CommState=WaitingStart;
    int rxPackSize=0;
    void packetFormerCheck(){

        if (CommState==WaitingStart){
            //ждем новый пакет
            //статус может изменить таймер таймаута
            if (rxIndex==0){
                return;
            }
            if (rxIndex>0 && rxBuf[0]!=PACKET_START){
                rxIndex=0;
                create_err(0x01);
                return;
            }else{
                CommState=ReceivingSize;
                rxPackSize=0;
                timerEnabled=true;
                timerCounter=0;
            }
        }
        if (CommState==ReceivingSize){
            if (rxIndex>3){
                int inPacksize=TheService.getU8(rxBuf[1]) | TheService.getU8(rxBuf[2])<<8;
                if (inPacksize<6){
                    create_err(0x02);
                    timerEnabled=false;
                    return;
                }
                if (inPacksize>MAX_BUF_SIZE_RX-6){
                    create_err(0x03);
                    timerEnabled=false;
                    return;
                }
                rxPackSize=inPacksize;
                CommState=ReceivingPacket;
            }else{
                return;
            }
        }
        if (CommState==ReceivingPacket){
            if (rxIndex>=rxPackSize){
                //Пакет пришел. останавливаем таймер и проверяем его
                timerEnabled=false;
                CommState=Processing;
            }

        }
        if (CommState==ReceivingTimeout){
            create_err(0x06);
            timerEnabled=false;
        }
        if(CommState==Processing){
            //останавливаем таймер таймаута так как пришел весь пакет
            timerEnabled=false;
            //check CRC
            byte crc=0;
            //265-2=263
            for (int i=0;i<rxPackSize-2;i++){
                crc+=rxBuf[i];
            }
            //если контрольная сумма сошлась
            if (crc==rxBuf[rxPackSize-2]&&
                    (byte)(crc^(byte)0xAA)==rxBuf[rxPackSize-1]){
                processIncomingPacket();
            }else{//crc error
                create_err(0x04);
                timerEnabled=false;
                return;
            }
            rxIndex=0;
            CommState=WaitingStart;
        }
    }

    void PacketTimeOut()
    {
        CommState=ReceivingTimeout;
    }
    void create_err(int err){
        //createOutPacketAndSend(0x02,1,&err);
        Log.e(LOG_TAG,"bluetooth err " +String.valueOf(err));
        rxIndex=0;
        CommState=WaitingStart;
    }



    public synchronized void createOutPacketAndSend(int command, int bodySize,
                                byte[] bodyData){
        int i=0;
        int txBufSize=6+bodySize;
        txBuf[0]=(byte)0xB1;
        txBuf[1]=(byte)(txBufSize);
        txBuf[2]=(byte)(txBufSize>>8);
        txBuf[3]=(byte)command;

        if (bodySize>0){
            for (i=0;i<bodySize;i++){
                txBuf[4+i]=bodyData[i];
            }
        }

        byte crc=0;
        for (i=0;i<4+bodySize;i++){
            crc+=txBuf[i];
        }
        txBuf[4+bodySize]=crc;
        txBuf[5+bodySize]=(byte)(crc^(byte)0xAA);


        if (isReady()){
            try {
                btOutputStream.write(txBuf,0,txBufSize);
            } catch (IOException e) {
                e.printStackTrace();
                LogDebug(e.toString());
                closeSocket();
            }
        }
    }


    //endregion




}
