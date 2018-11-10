using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Control
{
    public class DataGridElement : INotifyPropertyChanged
    {

        public event PropertyChangedEventHandler PropertyChanged;
        private void propChange(string prop)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));
        }


        public DataGridElement(string name, string dim)
        {
            Name = name;
            Dimension = dim;
        }


        public string Name { get; set; }
        string _value;
        public string Value
        {
            get { return _value; }
            set
            {
                this._value = value;
                propChange("Value");
            }
        }

        public string Dimension { get; set; }

        static ObservableCollection<DataGridElement> list;
        static DataGridElement systemClockPrescaler = new DataGridElement("System prescaler", "");
        static DataGridElement mainClock = new DataGridElement("Main clock", "Гц");
        static DataGridElement freqF1 = new DataGridElement("Freq F1", "Гц");
        static DataGridElement periodF1 = new DataGridElement("Period F1", "");
        static DataGridElement dutyF1 = new DataGridElement("Duty F1", "%");
        static DataGridElement width_half_F1 = new DataGridElement("ширина полпериода", "нс");

        static DataGridElement freqF0 = new DataGridElement("Freq F0", "Гц");
        static DataGridElement periodF0 = new DataGridElement("Period F0", "");
        static DataGridElement dutyF0 = new DataGridElement("Duty F0", "%");


        static DataGridElement freqF2 = new DataGridElement("Freq F2", "Гц");
        static DataGridElement periodF2 = new DataGridElement("Period F2", "");
        static DataGridElement dutyF2_start = new DataGridElement("F2 Start", "");
        static DataGridElement dutyF2_stop = new DataGridElement("F2 Stop", "");
        static DataGridElement width_time = new DataGridElement("ширина пролома", "нс");


        internal static ObservableCollection<DataGridElement> getModel()
        {
            if (list == null)
            {
                list = new ObservableCollection<DataGridElement>();
                list.Add(systemClockPrescaler);
                list.Add(mainClock);

                list.Add(freqF1);
                list.Add(periodF1);
                list.Add(dutyF1);

                list.Add(freqF0);
                list.Add(periodF0);
                list.Add(dutyF0);

                list.Add(freqF2);
                list.Add(periodF2);
                list.Add(dutyF2_start);
                list.Add(dutyF2_stop);
                list.Add(width_time);
                list.Add(width_half_F1);
            }


            return list;
        }


        public static void applyResult(CalculationResult result)
        {
            systemClockPrescaler.Value = result.clock_prescaler.ToString();
            mainClock.Value = result.main_clock.ToString();

            freqF1.Value = result.freq_F1.ToString();
            periodF1.Value = result.period_F1.ToString();
            dutyF1.Value = result.duty_F1.ToString();

            freqF0.Value = result.freq_F0.ToString();
            periodF0.Value = result.period_F0.ToString();
            dutyF0.Value = result.duty_F0.ToString();

            freqF2.Value = result.freq_F2.ToString();
            periodF2.Value = result.period_F2.ToString();
            dutyF2_start.Value = result.duty_F2_start.ToString();
            dutyF2_stop.Value = result.duty_F2_stop.ToString();
            width_time.Value = result.widthTime.ToString();
            width_half_F1.Value = result.widthTimeF1.ToString();

        }
    }
}
