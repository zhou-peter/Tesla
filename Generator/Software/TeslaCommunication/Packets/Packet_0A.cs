using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Установить частоту для ШИМа
    /// </summary>
    public class Packet_0A : AbstractOutPacket
    {

        struct body
        {
            public ushort period;
            public ushort duty;
            public byte feature_number;
        }


        private body b;
        public Packet_0A(int period, int duty, byte num)
        {
            Command = 0x0A;
            b = new body { period = (UInt16)period, duty = (UInt16)duty, feature_number=num};            
            BodySize = getSize(b);
        }

        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
