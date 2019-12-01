package ru.track_it.motohelper.Packets;

import java.util.HashMap;
import java.util.Map;

import ru.track_it.motohelper.AccelData;
import ru.track_it.motohelper.Data;

public class PacketIn_11 extends AbstractInPacket {
    public static final int ACCEL_FREQ = 10;
    public Map<Long, AccelData> accelData=new HashMap<>();

    @Override
    public void ApplyBody(byte[] buf, int offset, int size) {
        long timestamp = Utils.getUInt32(buf, offset);
        for (int i=4, j=offset+4; i<size; i+=6, j+=6){
            AccelData newAccelData=new AccelData();
            newAccelData.ms=timestamp;
            newAccelData.x=Utils.getInt16(buf, j);
            newAccelData.y=Utils.getInt16(buf, j+2);
            newAccelData.z=Utils.getInt16(buf, j+4);
            accelData.put(timestamp, newAccelData);
            timestamp+=ACCEL_FREQ;
        }
    }

    @Override
    public void DefaultProcess() {
        Data.accelData.putAll(accelData);
    }
}
