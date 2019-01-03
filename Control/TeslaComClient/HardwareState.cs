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
        public enum SearchState
        {
            Idle = 0,
            Searching,
            Generating
        }


        public bool IsFake = true;
        public bool enabledF1 { get; set; }
        public bool enabledF2 { get; set; }
        public bool enabledF3 { get; set; }
        public bool enabledF4 { get; set; }
        public bool enabledF5 { get; set; }
        public bool enabledF6 { get; set; }
        public bool ledLight { get; set; }
        public int currentPeriod { get; set; }
        public SearchState currentState { get; set; }

        public bool IsSearchIdle
        {
            get
            {
                return currentState == SearchState.Idle;
            }
            set { }
        }



        public HardwareState() { }

        
        internal HardwareState(TeslaCommunication.HardwareState hwState) {
            

            if (hwState != null)
            {
                Utils.Copy(hwState, this);
                IsFake = false;
            }
        }

    }
}
