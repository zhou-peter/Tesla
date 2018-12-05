﻿using System;
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
            public byte num;
            public byte prescaler;
            public ushort period;
            public ushort duty;
        }

        body b;

        public Packet_04(byte num, byte prescaler, int period, int duty)
        {
            Command = 4;
            BodySize = 6;
            b = new body();
            b.num = num;
            b.prescaler = prescaler;
            b.period = (ushort)period;
            b.duty = (ushort)duty;
        }


        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(b);
        }


    }
}
