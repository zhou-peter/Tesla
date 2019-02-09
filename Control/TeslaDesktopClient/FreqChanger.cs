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
            textBoxStart.TextChanged += TextBoxStart_TextChanged;
            textBoxStop.TextChanged += TextBoxStop_TextChanged;
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
        void sendNewPwmPeriodAndDuty()
        {
            int period = int.Parse(textBoxPWMGenerate.Text);
            int duty = int.Parse(textBoxDuty.Text);
            client.searchGeneratePWM(period, duty);
        }

        internal void setEnvoirnment(string featureName, int featureNumber, CommunicationProtocolClient client)
        {
            this.client = client;
            this.groupBox2.Text = featureName;
            this.featureNumber = featureNumber;
        }

        void updateTrackBarFreqRanges()
        {
            int start = int.Parse(textBoxStart.Text);
            int stop = int.Parse(textBoxStop.Text);
            if (stop > start)
            {
                trackBarFreq.Minimum = start;
                trackBarFreq.Maximum = stop;
            }
            else
            {
                trackBarFreq.Minimum = stop;
                trackBarFreq.Maximum = start;
            }


        }

        private void buttonCopy_Click(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(textBoxCurrent.Text))
            {
                textBoxPWMGenerate.Text = textBoxCurrent.Text;
                updateTrackBarFreqRanges();
                trackBarFreq.Value = int.Parse(textBoxPWMGenerate.Text);
                onFreqChange();
            }
        }


        
        void onFreqChange()
        {
            if (trackBarFreq.Value > 0)
            {
                labelfreqValue.Text = (Common.CPU_FREQ / trackBarFreq.Value).ToString();
                textBoxPWMGenerate.Text = trackBarFreq.Value.ToString();
                trackBarDuty.Maximum = trackBarFreq.Value;
                if (halfDutyMode)
                {
                    trackBarDuty.Value = trackBarDuty.Maximum / 2;
                }
                textBoxDuty.Text = trackBarDuty.Value.ToString();
            }
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

    }
}
