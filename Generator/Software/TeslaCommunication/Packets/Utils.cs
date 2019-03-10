using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace TeslaCommunication.Packets
{

    public static class Utils
    {
        public static byte PACKET_START = 0xCC;
        public static T ByteArrayToStructure<T>(byte[] bytearray, int offset)
        {
            Type t = typeof(T);
            object obj = Activator.CreateInstance<T>();

            int len = Marshal.SizeOf(obj);

            IntPtr i = Marshal.AllocHGlobal(len);

            Marshal.Copy(bytearray, offset, i, len);

            obj = Marshal.PtrToStructure(i, t);

            Marshal.FreeHGlobal(i);

            return (T)obj;
        }


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
                foreach (byte b in bodyData)
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



        public static T Create<T>(object from)
        {
            Type t = typeof(T);
            object to = Activator.CreateInstance<T>();
            foreach (var toPropInfo in to.GetType().GetProperties())
            {
                PropertyInfo fromPropInfo = from.GetType().GetProperty(toPropInfo.Name);
                if (fromPropInfo != null)
                {
                    toPropInfo.SetValue(to, fromPropInfo.GetValue(from, null), null);
                }
            }

            foreach (var toFieldInfo in to.GetType().GetFields())
            {
                FieldInfo fromPropInfo = from.GetType().GetField(toFieldInfo.Name);
                if (fromPropInfo != null)
                {
                    object value = fromPropInfo.GetValue(from);
                    if (toFieldInfo.FieldType == typeof(UInt16))
                    {
                        toFieldInfo.SetValue(to, Convert.ToUInt16(value));
                    }
                    else
                    {
                        toFieldInfo.SetValue(to, value);
                    }                    
                }
            }

            return (T)to;
        }
    }
}
