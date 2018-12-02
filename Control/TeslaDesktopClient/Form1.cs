using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using TeslaDesktopClient.TeslaCommunication;

namespace TeslaDesktopClient
{
    public partial class Form1 : Form
    {

        public Form1()
        {
            InitializeComponent();
            client=new TeslaCommunication.CommunicationProtocolClient();
            client.Open();
            updateView();
            timerAlive= new Timer();
            timerAlive.Interval = 100;
            timerAlive.Tick += TimerAlive_Tick;
            timerAlive.Start();
        }


        protected override void OnClosed(EventArgs e)
        {
            timerAlive.Stop();
            base.OnClosed(e);
            client.Close();            
        }
        Timer timerAlive;
        HardwareState currentState;
        CommunicationProtocolClient client;

        private void button1_Click(object sender, EventArgs e)
        {
            

            if (client.IsConnected())
            {
                client.Disconnect();
            }
            else
            {
                client.Connect(textBoxCom.Text);
            }
            updateView();
        }


        private void TimerAlive_Tick(object sender, EventArgs e)
        {
            currentState = client.getHardwareState();
            this.Invoke(() => {
                if (currentState != null)
                {
                    if (currentState.ledLight)
                    {
                        labelLed.BackColor = Color.Blue;
                    }
                    else
                    {
                        labelLed.BackColor = Color.Transparent;
                    }

                    checkBox1.SetChecked(currentState.enabledF1);

                }
            });
        }
        void updateView()
        {
            if (client.IsConnected())
            {
                button1.Text = "Отключиться";
            }
            else
            {
                button1.Text = "Подключиться";
            }
        }

        byte getByte(bool b)
        {
            if (b)
                return 1;
            return 0;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            client.setEnabled(1, checkBox1.Checked);
        }

    }
}
