using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Искать частоту
    /// </summary>
    public class Packet_06 : AbstractOutPacket
    {

        struct body
        {
            public ushort start;
            public ushort stop;
            public ushort delay;
        }


        private body b;
        public Packet_06(int start, int stop, int delay)
        {
            Command = 6;
            b = new body { start = (UInt16)start, stop = (UInt16)stop, delay = (UInt16)delay };            
            BodySize = getSize(b);
        }

        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
