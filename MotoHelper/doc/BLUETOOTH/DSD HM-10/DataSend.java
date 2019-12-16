package com.reb.dsd_ble.ble.profile;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.util.Log;
import com.reb.dsd_ble.ble.profile.task.AbTaskItem;
import com.reb.dsd_ble.ble.profile.task.AbTaskListener;
import com.reb.dsd_ble.ble.profile.task.AbTaskQueue;
import com.reb.dsd_ble.ble.profile.utility.BleDataTypeUtils;
import com.reb.dsd_ble.util.DebugLog;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;

public class DataSend {
    public static final int STATE_DATA_SEND_ERROR = 7;
    public static final int STATE_DATA_SEND_IDLE = 0;
    private static final String TAG = "DataSend";
    private static final int chunkSize = 20;
    private final int STATE_DATA_SENDING = 1;
    private final int STATE_DATA_SEND_RESPONSE_ACK = 2;
    private final int STATE_DATA_SEND_RESPONSE_NACK = 3;
    private final int STATE_DATA_SEND_TIMOUT = 4;
    private final int STATE_LONG_DATA_SENDING = 6;
    private final int STATE_LONG_DATA_SEND_IDLE = 5;
    private boolean isLastChunk = false;
    private boolean is_long_data = false;
    private final AbTaskQueue mAbTaskQueue = AbTaskQueue.getInstance();
    private BluetoothGatt mGatt = null;
    private UUID mLongDataCharacteristic = null;
    private int mLongDataCharacteristicWriteType = 1;
    private Object mSendThreadLock = new Object();
    private int mServiceState = 0;
    public UUID mServiceUuid = null;
    private List<AbTaskItem> mTaskItems = new LinkedList();
    private Object object = new Object();
    private int totalChunk;

    public DataSend(UUID mServiceUuid2, UUID mLongDataCharacteristic2, int mLongDataCharacteristicWriteType2) {
        this.mServiceUuid = mServiceUuid2;
        this.mLongDataCharacteristic = mLongDataCharacteristic2;
        this.mLongDataCharacteristicWriteType = mLongDataCharacteristicWriteType2;
    }

    public void sendData(BluetoothGatt gatt, final byte[] cmd) {
        if (cmd == null || gatt == null || cmd.length == 0) {
            DebugLog.m10i("sendData cmd error");
            return;
        }
        this.mGatt = gatt;
        AbTaskItem item = new AbTaskItem();
        item.setListener(new AbTaskListener() {
            public void update() {
            }

            public void get() {
                DataSend.this.startSendLongThread(cmd);
            }
        });
        synchronized (this.object) {
            DebugLog.m10i("mTaskItems.size:" + this.mTaskItems.size());
            if (this.mTaskItems.size() == 0) {
                this.mTaskItems.add(item);
                this.mAbTaskQueue.execute((AbTaskItem) this.mTaskItems.get(0));
            } else {
                this.mTaskItems.add(item);
            }
        }
    }

    private void sendCmd2Ble(byte[] cmd) {
        synchronized (this.mSendThreadLock) {
            try {
                BluetoothGatt bg = this.mGatt;
                if (bg == null) {
                    DebugLog.m10i("BluetoothGatt is null");
                    updateCmdState(0);
                    return;
                }
                BluetoothGattService bgs = bg.getService(this.mServiceUuid);
                if (bgs == null) {
                    DebugLog.m10i("GattAttributes.SMALLRADAR_BLE_SERVICE is null");
                    updateCmdState(0);
                    return;
                }
                this.mServiceState = 1;
                BluetoothGattCharacteristic characteristic = bgs.getCharacteristic(this.mLongDataCharacteristic);
                if (characteristic == null) {
                    updateCmdState(0);
                    DebugLog.m10i("GattAttributes.SMALLRADAR_BLE_SERVICE_WRITE_NOTIFY_CHARACTERISTIC is null,mLongDataCharacteristic:" + this.mLongDataCharacteristic);
                    return;
                }
                characteristic.setValue(cmd);
                DebugLog.m10i("write  result: " + bg.writeCharacteristic(characteristic) + ",cmd: " + BleDataTypeUtils.bytesToHexString(cmd));
            } catch (Exception e) {
                DebugLog.m10i("write failed,characteristic is  not exist ");
                updateCmdState(0);
            }
        }
    }

    /* access modifiers changed from: private */
    public void startSendLongThread(byte[] cmd) {
        if (cmd.length % 20 > 0) {
            this.totalChunk = (cmd.length / 20) + 1;
        } else {
            this.totalChunk = cmd.length / 20;
        }
        this.isLastChunk = false;
        this.is_long_data = true;
        int ChunkNumble = 0;
        while (!this.isLastChunk) {
            switch (this.mServiceState) {
                case 1:
                    break;
                case 7:
                    updateCmdState(0);
                    sendNextCmd(true);
                    return;
                default:
                    DebugLog.m10i("发送第" + ChunkNumble + "个包");
                    if (this.totalChunk <= ChunkNumble + 1) {
                        this.isLastChunk = true;
                        if (cmd.length % 20 > 0) {
                            sendCmd2Ble(BleDataTypeUtils.bytesCut(cmd, ChunkNumble * 20, cmd.length % 20));
                        } else {
                            sendCmd2Ble(BleDataTypeUtils.bytesCut(cmd, ChunkNumble * 20, 20));
                        }
                        ChunkNumble++;
                        DebugLog.m10i("发送结束");
                        break;
                    } else {
                        this.isLastChunk = false;
                        sendCmd2Ble(BleDataTypeUtils.bytesCut(cmd, ChunkNumble * 20, 20));
                        ChunkNumble++;
                        break;
                    }
            }
        }
    }

    public boolean isLastChunk() {
        return this.isLastChunk;
    }

    private void addTaskItems(AbTaskItem item) {
        synchronized (this.object) {
            this.mTaskItems.add(item);
        }
    }

    private void removeTaskItems(int item) {
        synchronized (this.object) {
            this.mTaskItems.remove(item);
        }
    }

    public void clearTaskItems() {
        synchronized (this.object) {
            if (this.mTaskItems != null) {
                this.mTaskItems.clear();
            }
        }
    }

    public void updateCmdState(int newState) {
        this.mServiceState = newState;
    }

    public void resetAllState() {
        this.isLastChunk = true;
        this.mServiceState = 0;
        clearTaskItems();
    }

    public void sendNextCmd(boolean sendfail) {
        Log.i(TAG, "BleManager sendNextCmd");
        if (this.mTaskItems.size() > 0) {
            if (!sendfail) {
                Log.i(TAG, "BleManager sendNextCmd,remove");
                removeTaskItems(0);
            }
            if (this.mTaskItems.size() > 0) {
                Log.i(TAG, "BleManager sendNextCmd,execute");
                this.mAbTaskQueue.execute((AbTaskItem) this.mTaskItems.get(0));
            }
        }
    }
}
