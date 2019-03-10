using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace TeslaCommunication.Packets
{
    public abstract class AbstractOutPacket
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

        protected int getSize(object obj)
        {
            return Marshal.SizeOf(obj);
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





        public virtual byte[] ToArray()
        {
            if (BodySize > 0)
            {
                return Utils.PacketToArray(Command, BodySize, GetBody());
            }
            return Utils.PacketToArray(Command, 0, null);
        }



    }
}
