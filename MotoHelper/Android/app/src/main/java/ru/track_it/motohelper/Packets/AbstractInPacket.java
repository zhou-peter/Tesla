package ru.track_it.motohelper.Packets;

public abstract class AbstractInPacket {

    public int Command;

    public int BodySize;

    public abstract void ApplyBody(Byte[] buf, int offset, int size);

    public abstract void Process();
}
