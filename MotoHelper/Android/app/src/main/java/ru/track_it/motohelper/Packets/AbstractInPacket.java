package ru.track_it.motohelper.Packets;

public abstract class AbstractInPacket {

    public int Command;

    public int BodySize;

    //called in packet former thread (should be fast)
    public abstract void ApplyBody(byte[] buf, int offset, int size);

    //действие по умолчанию для пакета (вызывается в другом потоке и
    //может быть длительным)
    public void DefaultProcess()
    {

    }

}
