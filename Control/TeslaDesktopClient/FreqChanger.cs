using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using TeslaDesktopClient.TeslaCommunication;

namespace TeslaDesktopClient
{
    public partial class FreqChanger : UserControl
    {


        CommunicationProtocolClient client;
        bool halfDutyMode = true;
        private int featureNumber;

        public FreqChanger()
        {
            InitializeComponent();

            trackBarDuty.MouseDown += TrackBarDuty_MouseDown;
            trackBarFreq.Scroll += trackBarFreq_Scroll;
            trackBarDuty.Scroll += trackBarDuty_Scroll;

            trackBarFreq.MouseDown += TrackBarFreq_MouseDown;
            trackBarFreq.MouseUp += TrackBarFreq_MouseUp;

            textBoxStart.TextChanged += TextBoxStart_TextChanged;
            textBoxStop.TextChanged += TextBoxStop_TextChanged;
            //updateTrackBarFreqRanges();
            //onFreqChange();
        }

        internal void setEnvoirnment(string featureName, int featureNumber, CommunicationProtocolClient client)
        {
            this.client = client;
            this.groupBox2.Text = featureName;
            this.featureNumber = featureNumber;
            WindowState ws = WindowState.Load();
            textBoxStart.DataBindings.Add("Text", ws, "FromF" + featureNumber.ToString(), false, DataSourceUpdateMode.OnPropertyChanged);
            textBoxStop.DataBindings.Add("Text", ws, "ToF" + featureNumber.ToString(), false, DataSourceUpdateMode.OnPropertyChanged);
            textBoxPeriod.DataBindings.Add("Text", ws, "CurrentF" + featureNumber.ToString(), false, DataSourceUpdateMode.OnPropertyChanged);
            textBoxDuty.DataBindings.Add("Text", ws, "DutyF" + featureNumber.ToString(), false, DataSourceUpdateMode.OnPropertyChanged);


            updateTrackBarFreqRanges();
            onFreqChange();
        }

        private void TextBoxStop_TextChanged(object sender, EventArgs e)
        {
            updateTrackBarFreqRanges();
        }

        private void TextBoxStart_TextChanged(object sender, EventArgs e)
        {
            updateTrackBarFreqRanges();
        }

        /*
        bool pwmGenerating = false;

        private void button3_Click(object sender, EventArgs e)
        {
            int start = int.Parse(textBoxStart.Text);
            int stop = int.Parse(textBoxStop.Text);
            int delay = int.Parse(textBoxDelay.Text);
            client.searchStart(start, stop, delay);

        }

        private void button4_Click(object sender, EventArgs e)
        {
            pwmGenerating = false;
            client.searchStop();
        }

            

        private void button5_Click(object sender, EventArgs e)
        {
            pwmGenerating = true;
            sendNewPwmPeriodAndDuty();
        }
*/
        /*
        private void buttonCopy_Click(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(textBoxCurrent.Text))
            {
                textBoxPWMGenerate.Text = textBoxCurrent.Text;
                updateTrackBarFreqRanges();
                trackBarFreq.Value = int.Parse(textBoxPWMGenerate.Text);
                onFreqChange();
            }
        }*/

        DateTime lastSend = DateTime.Now;
        void sendNewPwmPeriodAndDuty()
        {
            int period = int.Parse(textBoxPeriod.Text);
            int duty = int.Parse(textBoxDuty.Text);
            if (client != null && lastSend.AddMilliseconds(100)<=DateTime.Now)
            {
                lastSend = DateTime.Now;
                client.searchGeneratePWM(period, duty, (byte)featureNumber);
            }            
        }


        void updateTrackBarFreqRanges()
        {
            int start = int.Parse(textBoxStart.Text);
            int stop = int.Parse(textBoxStop.Text);
            if (stop < start)
            {
                int tmp = stop;
                stop = start;
                start = tmp;
            }


            trackBarFreq.Minimum = start;
            trackBarFreq.Maximum = stop;

            trackBarFreq.Value = String.IsNullOrEmpty(textBoxCurrent.Text) ? 
                start+((stop-start)/2) : int.Parse(textBoxCurrent.Text);

        }


        
        void onFreqChange()
        {
            if (trackBarFreq.Value > 0)
            {
                labelfreqValue.Text = (Common.CPU_FREQ / trackBarFreq.Value).ToString();
                textBoxPeriod.Text = trackBarFreq.Value.ToString();
                trackBarDuty.Maximum = trackBarFreq.Value;
                if (halfDutyMode)
                {
                    trackBarDuty.Value = trackBarDuty.Maximum / 2;
                }
                textBoxDuty.Text = trackBarDuty.Value.ToString();
            }
            sendNewPwmPeriodAndDuty();
        }


        private void trackBarFreq_Scroll(object sender, EventArgs e)
        {
            onFreqChange();
        }


        private void trackBarDuty_Scroll(object sender, EventArgs e)
        {
            halfDutyMode = false;
            textBoxDuty.Text = trackBarDuty.Value.ToString();
        }

        private void TrackBarDuty_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                int half = trackBarDuty.Maximum / 2;
                halfDutyMode = true;
                trackBarDuty.Value = half;
            }
        }



        bool freqTracking = false;
        private void TrackBarFreq_MouseUp(object sender, MouseEventArgs e)
        {
            freqTracking = false;
        }

        private void TrackBarFreq_MouseDown(object sender, MouseEventArgs e)
        {
            freqTracking = true;
        }


        internal void OnIncomingState(HardwareState currentState)
        {
            int theirPeriod = 0;
            if (this.featureNumber == 1)
            {
                theirPeriod = currentState.periodF1;
            }
            else if (this.featureNumber == 10)
            {
                theirPeriod = currentState.periodF10;
            }

            textBoxCurrent.Text = theirPeriod.ToString();

            if (textBoxCurrent.Text.Equals(textBoxPeriod.Text) 
                && !freqTracking){
                trackBarFreq.Value = int.Parse(textBoxPeriod.Text);
                onFreqChange();
            }
        }
    }
}
