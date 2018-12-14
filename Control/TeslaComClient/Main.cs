using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TeslaComClient.TeslaCommunication;

namespace TeslaComClient
{

    
    [Guid("0D53A3E8-E51A-49C7-004E-E72A2064F900")]
    [Serializable(), ClassInterface(ClassInterfaceType.AutoDual), ComVisible(true)]
    public class Main : IDisposable
    {

        CommunicationProtocolClient client;


        public Main(){
            string assemblyLoc = GetType().Assembly.Location;
            string configName = assemblyLoc + ".config";
            AppDomain.CurrentDomain.SetData("APP_CONFIG_FILE", configName);
            try
            {
                if (client == null)
                {
                    client = new CommunicationProtocolClient();
                    client.Open();
                }
            }
            catch (Exception ex)
            {
            }
        }

        public void Dispose()
        {
            if (client != null)
            {
                client.Close();
            }
        }



        public bool Connect(string comPortName)
        {
            if (client == null)
            {
                client = new CommunicationProtocolClient();
                client.Open();
            }

            return client.Connect(comPortName);
        }
        //close Com port connection
        public void Disconnect()
        {
            if (client == null)
            {
                client = new CommunicationProtocolClient();
                client.Open();
            }
            //close Com port connection
            client.Disconnect();
        }
        
        public HardwareState GetState()
        {
            return new HardwareState(client.getHardwareState());
        }

        public void SetTimerState(int timerNumber, bool enabled)
        {
            client.setEnabled((byte)timerNumber, enabled);
        }

        public void SetTimersConfiguraion(TimersConfiguration timersConfiguration)
        {
            TeslaCommunication.TimersConfiguration tc = new TeslaCommunication.TimersConfiguration();
            Utils.Copy(timersConfiguration, tc);
            client.setTimersConfiguration(tc);
        }
    }
}
