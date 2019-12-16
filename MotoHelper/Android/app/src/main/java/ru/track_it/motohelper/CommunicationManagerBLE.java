package ru.track_it.motohelper;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.List;
import java.util.Set;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import ru.track_it.motohelper.Packets.PacketOut_01;

final class CommunicationManagerBLE extends BluetoothGattCallback implements BluetoothAdapter.LeScanCallback {
    //private final static UUID SERIAL_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private final static String LOG_TAG = "CommunicationManager";
    private static HashMap<String, String> attributes = new HashMap();
    public static String HM_10_Service = "0000ffe0-0000-1000-8000-00805f9b34fb";
    public static String HM_10_Module = "0000ffe1-0000-1000-8000-00805f9b34fb";
    public static final UUID NOTIFY_DESCRIPTOR = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    static {
        attributes.put(HM_10_Service, "HM-10 Service");
        attributes.put(HM_10_Module, "HM-10 Module");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }

    private BluetoothAdapter btAdapter = null;
    private BluetoothDevice btDevice;
    private BluetoothGatt mBluetoothGatt;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic gattCharacterc;
    private BluetoothSocket socket = null;
    private InputStream btInputStream = null;
    private OutputStream btOutputStream = null;
    private boolean socketConnected = false;
    private Context context;

    public CommunicationManagerBLE(Context context) {
        this.context = context;
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


        btAdapter.startLeScan(this);


    }

    @Override
    public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
        if (btDevice == null) {
            String name = device.getName();
            Log.d(LOG_TAG, "Device: " + name + " (" + device.getAddress() + ")");
            if (name != null) {
                final Pattern namePattern = Pattern.compile("MotoHelpeR");
                Matcher m = namePattern.matcher(name);
                if (m.matches()) {
                    btDevice = device;
                    btAdapter.stopLeScan(this);
                    connectToDevice(device);
                }
            }
        }
    }

    private void connectToDevice(BluetoothDevice btDevice) {
        try {
            mBluetoothGatt = btDevice.connectGatt(context, true, this);
        } catch (Exception e) {
            LogDebug("Error creating socket");
        }
    }

    @Override
    public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
        if (newState == BluetoothProfile.STATE_CONNECTED) {
            mBluetoothGatt.discoverServices();
        }
    }

    @Override
    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
        if (status == BluetoothGatt.GATT_SUCCESS) {
            service = gatt.getService(UUID.fromString(HM_10_Service));
            if (service != null) {
                gattCharacterc = service.getCharacteristic(UUID.fromString(HM_10_Module));
                gatt.setCharacteristicNotification(gattCharacterc, true);
                BluetoothGattDescriptor descriptor = gattCharacterc.getDescriptor(NOTIFY_DESCRIPTOR);
                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                gatt.writeDescriptor(descriptor);
                gatt.readCharacteristic(gattCharacterc);
            }
        } else {
            Log.w(LOG_TAG, "onServicesDiscovered received: " + status);
        }
    }

    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
        byte[] data = characteristic.getValue();
        if (data != null && data.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(data.length);
            for (byte byteChar : data) {
                stringBuilder.append(String.format("%02X ", byteChar));
            }
            Log.d(LOG_TAG, stringBuilder.toString());
        }
    }

    @Override
    public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

    }




    public InputStream getInputStream() {
        return btInputStream;
    }

    public OutputStream getOutputStream() {
        return btOutputStream;
    }


    public static class InputStreamBLE extends InputStream {

        @Override
        public int read() throws IOException {
            return 0;
        }
    }


}
