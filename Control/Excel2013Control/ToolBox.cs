using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;
using Microsoft.Office.Tools.Ribbon;
using Microsoft.Office.Interop.Excel;
using Excel2013Control.TeslaCommunication;
using System.Windows.Forms;
using System.Drawing;

namespace Excel2013Control
{
    public partial class ToolBox
    {
        private void ToolBox_Load(object sender, RibbonUIEventArgs e)
        {
            client = new CommunicationProtocolClient();
            this.Close += ToolBox_Close;
            updateView();
        }

        private void ToolBox_Close(object sender, EventArgs e)
        {
            if (timerAlive != null)
            {
                timerAlive.Stop();
            }
            if (client != null)
            {
                client.Close();
            }
        }

        Timer timerAlive;
        HardwareState currentState;
        CommunicationProtocolClient client;


        Worksheet Activesheet
        {
            get
            {
                return Globals.ThisAddIn.GetActiveWorksheet();
            }
        }
        void setCheckboxesState(bool enabled)
        {
            checkBox1.Enabled = enabled;
            checkBox2.Enabled = enabled;
            checkBox3.Enabled = enabled;
            checkBox4.Enabled = enabled;
            checkBox5.Enabled = enabled;
            checkBox6.Enabled = enabled;
        }

        void setCheckboxState(RibbonCheckBox checkBox, bool check)
        {
            if (checkBox.Checked != check)
            {
                checkBox.Enabled = false;
                checkBox.Checked = check;
                checkBox.Enabled = true;
            }
        }


        void updateView()
        {
            if (isServiceConnected())
            {
                buttonService.Enabled = false;
                if (isPortConnected())
                {
                    buttonPort.Enabled = false;
                    buttonConfig.Enabled = true;
                    setCheckboxesState(true);
                }
                else
                {
                    buttonPort.Enabled = true;
                    buttonConfig.Enabled = false;
                    setCheckboxesState(false);
                }
            }
            else
            {
                buttonService.Enabled = true;
                buttonPort.Enabled = false;
                buttonConfig.Enabled = false;
                setCheckboxesState(false);
            }
        }
        void showMessage(string text)
        {
            System.Windows.Forms.MessageBox.Show(text);
        }

        bool isServiceConnected()
        {
            return client.State == System.ServiceModel.CommunicationState.Opened;
        }
        bool isPortConnected()
        {
            if (isServiceConnected())
            {
                return client.IsConnected();
            }
            return false;
        }

        private void buttonService_Click(object sender, RibbonControlEventArgs e)
        {
            try
            {
                if (client.State == System.ServiceModel.CommunicationState.Faulted)
                {
                    client = new CommunicationProtocolClient();
                }
                client.Open();
                timerAlive = new Timer();
                timerAlive.Interval = 500;
                timerAlive.Tick += TimerAlive_Tick;
                timerAlive.Start();
            }
            catch (Exception ex)
            {
                showMessage(ex.ToString());
            }

            updateView();
        }

        private void buttonPort_Click(object sender, RibbonControlEventArgs e)
        {
            try
            {
                string portName = Activesheet.Range["A2"].Value2;
                client.Connect(portName);
            }
            catch (Exception ex)
            {
                showMessage(ex.ToString());
            }

            updateView();
        }

        private void TimerAlive_Tick(object sender, EventArgs e)
        {
            currentState = client.getHardwareState();
            if (currentState != null)
            {
                if (currentState.ledLight)
                {
                    labelBlink.Visible = true;
                }
                else
                {
                    labelBlink.Visible = false;
                }
                setCheckboxState(checkBox1, currentState.enabledF1);
                setCheckboxState(checkBox2, currentState.enabledF2);
                setCheckboxState(checkBox3, currentState.enabledF3);
                setCheckboxState(checkBox4, currentState.enabledF4);
                setCheckboxState(checkBox5, currentState.enabledF5);
                setCheckboxState(checkBox6, currentState.enabledF6);
            }
        }


        private void buttonConfig_Click(object sender, RibbonControlEventArgs e)
        {
            TimersConfiguration timersConfig = new TimersConfiguration();

            Range range = Activesheet.Range["T6:U17"];


            try
            {
                int rowStart = range.Row;
                int colStart = range.Column;
                for (int rowIndex = 0; rowIndex < 12; rowIndex++)
                {
                    Range valueRange = Activesheet.Cells[rowStart + rowIndex, colStart];
                    int value = (int)valueRange.Value;

                    Range keyRange = Activesheet.Cells[rowStart + rowIndex, colStart + 1];
                    String key = keyRange.Value.ToString();

                    switch (key)
                    {
                        case "AZQ9":
                            timersConfig.periodCarrier = value;
                            break;
                        case "AZQ8":
                            timersConfig.periodGap = value;
                            break;
                        case "AZQ22":
                            timersConfig.onGap = value;
                            break;
                        case "AZQ23":
                            timersConfig.offGap = value;
                            break;
                        case "AZQ13":
                            timersConfig.periodBunch = value;
                            break;
                        case "AZQ16":
                            timersConfig.dutyBunch = value;
                            break;
                        case "AZQ27":
                            timersConfig.startGap = value;
                            break;
                        case "AZQ28":
                            timersConfig.stopGap = value;
                            break;
                        case "AZQ35":
                            timersConfig.startHigh = value;
                            break;
                        case "AZQ36":
                            timersConfig.stopHigh = value;
                            break;
                        case "AZQ43":
                            timersConfig.startLow = value;
                            break;
                        case "AZQ44":
                            timersConfig.stopLow = value;
                            break;
                    }

                }
                client.setTimersConfiguration(timersConfig);
            }
            catch (Exception ex)
            {
                showMessage(ex.ToString());
            }
        }

        private void checkBox1_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(1, checkBox1.Checked);
        }

        private void checkBox2_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(2, checkBox2.Checked);
        }

        private void checkBox3_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(3, checkBox3.Checked);
        }

        private void checkBox4_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(4, checkBox4.Checked);
        }

        private void checkBox5_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(5, checkBox5.Checked);
        }

        private void checkBox6_Click(object sender, RibbonControlEventArgs e)
        {
            client.setEnabled(6, checkBox6.Checked);
        }
    }
}
