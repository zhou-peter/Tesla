using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Остановить поиск
    /// </summary>
    public class Packet_08 : AbstractOutPacket
    {

        public Packet_08()
        {
            Command = 8;   
            BodySize = 0;
        }

        public override IEnumerable<byte> GetBody()
        {
            return null;
        }


    }
}
