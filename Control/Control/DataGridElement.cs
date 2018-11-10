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


        public DataGridElement(string name)
        {
            this.name = name;
        }


        string name;
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        string value;
        public string Value
        {
            get { return value; }
            set
            {
                this.value = value;
                propChange("Value");
            }
        }

        static ObservableCollection<DataGridElement> list;
        static DataGridElement mainClock = new DataGridElement("Main clock");
        static DataGridElement period = new DataGridElement("Period F1");

        internal static ObservableCollection<DataGridElement> getModel()
        {
            if (list == null)
            {
                list = new ObservableCollection<DataGridElement>();
                list.Add(mainClock);
                list.Add(period);
            }


            return list;
        }


        public static void applyResult(CalculationResult result)
        {
            mainClock.Value = result.main_clock.ToString();

        }
    }
}
