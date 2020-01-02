package ru.track_it.motohelper.Packets;

import java.nio.ByteBuffer;

public abstract class AbstractOutPacket {

    public int Command;

    public int BodySize;

    public abstract Iterable<Byte> GetBody();

    public byte[] ToArray()
    {
        if (BodySize > 0)
        {
            return Utils.PacketToArray(Command, BodySize, GetBody());
        }
        return Utils.PacketToArray(Command, 0, null);
    }

}
