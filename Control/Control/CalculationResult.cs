using System.Collections.Generic;

namespace Control
{
    public class CalculationResult
    {
        
        public int clock_prescaler;
        internal int main_clock;
        internal int period_F1;
        internal float duty_F1;
        internal int freq_F1;

        public int presc_F0;
        public int freq_F0;
        public float duty_F0;
        internal int period_F0;


        internal int freq_F2;
        internal int period_F2;
        internal int duty_F2_start;
        internal int duty_F2_stop;
        internal int widthTime;
        internal int widthTimeF1;
    }


}