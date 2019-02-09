using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TeslaCommunication.Packets;

namespace TeslaCommunication
{

    [Serializable]
    public class HardwareState
    {
        
        public enum SearchState
        {
            Idle=0,
            Searching,
            Generating
        }


        public bool enabledF1;
        public bool enabledF2;
        public bool enabledF3;
        public bool enabledF4;
        public bool enabledF5;
        public bool enabledF6;
        public bool enabledF10;
        public bool ledLight;
        public int periodF1;
        public int periodF10;


        public SearchState currentState;

        bool byteToBool(byte b)
        {
            if (b > 0)
                return true;
            return false;
        }

        public HardwareState(StateStruct state)
        {
            this.enabledF1 = byteToBool(state.enabled_f1);
            this.enabledF2 = byteToBool(state.enabled_f2);
            this.enabledF3 = byteToBool(state.enabled_f3);
            this.enabledF4 = byteToBool(state.enabled_f4);
            this.enabledF5 = byteToBool(state.enabled_f5);
            this.enabledF6 = byteToBool(state.enabled_f6);
            this.enabledF10 = byteToBool(state.enabled_f10);
            this.ledLight = byteToBool(state.led_light);

            int tmp = state.search_env & 0x0F;
            switch (tmp)
            {
                case 0:
                    currentState = SearchState.Idle;
                    break;
                case 1:
                    currentState = SearchState.Searching;
                    break;
                case 2:
                    currentState = SearchState.Generating;
                    break;
                default:
                    throw new ArgumentException();
            }

            this.periodF1 = state.period_f1;
            this.periodF10 = state.period_f10;
        }
    }
}
