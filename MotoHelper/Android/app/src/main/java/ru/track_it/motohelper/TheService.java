package ru.track_it.motohelper;

import android.app.Service;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TheService extends Service
        implements Runnable {
public TheService(){
    super();
}

    Thread thread, looperThread, bluetoothThread, flashingThread;
    BluetoothSPPClient btClient=null;
    static Handler theHandler;
    static String LOG_TAG = "BoilerService";
    boolean shouldStop = false;
    boolean doDial = false;
    String State = STATE_INITIALIZING;
    int StateFlash=STATE_FLASH_IDLE;

    //onPacket_0x01 etc Key - is the 01
    HashMap<Integer, Method> protocolMethods = new java.util.HashMap<Integer, Method>();


    public static final int MSG_DO_POKER = 1;
    public static final int MSG_STOPSERVICE = 2;
    public static final int MSG_DO_FUEL = 3;
    public static final int MSG_SET_CONFIG = 5;
    public static final int MSG_GET_CONFIG = 7;
    public static final int MSG_SERVICE_STATE = 101;
    public static final int MSG_BLUETOOTH_LOG=102;
    public static final int MSG_CONNECTION_LOST=103;
    public static final int MSG_COMMUNICATION_ERROR=104;
    public static final int MSG_BLUETOOTH_SHOULD_ENABLE=105;
    public static final int MSG_BLUETOOTH_SHOULD_PAIR=106;
    public static final int MSG_KEEP_ALIVE = 201;
    public static final int MSG_CONFIG_PAGE = 206;
    public static final int MSG_FLASH_DO = 251;
    public static final int MSG_FLASH_SHOULD_FLASH = 252;
    public static final int MSG_FLASH_PROBLEM = 253;
    public static final int MSG_FLASH_UNSUTABLE = 254;
    public static final int MSG_FLASH_PROGRESS = 255;
    public static final int MSG_FLASH_PAGE_COUNT = 256;
    public static final int MSG_FLASH_DONE = 257;
    public static final int MSG_FLASH_START_PROBLEM= 258;
    public static final String STATE_INITIALIZING = "INITIALIZING";
    public static final String STATE_UNAVALIBLE = "UNAVALIBLE";
    public static final String STATE_WORKING = "WORKING";
    public static final int STATE_FLASH_IDLE = 1000;
    public static final int STATE_FLASH_PREPARE = 1001;
    public static final int STATE_FLASH_WAITFLASHREADY = 1002;
    public static final int STATE_FLASH_FLASHING = 1003;
    public static final int STATE_FLASH_FLASHING_ACK_WAIT=1004;
    public static final int STATE_FLASH_FLASHING_ACK_RECEIVED=1005;


    public void stopService() {
        shouldStop = true;
    }


    @Override
    public void onCreate() {
        super.onCreate();
        try{
            Class<?> c = Class.forName(this.getClass().getName());

            Pattern namePattern=Pattern.compile("onPacket_0x(.*?)");
            for (Method m : c.getDeclaredMethods()){
                String name=m.getName();
                Matcher mat = namePattern.matcher(name);
                if (mat.matches()){
                    String hex=mat.group(1);
                    Integer p =Integer.parseInt(hex,16);
                    protocolMethods.put(p,m);
                }
            }
        }catch(ClassNotFoundException ex){

        }

        thread = new Thread(this);
        thread.start();
        Log.d(LOG_TAG, "onCreate");
    }

    public void onMessage(android.os.Message msg) {
        switch (msg.what) {
            case MSG_DO_POKER:
                byte[] seconds=new byte[1];
                seconds[0]=(byte)msg.arg1;
                if (btClient!=null)
                    btClient.createOutPacketAndSend(0x01,1,seconds);
                break;
            case MSG_DO_FUEL:
                byte[] secondsF=new byte[1];
                secondsF[0]=(byte)msg.arg1;
                if (btClient!=null)
                    btClient.createOutPacketAndSend(0x03,1,secondsF);
                break;
            case MSG_STOPSERVICE:
                shouldStop=true;
                break;
            case MSG_GET_CONFIG:
                if (btClient!=null)
                    btClient.createOutPacketAndSend(0x07,0,null);
                break;
            case MSG_SET_CONFIG:
                if (btClient!=null){
                    byte data[] = (byte[])msg.obj;
                    btClient.createOutPacketAndSend(0x05, data.length, data);
                }
                break;
            case MSG_FLASH_DO:
                if (StateFlash==STATE_FLASH_IDLE){
                    StateFlash=STATE_FLASH_PREPARE;
                    flashingThread= new Thread( new Runnable() { @Override public void run() {
                        flashDevice();
                    } });
                    flashingThread.start();
                    Log.d(LOG_TAG, "Flash process start");
                }else{
                    sendEmptyMessage(MSG_FLASH_PROBLEM);
                }
                break;
        }
    }
    public static int getU8(byte b){
        return b & 0xff;
    }
    public static int getU32(byte[] buf, int offset){
        int tmp=0;
        tmp|=getU8(buf[offset]);
        tmp|=getU8(buf[offset+1])<<8;
        tmp|=getU8(buf[offset+2])<<16;
        tmp|=getU8(buf[offset+3])<<24;
        return tmp;
    }
    public static int getU16(byte[] buf, int offset){
        int tmp=0;
        tmp|=getU8(buf[offset]);
        tmp|=getU8(buf[offset+1])<<8;
        return tmp;
    }
    public static boolean compareU8(byte a, byte b){
        return getU8(a)==getU8(b);
    }
    public static boolean compareU16(int a, int b){
        return (a&0x0000FFFF)==(b&0x0000FFFF);
    }
    public static void u16toBuf(byte[] buf, int offset, Integer...  params){
        for(int i : params){
            buf[offset++]= (byte)i;
            buf[offset++]=(byte) (i >> 8);
        }
    }


    public static StringBuffer getHexFromBytes(byte[] data) {
        StringBuffer sb=new StringBuffer((data.length*3)+4);
        for(byte b : data){
            sb.append(String.format("%02x", b));
            sb.append(" ");
        }
        return sb;
    }
    public static StringBuffer getHexFromInt(int data) {
        StringBuffer sb=new StringBuffer(10);
        sb.append(String.format("%02x", data));
        return sb;
    }
    //return string 0A from 10 value
    public static String getHexFromByte(byte b){
        return getHexFromBytes(new byte[]{b}).toString();
    }
    int isAliveCounter=0;
    @Override
    public void run() {

        try {
            //db ready, server ready, Ready for a messages
            looperThread = new LooperThread();
            looperThread.start();

            Looper.prepare();

            btClient=new BluetoothSPPClient(this);
            bluetoothThread = new Thread(btClient);
            bluetoothThread.setName("Bluetooth thread");
            bluetoothThread.start();


            State = STATE_WORKING;
            while (true) {
                if (shouldStop) break;

                Thread.sleep(100);
                isAliveCounter++;
                //5sec no keep alive - Connection lost
                if (isAliveCounter>50){
                    Message msg=new Message();
                    msg.what=TheService.MSG_CONNECTION_LOST;
                    sendMessage(msg);
                    isAliveCounter=0;
                }
                //если блютуз клиент вылетел
                if (!this.shouldStop && btClient.shouldStop){
                    btClient=new BluetoothSPPClient(this);
                    bluetoothThread = new Thread(btClient);
                    bluetoothThread.start();
                }
            }
            if (btClient!=null)btClient.shouldStop=true;
            State = STATE_UNAVALIBLE;
            stopSelf();
        } catch (Exception ex) {
            ex.printStackTrace();
            Log.e(LOG_TAG, "service main run", ex);
            State = STATE_UNAVALIBLE;
            stopSelf();
        }
    }


    //обработка пакета блютуз. (В потоке блютуз)
    //если долго, то запускать в другом потоке - не задерживать
    public void onPacket(byte[] rxBuf)  {
        int packetType=getU8(rxBuf[3]);
        int packetSize=getU8(rxBuf[1])+getU8(rxBuf[2])*256;
        int bodySize=packetSize-6;
        byte[] body=null;
        if (bodySize>0){
            body=new byte[bodySize];
            System.arraycopy(rxBuf,4,body,0,bodySize);
        }

        //create string type onPacket_0xBA
        String functionName="onPacket_0x"+getHexFromByte((byte)packetType);
        Method m =null;
        try {

            m=protocolMethods.get(packetType);
            if (m!=null) m.invoke (this, body);
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }catch(Exception ex){
            ex.printStackTrace();
            Log.e(LOG_TAG,"There is not function " + functionName);
        }



       /* switch(packetType){
            case 0x04:
                onPacket_0x04(body);
                break;
            case 0x06:
                onPacket_0x06(body);
                break;
            default:
                Log.d(LOG_TAG,"Unknown packet "+ packetType);
        }*/
    }

    public void onPacket_0x02(byte[] data){
        byte problemCode=data[0];
        sendIntMessage(MSG_COMMUNICATION_ERROR,problemCode);
    }
    public void onPacket_0x04(byte[] data){
        Message msg=new Message();
        msg.what=TheService.MSG_KEEP_ALIVE;
        msg.obj=data;
        sendMessage(msg);
        isAliveCounter=0;
    }

    public void onPacket_0x06(byte[] data){
        Message msg=new Message();
        msg.what=TheService.MSG_CONFIG_PAGE;
        msg.obj=data;
        sendMessage(msg);
    }
    public void onPacket_0xB2(byte[] data){
        if (flashingThread==null){
            //если битая прошивка и процесс по прошивке
            //и не запускался
            sendEmptyMessage(MSG_FLASH_SHOULD_FLASH);
            return;
        }
        if (StateFlash == STATE_FLASH_WAITFLASHREADY){
            StateFlash=STATE_FLASH_FLASHING;
            synchronized(flashingThread) {
                flashingThread.notify();
            }
        }
        if (StateFlash == STATE_FLASH_IDLE){
            sendEmptyMessage(MSG_FLASH_SHOULD_FLASH);
        }
    }
    //адрес по которому надо было прошить крайний раз
    int fwLastFlashAddress=0;
    boolean fwLastPageFlashed=false;
    int fwCRC=0;
    public void onPacket_0xB8(byte[] data){
        if (StateFlash == STATE_FLASH_IDLE){
            sendEmptyMessage(MSG_FLASH_SHOULD_FLASH);
            return;
        }
        if (StateFlash == STATE_FLASH_FLASHING_ACK_WAIT){
            if (getU32(data, 0)==fwLastFlashAddress){
                if (data[4]==1){
                    fwLastPageFlashed=true;
                }else{
                    fwLastPageFlashed=false;
                }
                StateFlash=STATE_FLASH_FLASHING_ACK_RECEIVED;
            }
            synchronized(flashingThread) {
                flashingThread.notify();
            }
        }
    }
    void addPageToCRC(byte[] buf, int offset){
        for(int i=offset;i<buf.length;i++){
            fwCRC+=buf[i];
        }
        fwCRC&=0xFFFF;
    }
    public void flashDevice(){
        byte[] pageBuf=new byte[1024+4];
        int baseAddress=0x08001800;

        InputStream configStream = null;
        InputStream flashStream=null;

        try {

            configStream=null;//this.getResources().openRawResource(R.raw.conf);
            flashStream=null;//this.getResources().openRawResource(R.raw.flash);
            float flashSize=(float)flashStream.available()/1024F;
            int pageCount=(int)Math.ceil(flashSize);
            fwCRC=0;
            if (configStream.read(pageBuf,0,1024)>100){
                if (pageCount>0 && pageCount<60){
                    sendIntMessage(MSG_FLASH_PAGE_COUNT, pageCount);
                    StateFlash = STATE_FLASH_WAITFLASHREADY;
                    int tryCount=0;
                    while(true) {
                        if (btClient != null && btClient.isReady()) {
                            btClient.createOutPacketAndSend(0xB3, 0, null);
                        }
                        Thread.sleep(200);
                        //будет изменено входящим пакетом Б2
                        if (StateFlash == STATE_FLASH_FLASHING)break;
                        Thread.sleep(200);
                        tryCount++;
                        if (tryCount>3){
                            sendEmptyMessage(MSG_FLASH_START_PROBLEM);
                            return;
                        }
                    }
                }else{
                    Log.d(LOG_TAG, "Invalid flash size");
                    return;
                }
                //StateFlash == STATE_FLASH_FLASHING

                fwLastFlashAddress=baseAddress;
                for (int i=0;i<pageCount;i++){
                    flashStream.read(pageBuf,4,1024);
                    fwLastPageFlashed=false;
                    //ожидаем готовности bluetooth
                    int tryCount=0;
                    while(true) {
                        if (flashPage(pageBuf)) {
                            //переходим к следующей странице
                            //если и прошивка и верификация прошла успешно
                            if (fwLastPageFlashed) break;
                        }
                        tryCount++;
                        if (tryCount>3){
                            sendIntMessage(MSG_FLASH_PROBLEM, fwLastFlashAddress);
                            return;
                        }
                    }
                    fwLastFlashAddress+=0x400;
                    //отправляем прогрессбару сообщение
                    sendIntMessage(TheService.MSG_FLASH_PROGRESS, i);
                    addPageToCRC(pageBuf, 4);
                }
                //all pages are flashed
                //flash config page
                configStream.reset();
                configStream.read(pageBuf,4,1024);
                fwLastFlashAddress=0x0800FC00;
                fwLastPageFlashed=false;
                //Устанавливаем количество страниц
                pageBuf[10+4]=(byte)pageCount;
                //Устанавливаем CRC
                pageBuf[2+4]=(byte)fwCRC;
                pageBuf[3+4]=(byte)(fwCRC>>8);
                int tryCount=0;
                while(true) {
                    if (flashPage(pageBuf)) {
                        //переходим к следующей странице
                        //если и прошивка и верификация прошла успешно
                        if (fwLastPageFlashed) break;
                    }
                    tryCount++;
                    if (tryCount>3){
                        sendIntMessage(MSG_FLASH_PROBLEM, fwLastFlashAddress);
                        return;
                    }
                }

                //На этот момент все страницы прошиты (включая конфиг-страницу)


                //на шару отправляем пакет чтобы перезагрузился и не ждем подтверждения
                while(true) {
                    if (btClient != null && btClient.isReady()) {
                        btClient.createOutPacketAndSend(0xB9, 0, null);
                        Thread.sleep(200);
                        break;
                    } else {
                        Thread.sleep(200);
                    }
                }
                StateFlash =STATE_FLASH_IDLE;
                sendEmptyMessage(MSG_FLASH_DONE);
            }

        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }catch (Resources.NotFoundException e){
            e.printStackTrace();
        }finally {
            StateFlash=STATE_FLASH_IDLE;
            try {
                if (configStream!=null) configStream.close();
                if (flashStream!=null)flashStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }


    }

    private boolean flashPage(byte[] pageBuf) throws InterruptedException {
        pageBuf[0]=(byte)fwLastFlashAddress;
        pageBuf[1]=(byte)(fwLastFlashAddress>>8);
        pageBuf[2]=(byte)(fwLastFlashAddress>>16);
        pageBuf[3]=(byte)(fwLastFlashAddress>>24);
        Log.d(LOG_TAG,"ADDRESS " + getHexFromInt(fwLastFlashAddress));
        if (btClient != null && btClient.isReady()) {
            btClient.createOutPacketAndSend(0xB7, 1024+4, pageBuf);
            StateFlash = STATE_FLASH_FLASHING_ACK_WAIT;
            Thread.sleep(2000);
            //нас разбудят пораньше чем через 2 секунды если придет
            //подтверждение
        } else {
            Thread.sleep(200);
        }
        if (StateFlash == STATE_FLASH_FLASHING_ACK_RECEIVED){
            StateFlash = STATE_FLASH_FLASHING;
            return true;
        }
        return false;
    }
    void sendEmptyMessage(int what){
        if (binder.currentActivity!=null){
            binder.currentActivity.getServiceHandler().sendEmptyMessage(what);
        }
    }
    void sendIntMessage(int what, int arg1){
        Message msg=new Message();
        msg.what=what;
        msg.arg1=arg1;
        sendMessage(msg);
    }
    protected boolean sendMessage(Message msg){
        if (binder.currentActivity!=null){
            binder.currentActivity.getServiceHandler().sendMessage(msg);
            return true;
        }
        return false;
    }
//region subclasses
    @Override
    public IBinder onBind(Intent intent) {

        return binder;
    }

    interface IServiceClient {
        Handler serviceHandler=null;
        public Handler getServiceHandler();
        public void sendEmptyMessage(int what);
    }

    TheBinder binder=new TheBinder();
    class TheBinder extends Binder {
        TheService getService() {
            return TheService.this;
        }

        IServiceClient currentActivity;
        public void setCurrentActivity(IServiceClient activity) {
            currentActivity=activity;
        }
    }

    //класс для приема сообщений от Активити
    class LooperThread extends Thread {
        public void run() {
            Looper.prepare();
            theHandler=new Handler(){
                @Override
                public void handleMessage(Message msg) {
                    onMessage(msg);
                }
            };
            Looper.loop();
        }
    }
//endregion
}
