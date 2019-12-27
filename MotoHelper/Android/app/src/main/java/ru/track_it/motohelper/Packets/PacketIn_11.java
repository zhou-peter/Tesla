package ru.track_it.motohelper.Packets;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import ru.track_it.motohelper.AccelData;
import ru.track_it.motohelper.Data;

import static ru.track_it.motohelper.Data.accelArray;
import static ru.track_it.motohelper.Data.accelData;
import static ru.track_it.motohelper.Data.accelDataPointsLimit;
import static ru.track_it.motohelper.Data.accelLock;

public class PacketIn_11 extends AbstractInPacket {

    private Map<Long, AccelData> tmpData = new HashMap<>();

    @Override
    public void ApplyBody(byte[] buf, int offset, int size) {
        long timestamp = Utils.getUInt32(buf, offset);

        for (int i = 4, j = offset + 4; i < size; i += 6, j += 6) {
            AccelData newAccelData = new AccelData();
            newAccelData.ms = timestamp;
            newAccelData.x = Utils.getInt16(buf, j);
            newAccelData.y = Utils.getInt16(buf, j + 2);
            newAccelData.z = Utils.getInt16(buf, j + 4);
            tmpData.put(newAccelData.ms, newAccelData);
            timestamp -= Data.samplePeriod;
        }
    }

    @Override
    public void DefaultProcess() {
        //add/replace new data
        accelData.putAll(tmpData);

        //create temprory sorted list
        List<AccelData> tmpList = new ArrayList<>(accelData.size());
        tmpList.addAll(accelData.values());
        Collections.sort(tmpList, Data.accelDataSorter);

        //trim
        if (tmpList.size() > accelDataPointsLimit) {
            int delta = tmpList.size() - accelDataPointsLimit;
            while (--delta > 0) {
                AccelData oldData = tmpList.get(0);
                tmpList.remove(oldData);
                accelData.remove(oldData.ms);
            }
        }

        //replace datasource for a View
        synchronized (accelLock) {
            accelArray.clear();
            accelArray.addAll(tmpList);
        }
    }
}
