
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.ServiceModel;
using System.Threading;
using System.Windows;
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
        void Disconnect();
    }

    class TeslaServiceHost : ServiceHost
    {
        public TeslaServiceHost():
            base(Service.getInstance())
        {
            //thread = new Thread(new ThreadStart(()=> { Open(); }));
            
        }

        //public Thread thread;

        protected override void OnAbort()
        {
            base.OnAbort();
            Service.getInstance().OnClose();
        }
        protected override void OnClose(TimeSpan timeout)
        {
            base.OnClose(timeout);
            Service.getInstance().OnClose();
        }
    }



    /*InstanceContextMode необходимо установить на InstanceContextMode.Single. 
     * Это можно осуществить через атрибут ServiceBehaviorAttribute*/
    [ServiceBehaviorAttribute(InstanceContextMode =InstanceContextMode.Single)]
    public class Service : ICommunicationProtocol
    {

        static ServiceHost serviceHost;
        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type.
            using (serviceHost = new TeslaServiceHost())
            {
                // Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open();
                Service.getInstance().Init();

                // The service can now be accessed.
                Console.WriteLine("The service is ready.");
                SetConsoleCtrlHandler(new HandlerRoutine(ConsoleCtrlCheck), true);
                Console.WriteLine("CTRL+C,CTRL+BREAK or suppress the application to exit");
                Console.WriteLine();
                Console.ReadLine();
                // Close the ServiceHost.
                serviceHost.Close();

            }


        }


        #region Остановка с кнопки Х

        private static bool ConsoleCtrlCheck(CtrlTypes ctrlType)
        {
            // Put your own handler here
            Service.getInstance().OnClose();
            switch (ctrlType)
            {
                case CtrlTypes.CTRL_CLOSE_EVENT:
                    isclosing = true;
                    Console.WriteLine("Program being closed!");
                    break;
            }

            return true;
        }

        


        private static bool isclosing = false;
        // Declare the SetConsoleCtrlHandler function
        // as external and receiving a delegate.

        [DllImport("Kernel32")]
        public static extern bool SetConsoleCtrlHandler(HandlerRoutine Handler, bool Add);

        // A delegate type to be used as the handler routine
        // for SetConsoleCtrlHandler.
        public delegate bool HandlerRoutine(CtrlTypes CtrlType);

        // An enumerated type for the control messages
        // sent to the handler routine.

        public enum CtrlTypes
        {
            CTRL_C_EVENT = 0,
            CTRL_BREAK_EVENT,
            CTRL_CLOSE_EVENT,
            CTRL_LOGOFF_EVENT = 5,
            CTRL_SHUTDOWN_EVENT
        }

        #endregion









        private Service() { }
        static Service instance;
        public static Service getInstance()
        {
            if (instance == null)
            {
                instance = new Service();
            }
            return instance;
        }

        private void Init()
        {
            mgr = new PacketsManager();
        }

        PacketsManager mgr;
        SerialPort sp = null;
        bool connectionOpen = false;


        public void OnClose() {
            Disconnect();
        }


        public bool Connect(string comPortName)
        {
            //If we are not connected, connect
            if (connectionOpen == false)
            {
                try
                {
                    sp = new SerialPort(comPortName);
                    sp.BaudRate = 9600;
                    sp.Parity = Parity.None;
                    sp.DataBits = 8;
                    sp.Open();
                    //sp.DtrEnable = true;
                    sp.BaseStream.Flush();
                    sp.DataReceived += new SerialDataReceivedEventHandler(sp_DataReceived);

                    connectionOpen = true;
                    mgr.SetDataChannel(sp);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                    return false;
                }
            }
            return true;
        }


        void sp_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                int length = sp.BytesToRead;
                byte[] buf = new byte[length];
                sp.Read(buf, 0, length);
                if (mgr != null)
                {
                    mgr.AddBytes(buf);
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "On receive");
            }
        }

        public void Disconnect()
        {
            if (sp != null && sp.IsOpen)
            {
                sp.BaseStream.Flush();
                sp.Close();
            }
            connectionOpen = false;
        }
    }

}

