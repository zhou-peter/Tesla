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
    public class Packet_02 : AbstractOutPacket
    {
        struct body
        {
            public byte num;
            public byte enabled;
        }

        body b;

        public Packet_02(byte num, bool enabled)
        {
            b = new body();
            b.num = num;
            b.enabled = getByte(enabled);
        }


        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
