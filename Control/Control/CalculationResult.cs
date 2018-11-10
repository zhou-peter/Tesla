using System.Collections.Generic;

namespace Control
{
    public class CalculationResult
    {
        
        public int clock_prescaler;
        internal int main_clock;
        internal int period_F1;
        internal float duty_F1;

        public List<DataGridElement> generateDataContext()
        {
            List<DataGridElement> list = new List<DataGridElement>();
            list.Add(new DataGridElement { Name = "Main clock", Value = main_clock.ToString() });
            return list;
        }
    }


}