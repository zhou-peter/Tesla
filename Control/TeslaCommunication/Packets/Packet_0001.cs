using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Текущее состояние
    /// </summary>
    public class Packet_0001 : AbstractPacket
    {

        StateStruct state = new StateStruct();





        public override IEnumerable<byte> GetBody()
        {
            return StructureToByteArray(state);
        }
    }
}
