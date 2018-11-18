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
    public class Packet_0023 : AbstractPacket
    {
        public override IEnumerable<byte> GetBody()
        {
            throw new NotImplementedException();
        }
    }
}
