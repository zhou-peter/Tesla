using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Текущее состояние
    /// </summary>
    public class Packet_01 : AbstractInPacket
    {

        public StateStruct state;

        public override void ApplyBody(byte[] buf, int offset, int size)
        {
            state = Utils.ByteArrayToStructure<StateStruct>(buf, offset);
        }

        public override void Process()
        {
           
        }
    }
}
