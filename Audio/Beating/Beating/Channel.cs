using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using NAudio.Wave;

namespace Beating
{
    public partial class Channel : UserControl
    {
        public Channel()
        {
            InitializeComponent();
        }

        private WaveOut waveOut;
        private WaveOut waveOut2;

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
            {
                startGeneration();
            }
            else
            {
                stopGeneration();
            }
            
        }

        private void startGeneration()
        {
            if (waveOut == null)
            {
                var sineWaveProvider = new SineWaveProvider32();
                sineWaveProvider.SetWaveFormat(16000, 2); // 16kHz mono
                sineWaveProvider.Frequency = 2000;
                sineWaveProvider.Amplitude = 0.25f;

                var mono = sineWaveProvider.ToMono(0.1F, 0.9F);

                var sineWaveProvider2 = new SineWaveProvider32();
                sineWaveProvider2.SetWaveFormat(16000, 2); 
                sineWaveProvider2.Frequency = 1000;
                sineWaveProvider2.Amplitude = 0.25f;

                var mono2 = sineWaveProvider2.ToMono(0.9F, 0.1F);

                waveOut = new WaveOut();
                waveOut.Init(mono);

                waveOut2 = new WaveOut();
                waveOut2.Init(mono2);

                waveOut.Play();
                waveOut2.Play();
            }
        }

        void stopGeneration()
        {
            if (waveOut != null)
            {
                waveOut.Stop();
                waveOut.Dispose();
                waveOut = null;
            }
        }
    }
}
