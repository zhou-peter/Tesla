package ru.track_it.motohelper;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.locks.ReentrantLock;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

final class CommunicationManager {
    //private final static UUID SERIAL_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private final static String LOG_TAG = "CommunicationManager";
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket socket = null;
    private InputStream btInputStream = null;
    private OutputStream btOutputStream = null;
    private boolean socketConnected = false;
    private static final CommunicationManager instance = new CommunicationManager();

    private CommunicationManager() {

    }

    public static CommunicationManager getInstance() {
        return instance;
    }

    static void LogDebug(String text) {
        Log.d(LOG_TAG, text);
    }


    public boolean isReady() {
        if (socket == null
                || btInputStream == null
                || btOutputStream == null) return false;
        //if (!socket.isConnected())return false;
        /*By the way, I found that on some android devices isConnected()
        always returns false. In such case just try to write something to socket
        and check if there is no exception.*/
        return socketConnected;
    }

    public void closeSocket() {
        try {
            socketConnected = false;
            if (btInputStream != null) btInputStream.close();
            if (btOutputStream != null) btOutputStream.close();
            if (socket != null) socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public synchronized void Connect() {
        //close existing connection
        closeSocket();

        btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) {
            LogDebug("Bluetooth adapter is not available.");
            return;
        }
        Log.d(LOG_TAG, "Bluetooth adapter is found.");

        if (!btAdapter.isEnabled()) {
            LogDebug("Bluetooth is disabled. Check configuration.");
            return;
        }
        Log.d(LOG_TAG, "Bluetooth is enabled.");

        if (!btAdapter.isDiscovering()) btAdapter.startDiscovery();
        BluetoothDevice btDevice = null;
        Set<BluetoothDevice> bondedDevices = btAdapter.getBondedDevices();

        Pattern namePattern = Pattern.compile("MotoHelpeR");
        if (btDevice == null) {
            for (BluetoothDevice dev : bondedDevices) {
                String name = dev.getName();
                Log.d(LOG_TAG, "Paired device: " + name + " (" + dev.getAddress() + ")");
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


        btAdapter.cancelDiscovery();

        try {
            socket = btDevice.createRfcommSocketToServiceRecord(UUID.randomUUID());
        } catch (Exception e) {
            LogDebug("Error creating socket");
        }

        try {
            socket.connect();
            socketConnected = true;
            LogDebug("Connected");
        } catch (IOException e) {
            LogDebug(e.getMessage());
            try {
                LogDebug("trying fallback...");

                socket = (BluetoothSocket) btDevice.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(btDevice, 1);
                socket.connect();
                socketConnected = true;
                LogDebug("Connected");
            } catch (Exception e2) {
                LogDebug("Couldn't establish Bluetooth connection!");
            }
        }

        try {
            if (socketConnected) {
                btInputStream = socket.getInputStream();
                btOutputStream = socket.getOutputStream();
                btOutputStream.write(0);
            }
        } catch (Exception ex) {
            socketConnected = false;
        }

    }


    public InputStream getInputStream() {
        return btInputStream;
    }

    public OutputStream getOutputStream() {
        return btOutputStream;
    }
}
