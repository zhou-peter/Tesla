package ru.track_it.motohelper.Packets;

public class PacketOut_01 extends AbstractOutPacket {

    public PacketOut_01(){
        Command=1;
    }

    @Override
    public Iterable<Byte> GetBody() {
        return null;
    }
}
