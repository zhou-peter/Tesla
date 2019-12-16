package com.reb.dsd_ble.ble.profile;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.util.Log;
import com.reb.dsd_ble.ble.profile.utility.BleConfiguration;
import com.reb.dsd_ble.util.DebugLog;
import com.reb.dsd_ble.util.HexStringConver;
import java.util.Arrays;

public class BleCore {
    private static final String ERROR_CONNECTION_STATE_CHANGE = "Error on connection state change";
    private static final String ERROR_DISCOVERY_SERVICE = "Error on discovering services";
    private static final String ERROR_WRITE_DESCRIPTOR = "Error on writing descriptor";
    private static final String TAG = "BleManager";
    private static BleCore mInstances;
    /* access modifiers changed from: private */
    public String address;
    /* access modifiers changed from: private */
    public BluetoothGatt mBluetoothGatt;
    /* access modifiers changed from: private */
    public BleManagerCallbacks mCallbacks;
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (status != 0) {
                BleCore.this.onError(BleCore.ERROR_CONNECTION_STATE_CHANGE, status);
            } else if (newState == 2) {
                Log.i(BleCore.TAG, "Device connected");
                BleCore.this.mBluetoothGatt.discoverServices();
                BleCore.this.mIsConnected = true;
                BleCore.this.mCallbacks.onDeviceConnected();
            } else if (newState == 0) {
                Log.i(BleCore.TAG, "Device disconnected, mUserDisConnect: " + BleCore.this.mUserDisConnect);
                if (BleCore.this.mUserDisConnect) {
                    BleCore.this.mCallbacks.onDeviceDisconnected();
                } else {
                    BleCore.this.mCallbacks.onLinklossOccur(BleCore.this.address);
                    BleCore.this.mUserDisConnect = false;
                }
                BleCore.this.closeBluetoothGatt();
            }
        }

        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            DebugLog.m10i("status:" + status);
            if (status == 0) {
                BluetoothGattCharacteristic mCharacteristic = null;
                for (BluetoothGattService service : gatt.getServices()) {
                    if (service.getUuid().equals(BleConfiguration.SERVICE_BLE_SERVICE2)) {
                        mCharacteristic = service.getCharacteristic(BleConfiguration.WRITE_LONG_DATA_CHARACTERISTIC2);
                        DebugLog.m10i("service is found------" + mCharacteristic);
                    }
                }
                if (mCharacteristic == null) {
                    BleCore.this.mCallbacks.onDeviceNotSupported();
                    gatt.disconnect();
                    return;
                }
                BleCore.this.mCallbacks.onServicesDiscovered();
                BleCore.this.enableNotification(gatt);
                return;
            }
            BleCore.this.onError(BleCore.ERROR_DISCOVERY_SERVICE, status);
        }

        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            Log.e(BleCore.TAG, "onCharacteristicRead---->");
        }

        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            Log.e(BleCore.TAG, "onDescriptorWrite---->" + Arrays.toString(descriptor.getValue()) + "status:" + status);
            if (status != 0) {
                BleCore.this.onError(BleCore.ERROR_WRITE_DESCRIPTOR, status);
            } else if (descriptor.getUuid().equals(BleConfiguration.NOTIFY_DESCRIPTOR) && descriptor.getValue()[0] == BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE[0]) {
                BleCore.this.mCallbacks.onNotifyEnable();
            }
        }

        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            Log.d(BleCore.TAG, "onCharacteristicChanged---->");
            BleCore.this.mCallbacks.onRecive(characteristic.getValue());
        }

        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            Log.i(BleCore.TAG, "onCharacteristicWrite---->" + characteristic.getUuid().equals(BleConfiguration.WRITE_LONG_DATA_CHARACTERISTIC2) + "******" + (BleCore.this.mCallbacks != null) + "*****" + characteristic.getUuid().equals(BleConfiguration.WRITE_LONG_DATA_CHARACTERISTIC2));
            if (characteristic.getUuid().equals(BleConfiguration.WRITE_LONG_DATA_CHARACTERISTIC2)) {
                byte[] value = characteristic.getValue();
                if (status == 0) {
                    DebugLog.m10i("write:" + HexStringConver.bytes2HexStr(value));
                    BleConfiguration.mDataSend.updateCmdState(0);
                    if (BleConfiguration.mDataSend.isLastChunk()) {
                        BleCore.this.mCallbacks.onWriteSuccess(value, true);
                        BleConfiguration.mDataSend.sendNextCmd(false);
                        return;
                    }
                    return;
                }
                BleConfiguration.mDataSend.updateCmdState(0);
                if (BleConfiguration.mDataSend.isLastChunk()) {
                    BleCore.this.mCallbacks.onWriteSuccess(value, false);
                    BleConfiguration.mDataSend.sendNextCmd(true);
                    return;
                }
                BleConfiguration.mDataSend.updateCmdState(7);
            }
        }

        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            super.onReadRemoteRssi(gatt, rssi, status);
            if (status == 0) {
                BleCore.this.mCallbacks.onReadRssi(rssi);
            } else {
                BleCore.this.mBluetoothGatt.readRemoteRssi();
            }
        }
    };
    /* access modifiers changed from: private */
    public boolean mIsConnected;
    /* access modifiers changed from: private */
    public boolean mUserDisConnect;

    public static BleCore getInstances() {
        if (mInstances == null) {
            synchronized (BleCore.class) {
                if (mInstances == null) {
                    mInstances = new BleCore();
                }
            }
        }
        return mInstances;
    }

    public boolean connect(Context context, BluetoothDevice device) {
        Log.i(TAG, "connect--" + device.getAddress());
        if (!isConnected()) {
            this.address = device.getAddress();
            if (this.mBluetoothGatt != null) {
                return this.mBluetoothGatt.connect();
            }
            this.mBluetoothGatt = device.connectGatt(context, false, this.mGattCallback);
            return true;
        } else if (device.getAddress().equals(this.address)) {
            Log.d(TAG, "是同一个设备，忽略掉");
            if (!this.mIsConnected) {
                return true;
            }
            this.mCallbacks.onDeviceConnected();
            return true;
        } else {
            Log.d(TAG, "不同设备，先断开");
            disConnect(false);
            this.mBluetoothGatt = null;
            return true;
        }
    }

    public boolean connect(Context context, String address2) {
        if (BluetoothAdapter.checkBluetoothAddress(address2)) {
            BluetoothAdapter adapter = ((BluetoothManager) context.getSystemService("bluetooth")).getAdapter();
            if (adapter != null) {
                return connect(context, adapter.getRemoteDevice(address2));
            }
        }
        return false;
    }

    public void disconnect() {
        disConnect(true);
    }

    private void disConnect(boolean userDisconnect) {
        this.mUserDisConnect = userDisconnect;
        if (this.mBluetoothGatt != null) {
            this.mBluetoothGatt.disconnect();
        }
    }

    public void setGattCallbacks(BleManagerCallbacks callbacks) {
        this.mCallbacks = callbacks;
    }

    public boolean isConnected() {
        return this.mIsConnected;
    }

    public void closeBluetoothGatt() {
        BleConfiguration.mDataSend.resetAllState();
        if (this.mBluetoothGatt != null) {
            this.mBluetoothGatt.close();
            this.mBluetoothGatt = null;
        }
        this.mIsConnected = false;
    }

    /* access modifiers changed from: private */
    public void enableNotification(BluetoothGatt gatt) {
        BluetoothGattCharacteristic notifyCha = this.mBluetoothGatt.getService(BleConfiguration.SERVICE_BLE_SERVICE2).getCharacteristic(BleConfiguration.NOTIFY_LONG_DATA_CHARACTERISTIC1);
        gatt.setCharacteristicNotification(notifyCha, true);
        BluetoothGattDescriptor descriptor = notifyCha.getDescriptor(BleConfiguration.NOTIFY_DESCRIPTOR);
        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        gatt.writeDescriptor(descriptor);
    }

    /* access modifiers changed from: protected */
    public void onError(String errorWriteDescriptor, int status) {
        disConnect(false);
    }

    public boolean readRssi() {
        if (!this.mIsConnected || this.mBluetoothGatt == null) {
            return false;
        }
        return this.mBluetoothGatt.readRemoteRssi();
    }

    public boolean sendData(byte[] cmd) {
        DebugLog.m10i("mIsConnected:" + this.mIsConnected + "," + Arrays.toString(cmd));
        if (!this.mIsConnected) {
            return false;
        }
        BleConfiguration.mDataSend.sendData(this.mBluetoothGatt, cmd);
        return true;
    }
}
