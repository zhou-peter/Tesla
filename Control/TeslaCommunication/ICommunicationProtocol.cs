using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.ServiceModel;
using System.Text;
using TeslaCommunication.Packets;

namespace TeslaCommunication
{


    // Define a service contract.
    [ServiceContract(Namespace = "http://STM32TeslaCommunication")]
    public interface ICommunicationProtocol
    {
        [OperationContract]
        bool Connect(string comPortName);

        [OperationContract]
        bool IsConnected();

        [OperationContract]
        void Disconnect();

        [OperationContract]
        void ClearQueues();

        [OperationContract]
        void setEnabled(byte num, bool enabled);

        [OperationContract]
        HardwareState getHardwareState();

        [OperationContract]
        void setTimersConfiguration(TimersConfiguration timersConfiguration);

        [OperationContract]
        void searchStart(int periodStart, int periodStop, int delay);

        [OperationContract]
        void searchStop();

        [OperationContract]
        void searchGeneratePWM(int period, int duty, byte num);

    }
}
