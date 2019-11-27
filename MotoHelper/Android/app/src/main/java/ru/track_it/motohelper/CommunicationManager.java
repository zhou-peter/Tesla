package ru.track_it.motohelper;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Message;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

final class CommunicationManager {

    private final static String LOG_TAG="CommunicationManager";
    private final static UUID sppUuid = UUID.fromString("02001101-0000-1020-8000-00805F9B34FB");
    BluetoothAdapter btAdapter=null;
    BluetoothSocket btSocket=null;
    InputStream btInputStream=null;
    OutputStream btOutputStream=null;
    boolean socketClosed=false;

    static void LogDebug(String text){
        Log.d(LOG_TAG, text);
    }


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


    public void Connect(){
        btAdapter=BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) {
            LogDebug("Bluetooth adapter is not available.");
            return;
        }
        Log.d(LOG_TAG,"Bluetooth adapter is found.");

        if (!btAdapter.isEnabled()) {
            LogDebug("Bluetooth is disabled. Check configuration.");
            return;
        }
        Log.d(LOG_TAG,"Bluetooth is enabled.");

        if (!btAdapter.isDiscovering())btAdapter.startDiscovery();
        BluetoothDevice btDevice = null;
        Set<BluetoothDevice> bondedDevices = btAdapter.getBondedDevices();

        Pattern namePattern=Pattern.compile("MotoHelpeR");
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
    }




}
