package ru.track_it.motohelper.Packets;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class PacketsManager {

    public static final byte PACKET_START = 0x7C;
    private InputStream inputStream;
    private OutputStream outputStream;

    public ConcurrentLinkedQueue<AbstractInPacket> receivedPackets = new ConcurrentLinkedQueue<>();
    public ConcurrentLinkedQueue<AbstractOutPacket> packetsToSend = new ConcurrentLinkedQueue<>();

    public PacketsManager(InputStream inputStream, OutputStream outputStream) {
        this.inputStream=inputStream;
        this.outputStream=outputStream;
    }
}
