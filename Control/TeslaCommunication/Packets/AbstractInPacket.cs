using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace TeslaCommunication.Packets
{
    public abstract class AbstractInPacket
    {

        public int Command { get; set; }

        public int BodySize { get; set; }
        

        public abstract void ApplyBody(Byte[] buf, int offset, int size);
        public abstract void Process();
    }
}
