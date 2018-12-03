using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TeslaComClient.TeslaCommunication;

namespace TeslaComClient
{


    [Guid("EAA4976A-45C3-4BC5-000B-E474F4C3C83F")]
    public interface ComClass1Interface
    {
        bool Connect(string comPortName);
    }






    [Guid("0D53A3E8-E51A-49C7-004E-E72A2064F938")]
    [Serializable(), ClassInterface(ClassInterfaceType.AutoDual), ComVisible(true)]
    public class Main : ComClass1Interface
    {




        public Main(){
            string assemblyLoc = GetType().Assembly.Location;
            string configName = assemblyLoc + ".config";
            AppDomain.CurrentDomain.SetData("APP_CONFIG_FILE", configName);
        }


        CommunicationProtocolClient client;
        public bool Connect(string comPortName)
        {
            if (client == null)
            {
                client = new CommunicationProtocolClient();
                client.Open();
            }

            return client.Connect(comPortName);
        }
    }
}
