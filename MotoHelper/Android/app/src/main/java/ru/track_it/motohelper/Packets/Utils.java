package ru.track_it.motohelper.Packets;

import java.nio.ByteBuffer;

import static ru.track_it.motohelper.Packets.PacketsManager.PACKET_START;

public class Utils {


    public static final byte[] PacketToArray(int command, int bodySize,
                                             Iterable<Byte> bodyData) {
        int i = 0;
        int txBufSize = 4 + bodySize;
        byte[] txBuf = new byte[txBufSize];
        txBuf[0] = PACKET_START;
        txBuf[1] = (byte) (txBufSize);
        txBuf[2] = (byte) command;

        if (bodySize > 0) {
            for (byte b : bodyData) {
                txBuf[3 + i] = b;
                i++;
            }
        }

        byte crc = 0;
        for (i = 0; i < 3 + bodySize; i++) {
            crc += txBuf[i];
        }
        txBuf[3 + bodySize] = crc;

        return txBuf;
    }


    public static final short getInt16(byte[] buf, int offset) {
        short value = (short) (buf[offset] & 0xFF);
        value |= buf[offset + 1] << 8;
        return value;
    }

/*
    public static final int getInt32(byte[] buf, int offset){
        ByteBuffer byteBuffer = ByteBuffer.wrap(buf, offset, 4);
        return byteBuffer.getInt();
    }*/

    public static final long getUInt32(byte[] buf, int offset) {
        long value = buf[offset] & 0xFF;
        value |= (buf[offset + 1] & 0xFF) << 8;
        value |= (buf[offset + 2] & 0xFF) << 16;
        value |= (buf[offset + 3] & 0xFF) << 24;
        return value;
    }
}
