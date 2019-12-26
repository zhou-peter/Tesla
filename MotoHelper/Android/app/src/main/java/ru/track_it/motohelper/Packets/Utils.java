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
        short value = buf[offset];
        value |=buf[offset+1]<<8;
        return value;
    }

/*
    public static final int getInt32(byte[] buf, int offset){
        ByteBuffer byteBuffer = ByteBuffer.wrap(buf, offset, 4);
        return byteBuffer.getInt();
    }*/

    public static final long getUInt32(byte[] buf, int offset){
        int value = buf[offset];
        value |=buf[offset+1]<<8;
        value |=buf[offset+2]<<16;
        value |=buf[offset+3]<<24;
        long value2 = value & 0xFFFFFFFFl;
        return value2;
    }
}
