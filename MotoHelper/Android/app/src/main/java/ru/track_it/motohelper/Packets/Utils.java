package ru.track_it.motohelper.Packets;

import java.nio.ByteBuffer;

import static ru.track_it.motohelper.Packets.PacketsManager.PACKET_START;

public class Utils {


    public static final byte[] PacketToArray(int command, int bodySize,
                                       Iterable<Byte> bodyData)
    {
        int i = 0;
        int txBufSize = 6 + bodySize;
        byte[] txBuf = new byte[txBufSize];
        txBuf[0] = PACKET_START;
        txBuf[1] = (byte)(txBufSize);
        txBuf[2] = (byte)(txBufSize >> 8);
        txBuf[3] = (byte)command;

        if (bodySize > 0)
        {
            for(byte b : bodyData)
            {
                txBuf[4 + i] = b;
                i++;
            }
        }

        byte crc = 0;
        for (i = 0; i < 4 + bodySize; i++)
        {
            crc += txBuf[i];
        }
        txBuf[4 + bodySize] = crc;
        txBuf[5 + bodySize] = (byte)(crc ^ (byte)0xAA);

        return txBuf;
    }


    public static final short getInt16(byte[] buf, int offset){
        ByteBuffer byteBuffer = ByteBuffer.wrap(buf, offset, 2);
        return byteBuffer.getShort();
    }


    public static final int getInt32(byte[] buf, int offset){
        ByteBuffer byteBuffer = ByteBuffer.wrap(buf, offset, 4);
        return byteBuffer.getInt();
    }

    public static final long getUInt32(byte[] buf, int offset){
        ByteBuffer byteBuffer = ByteBuffer.wrap(buf, offset, 4);
        return ((long)byteBuffer.getInt())&0xFFFFFFFF;
    }
}
