using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

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
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            client.Close();
        }

        TeslaCommunication.CommunicationProtocolClient client;

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
    }
}
