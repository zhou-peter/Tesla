using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace TeslaDesktopClient
{

    [Serializable]
    public class WindowState : INotifyPropertyChanged
    {

        public event PropertyChangedEventHandler PropertyChanged;
        private void propChange(string prop)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));
        }



        double delta = 48;
        [XmlElement(IsNullable = false)]
        public double Delta
        {
            get { return delta; }
            set
            {
                if (value > 0 && value < 99)
                {
                    delta = value;
                    propChange("Delta");
                }
            }
        }

        [XmlElement(IsNullable = false)]
        public int FromF1 { get; set; } = 100;
        [XmlElement(IsNullable = false)]
        public int ToF1 { get; set; } = 50;
        [XmlElement(IsNullable = false)]
        public int CurrentF1 { get; set; } = 70;
        [XmlElement(IsNullable = false)]
        public int DutyF1 { get; set; } = 35;

        [XmlElement(IsNullable = false)]
        public int FromF10 { get; set; } = 1000;
        [XmlElement(IsNullable = false)]
        public int ToF10 { get; set; } = 500;
        [XmlElement(IsNullable = false)]
        public int CurrentF10 { get; set; } = 700;
        [XmlElement(IsNullable = false)]
        public int DutyF10 { get; set; } = 350;


        static WindowState state = null;
        public static WindowState Load()
        {
            if (state != null)
            {
                return state;
            }

            FileStream fs = null;
            String fileName = "window-state.xml";
            if (File.Exists(fileName))
            {
                try
                {
                    XmlSerializer ser = new XmlSerializer(typeof(WindowState));
                    fs = new FileStream(fileName, FileMode.Open);
                    state = (WindowState)ser.Deserialize(fs);
                }
                catch (Exception)
                {
                }
                finally
                {
                    if (fs != null) fs.Close();
                }
            }
            if (state != null)
                return state;


            state = new WindowState();
            return state;
        }

        public static void Save()
        {
            string fileName = "window-state.xml";
            FileStream fs = null;
            try
            {
                
                XmlSerializer ser = new XmlSerializer(typeof(WindowState));
                fs = new FileStream(fileName, FileMode.Create);
                ser.Serialize(fs, state);


            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                if (fs != null) fs.Close();
            }
        }

    }
}
