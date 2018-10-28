using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Xml.Serialization;

namespace Control
{
    public enum RadioTypes { RadioUse, TextBoxUse}
    [Serializable]
    public class MainWindowState
    {
        [XmlElement(IsNullable = false)]
        public int F0 { get; set; }
        [XmlElement(IsNullable = false)]
        public int F1 { get; set; }
        double delta = 48;
        [XmlElement(IsNullable = false)]
        public double Delta
        {
            get { return delta; }
            set
            {
                if (delta > 0 && delta < 99)
                {
                    delta = value;
                }
                else
                {
                    delta = 48;
                }
            }
        }
        [XmlElement(IsNullable = false)]
        public float DeltaWidth { get; set; }

        List<int> comboPrescalsers = new List<int>() { 16, 32, 64, 128, 256, 512, 1024, 2048 };
        [XmlIgnore]
        public List<int> ComboPrescalers { get { return comboPrescalsers; } }
        int currentComboPrescaler = 16;
        [XmlElement(IsNullable = false)]
        public int CurrentComboPrescaler { get { return currentComboPrescaler; } set { currentComboPrescaler = value; } }
        int textPrescaler = 200;
        [XmlElement(IsNullable = false)]
        public int TextPrescaler { get { return textPrescaler; } set { textPrescaler = value; } }
        RadioTypes radioType = RadioTypes.RadioUse;
        [XmlElement(IsNullable = false)]
        public RadioTypes RadioType { get { return radioType; } set { radioType = value; } }
        [XmlIgnore]
        public bool DropDownUse
        {
            get { return radioType == RadioTypes.RadioUse;  }
            set { radioType = value ? RadioTypes.RadioUse :  RadioTypes.TextBoxUse; }
        }
        [XmlIgnore]
        public bool TextBoxUse
        {
            get { return !DropDownUse; }
            set { DropDownUse = !value; }
        }


        public int GetPrescaler()
        {
            if (radioType == RadioTypes.RadioUse)
            {
                return currentComboPrescaler;
            }
            else
            {
                return textPrescaler;
            }
        }


        public static MainWindowState Load()
        {
            MainWindowState state = null;
            FileStream fs = null;
            String fileName = "window-state.xml";
            if (File.Exists(fileName))
            {
                try
                {
                    XmlSerializer ser = new XmlSerializer(typeof(MainWindowState));
                    fs = new FileStream(fileName, FileMode.Open);
                    state = (MainWindowState)ser.Deserialize(fs);
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


            state = new MainWindowState();
            return state;
        }

        public void Save()
        {
            string fileName = "window-state.xml";
            FileStream fs = null;
            try
            {

                XmlSerializer ser = new XmlSerializer(typeof(MainWindowState));
                fs = new FileStream(fileName, FileMode.Create);
                ser.Serialize(fs, this);


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
