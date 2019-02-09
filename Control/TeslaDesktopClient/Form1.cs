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
            
        }



        protected override void OnClosed(EventArgs e)
        {
            
            base.OnClosed(e);
            TeslaDesktopClient.WindowState.Save();

            if (timerAlive != null)
            {
                timerAlive.Stop();
            }
            if (client != null)
            {
                try
                {
                    client.Close();
                }
                catch (System.ServiceModel.EndpointNotFoundException)
                {

                }
                catch (Exception ex)
                {

                    MessageBox.Show(ex.ToString());
                }
                
            }        
        }


        Timer timerAlive;
        HardwareState currentState;
        CommunicationProtocolClient client;

        private void button2_Click(object sender, EventArgs e)
        {
            client = new TeslaCommunication.CommunicationProtocolClient();
            client.Open();
            updateView();
            timerAlive = new Timer();
            timerAlive.Interval = 100;
            timerAlive.Tick += TimerAlive_Tick;
            timerAlive.Start();

            button2.Enabled = false;

            freqChanger1.setEnvoirnment("#1 Несущая", 1, client);
            freqChanger10.setEnvoirnment("#10 Питающий трансформатор", 10, client);
        }
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

        object checkBoxLock = new object();

        int lastSentPeriod = 0;
        int lastSentDuty = 0;

        private void TimerAlive_Tick(object sender, EventArgs e)
        {
            if (client.State == System.ServiceModel.CommunicationState.Faulted)
            {
                this.Close();
                return;
            }
            if (client.State == System.ServiceModel.CommunicationState.Opened)
            {
                try
                {
                    currentState = client.getHardwareState();
                    /*
                    if (pwmGenerating)
                    {
                        int period = int.Parse(textBoxPWMGenerate.Text);
                        int duty = int.Parse(textBoxDuty.Text);
                        if (lastSentPeriod!=period ||
                            lastSentDuty != duty)
                        {
                            sendNewPwmPeriodAndDuty();
                            lastSentPeriod = period;
                            lastSentDuty = duty;
                        }
                    }
                    */
                }
                catch (Exception)
                {
                    this.Close();
                    return;
                }
                
                
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

                        lock (checkBoxLock)
                        {
                            checkBox1.SetChecked(currentState.enabledF1);
                            checkBox2.SetChecked(currentState.enabledF2);
                            checkBox3.SetChecked(currentState.enabledF3);
                            checkBox4.SetChecked(currentState.enabledF4);
                            checkBox5.SetChecked(currentState.enabledF5);
                            checkBox6.SetChecked(currentState.enabledF6);
                            checkBox10.SetChecked(currentState.enabledF10);
                        }
                        if (currentState.currentState == HardwareState.SearchState.Searching)
                        {
                            //textBoxCurrent.Text = currentState.currentPeriod.ToString();
                        }

                        freqChanger1.OnIncomingState(currentState);
                        freqChanger10.OnIncomingState(currentState);
                    }
                });
            }

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
            lock (checkBoxLock)
            {
                client.setEnabled(1, checkBox1.Checked);
            }
        }


        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            lock (checkBoxLock)
            {
                client.setEnabled(2, checkBox2.Checked);
            }
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            lock (checkBoxLock)
            {
                client.setEnabled(3, checkBox3.Checked);
            }
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            lock (checkBoxLock)
            {
                client.setEnabled(4, checkBox4.Checked);
            }
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            lock (checkBoxLock)
            {
                client.setEnabled(5, checkBox5.Checked);
            }
        }

        private void checkBox6_CheckedChanged(object sender, EventArgs e)
        {
            lock (checkBoxLock)
            {
                client.setEnabled(6, checkBox6.Checked);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
