using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    public abstract class AbstractPacket
    {

        public int Command { get; set; }

        public int BodySize { get; set; }

        public abstract IEnumerable<byte> GetBody();

        public static byte PACKET_START = 0xB1;



        public virtual byte[] ToArray()
        {
            if (BodySize > 0)
            {
                return PacketToArray(Command, BodySize, GetBody());
            }
            return PacketToArray(Command, 0, null);
        }

        //public abstract void InitFromArray(byte[] array);

        public static byte[] PacketToArray(int command, int bodySize,
                            IEnumerable<byte> bodyData)
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
                int i = 0;
                foreach(byte b in bodyData)
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
    }
}
