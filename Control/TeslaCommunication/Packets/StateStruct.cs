using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    public struct StateStruct
    {
        //включены - не включены шимы
        public byte enabled_f1; //byte == boolean
        public byte enabled_f2;
        public byte enabled_f3;
        public byte enabled_f4;

        public byte led_light;
        public byte tmp1;
        public byte tmp2;
        public byte tmp3;
    }
}
