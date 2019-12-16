package com.reb.dsd_ble.ble.profile.utility;

import android.content.SharedPreferences;
import com.reb.dsd_ble.ble.profile.DataSend;
import com.reb.dsd_ble.constant.ShareString;
import com.reb.dsd_ble.p004ui.DsdApplication;
import java.util.UUID;

public class BleConfiguration {
    public static long AUTO_INTERVAL = 1000;
    public static final boolean IS_AUTO_CONNECT = false;
    public static final int MAX_CONNECTED_DEVICES = 3;
    public static final UUID NOTIFY_DESCRIPTOR = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    public static UUID NOTIFY_LONG_DATA_CHARACTERISTIC1 = null;
    public static final int SCAN_PERIOD = 15000;
    public static UUID SERVICE_BLE_SERVICE2 = null;
    public static UUID SERVICE_UUID_OF_SCAN_FILTER1 = null;
    public static UUID WRITE_LONG_DATA_CHARACTERISTIC2 = null;
    public static final int WRITE_LONG_DATA_CHARACTERISTIC_WRITE_TYPE = 1;
    public static DataSend mDataSend = new DataSend(SERVICE_BLE_SERVICE2, WRITE_LONG_DATA_CHARACTERISTIC2, 1);

    public static void init(DsdApplication context) {
        SharedPreferences share = context.getSharedPreferences(ShareString.FILE_NAME, 0);
        String string = share.getString(ShareString.SCAN_FILTER_SERVICE_UUID, "ffe0");
        String service = share.getString(ShareString.SAVE_SERVICE_UUID, "ffe0");
        String writeCharactor = share.getString(ShareString.SAVE_CHARACT_UUID, "ffe1");
        String notifyCharactor = share.getString(ShareString.SAVE_NOTIFY_UUID, "ffe1");
        String head = "0000";
        String end = "-0000-1000-8000-00805f9b34fb";
        SERVICE_BLE_SERVICE2 = UUID.fromString(head + service + end);
        WRITE_LONG_DATA_CHARACTERISTIC2 = UUID.fromString(head + writeCharactor + end);
        NOTIFY_LONG_DATA_CHARACTERISTIC1 = UUID.fromString(head + notifyCharactor + end);
        mDataSend = new DataSend(SERVICE_BLE_SERVICE2, WRITE_LONG_DATA_CHARACTERISTIC2, 1);
        AUTO_INTERVAL = share.getLong(ShareString.SAVE_AUTO_INTERVAL, AUTO_INTERVAL);
    }
}