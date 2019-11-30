package ru.track_it.motohelper.Packets;

import ru.track_it.motohelper.AccelData;

public class PacketIn_11 extends AbstractInPacket {


    @Override
    public void ApplyBody(byte[] buf, int offset, int size) {
        AccelData newAccelData=new AccelData();

    }


}
