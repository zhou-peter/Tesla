using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Включить ШИМ на частоте
    /// </summary>
    public class Packet_0A : AbstractOutPacket
    {

        struct body
        {
            public ushort period;
        }


        private body b;
        public Packet_0A(int period)
        {
            Command = 0x0A;
            b = new body { period = (UInt16)period };            
            BodySize = getSize(b);
        }

        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
