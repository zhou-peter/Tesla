using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Включить отключить таймеры
    /// Основной, проломы, пропуски прерывания
    /// </summary>
    public class Packet_23 : AbstractOutPacket
    {
        struct body
        {
            public byte enabled_f1;
            public byte enabled_f2;
            public byte enabled_f3;
            public byte enabled_f4;
        }

        body b;

        public Packet_23(bool f1, bool f2, bool f3, bool f4)
        {
            b = new body();
            b.enabled_f1 = getByte(f1);
            b.enabled_f2 = getByte(f2);
            b.enabled_f3 = getByte(f3);
            b.enabled_f4 = getByte(f4);
        }


        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
