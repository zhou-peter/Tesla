using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;
using Microsoft.Office.Tools.Ribbon;
using Microsoft.Office.Interop.Excel;
using Excel2013Control.TeslaCommunication;

namespace Excel2013Control
{
    public partial class ToolBox
    {
        private void ToolBox_Load(object sender, RibbonUIEventArgs e)
        {
            GetClient();
            updateView();
        }
        CommunicationProtocolClient client;
        public CommunicationProtocolClient GetClient()
        {
            if (client == null)
            {
                client = new CommunicationProtocolClient();
            }
            return client;
        }

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
                client.Open();
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
                string portName = Activesheet.Range["A1"].Value2;
                client.Connect(portName);
            }
            catch (Exception ex)
            {
                showMessage(ex.ToString());
            }

            updateView();
        }

        private void buttonConfig_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox1_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox2_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox3_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox4_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox5_Click(object sender, RibbonControlEventArgs e)
        {

        }

        private void checkBox6_Click(object sender, RibbonControlEventArgs e)
        {

        }
    }
}
