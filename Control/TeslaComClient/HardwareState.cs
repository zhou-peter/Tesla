using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;



namespace TeslaComClient
{

    [Guid("0D53A3E8-E51A-49C7-0011-E72A2064F900")]
    [Serializable(), ClassInterface(ClassInterfaceType.AutoDual), ComVisible(true)]
    public class HardwareState
    {
        public bool IsFake = true;
        public bool enabledF1Field;
        public bool enabledF2Field;
        public bool enabledF3Field;
        public bool enabledF4Field;
        public bool ledLightField;


        public HardwareState() { }

        /*
        internal HardwareState(TeslaCommunication.HardwareState hwState) {
            

            if (hwState != null)
            {
                foreach (var prop in GetType().GetProperties())
                {
                    this.GetType().GetProperty(prop.Name).SetValue(this, prop.GetValue(hwState, null), null);
                }
                IsFake = false;
            }
        }
*/
    }
}
