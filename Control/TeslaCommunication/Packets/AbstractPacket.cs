using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace TeslaCommunication.Packets
{
    public abstract class AbstractPacket
    {

        public int Command { get; set; }

        public int BodySize { get; set; }

        public abstract IEnumerable<byte> GetBody();

        public static byte PACKET_START = 0xCC;

        protected byte getByte(bool b)
        {
            if (b) return 1;
            return 0;
        }

        protected byte[] StructureToByteArray(object obj)
        {
            int len = Marshal.SizeOf(obj);

            byte[] arr = new byte[len];

            IntPtr ptr = Marshal.AllocHGlobal(len);

            Marshal.StructureToPtr(obj, ptr, true);

            Marshal.Copy(ptr, arr, 0, len);

            Marshal.FreeHGlobal(ptr);

            return arr;
        }

        protected void ByteArrayToStructure(byte[] bytearray, ref object obj)
        {
            int len = Marshal.SizeOf(obj);

            IntPtr i = Marshal.AllocHGlobal(len);

            Marshal.Copy(bytearray, 0, i, len);

            obj = Marshal.PtrToStructure(i, obj.GetType());

            Marshal.FreeHGlobal(i);
        }





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
