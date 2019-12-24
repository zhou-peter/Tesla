package ru.track_it.motohelper;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.os.Build;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import ru.track_it.motohelper.Packets.AbstractInPacket;
import ru.track_it.motohelper.Packets.PacketOut_01;

import static android.content.Context.BLUETOOTH_SERVICE;

final class CommunicationManagerBLE extends BluetoothGattCallback {

    private final static String LOG_TAG = "CommunicationManager";
    public static String HM_10_Service = "0000ffe0-0000-1000-8000-00805f9b34fb";
    public static String HM_10_Module = "0000ffe1-0000-1000-8000-00805f9b34fb";
    public static final UUID NOTIFY_DESCRIPTOR = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    public static final String targetName = "MotoHelpeR";
    final Pattern namePattern = Pattern.compile(targetName);
    private BluetoothLeScanner bluetoothLeScannerAbove21;
    private BluetoothAdapter btAdapter;
    private BluetoothDevice btDevice;
    private BluetoothGatt mBluetoothGatt;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic gattCharacterc;

    private InputStreamBLE btInputStream = new InputStreamBLE();
    private OutputStream btOutputStream = new OutputStreamBLE();
    private boolean socketConnected = false;
    private Context context;

    public CommunicationManagerBLE(Context context) {
        this.context = context;
    }


    static void LogDebug(String text) {
        Log.d(LOG_TAG, text);
    }


    public boolean isReady() {
        return socketConnected;
    }

    public void closeSocket() {
        try {
            socketConnected = false;

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

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            ScanSettings scanSettings = new ScanSettings.Builder()
                    .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                    .build();


            bluetoothLeScannerAbove21 = (((BluetoothManager) context.getSystemService(BLUETOOTH_SERVICE)).getAdapter()).getBluetoothLeScanner();
            bluetoothLeScannerAbove21.startScan(null, scanSettings, above21Scanner);
        } else {
            btAdapter.startLeScan(below21Scanner);
        }


    }

    @TargetApi(21)
    ScanCallback above21Scanner=new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            if (testDevice(device)){
                bluetoothLeScannerAbove21.stopScan(new ScanCallback() {
                    @Override
                    public void onScanResult(int callbackType, ScanResult result) {
                        super.onScanResult(callbackType, result);
                    }
                });
                connectToDevice(device);
            }
        }
    };

    BluetoothAdapter.LeScanCallback below21Scanner = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
            if (testDevice(device)){
                btAdapter.stopLeScan(this);
                connectToDevice(device);
            }
        }
    };


    private boolean testDevice(BluetoothDevice device){
        if (btDevice == null) {
            String name = device.getName();
            Log.d(LOG_TAG, "Device: " + name + " (" + device.getAddress() + ")");
            if (name != null) {
                Matcher m = namePattern.matcher(name);
                if (m.matches()) {
                    btDevice = device;
                    return true;
                }
            }
        }
        return false;
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
                socketConnected = true;
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
            btInputStream.addBytes(data);
        }
    }

    @Override
    public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

    }

    @Override
    public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

    }

    public InputStream getInputStream() {
        return btInputStream;
    }

    public OutputStream getOutputStream() {
        return btOutputStream;
    }


    private class InputStreamBLE extends InputStream {

        private AtomicBoolean awaiting = new AtomicBoolean(true);
        private ConcurrentLinkedQueue<Byte> buffer = new ConcurrentLinkedQueue<>();

        public void addBytes(byte[] buf) {
            for (byte b : buf) {
                buffer.add(b);
            }
            synchronized (this) {
                if (awaiting.get()) {
                    Log.v(LOG_TAG, "notify");
                    notify();
                }
            }
        }

        @Override
        public int available() throws IOException {
            return buffer.size();
        }

        @Override
        public int read() throws IOException {
            while (buffer.isEmpty()) {
                try {
                    synchronized (this) {
                        awaiting.set(true);
                        Log.v(LOG_TAG, "go sleep");
                        wait();
                        Log.v(LOG_TAG, "awaike");
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    socketConnected = false;
                    return -1;
                } finally {
                    awaiting.set(false);
                }
            }
            return buffer.poll();
        }
    }

    private class OutputStreamBLE extends OutputStream {
        private ConcurrentLinkedQueue<Byte> buffer = new ConcurrentLinkedQueue<>();
        final int blePayloadLimit = 20;

        @Override
        public void write(int b) throws IOException {
            buffer.add((byte) b);
        }

        @Override
        public void flush() throws IOException {
            int currentSize = buffer.size();
            while (currentSize > 0) {
                if (currentSize > blePayloadLimit) {
                    currentSize = blePayloadLimit;
                }
                byte[] output = new byte[currentSize];
                for (int i = 0; i < currentSize; i++) {
                    output[i] = buffer.poll();
                }

                gattCharacterc.setValue(output);
                mBluetoothGatt.writeCharacteristic(gattCharacterc);
                mBluetoothGatt.setCharacteristicNotification(gattCharacterc, true);

                currentSize = buffer.size();
            }
        }
    }
}
