using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace TeslaComClient
{
    [Guid("0D53A3E8-E51A-49C7-0011-122A2064F900")]
    [Serializable(), ClassInterface(ClassInterfaceType.AutoDual), ComVisible(true)]
    public class TimersConfiguration
    {

        public int periodCarrier { get; set; }
        public int periodGap { get; set; }
        public int onGap { get; set; }
        public int offGap { get; set; }
        public int periodBunch { get; set; }
        public int dutyBunch { get; set; }
        public int startGap { get; set; }
        public int stopGap { get; set; }
        public int startHigh { get; set; }
        public int stopHigh { get; set; }
        public int startLow { get; set; }
        public int stopLow { get; set; }
    }
}
