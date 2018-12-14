using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Сконфигурировать период, дьюти
    /// </summary>
    public class Packet_04 : AbstractOutPacket
    {

        struct body
        {
            public ushort periodCarrier;
            public ushort periodGap;
            public ushort onGap;
            public ushort offGap;
            public ushort periodBunch;
            public ushort dutyBunch;
            public ushort startGap;
            public ushort stopGap;
            public ushort startHigh;
            public ushort stopHigh;
            public ushort startLow;
            public ushort stopLow;
        }


        private body b;
        public Packet_04(TimersConfiguration timersConfiguration)
        {
            Command = 4;            
            b = Utils.Create<body>(timersConfiguration);
            BodySize = getSize(b);
        }

        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
