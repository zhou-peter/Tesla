
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
            /*
            TimersConfiguration tc = new TimersConfiguration();
            tc.dutyBunch = 100;
            var a = new Packet_04(tc).GetBody();
            */
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
                //Service.getInstance().Connect("COM2");
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


        public void OnClose() {
            Disconnect();
            mgr.Dispose();
        }

        string comPortName;
        public bool Connect(string comPortName)
        {
            //If we are not connected, connect
            if (sp==null)
            {
                try
                {
                    ClearQueues();
                    sp = new SerialPort(comPortName);
                    sp.BaudRate = 9600;
                    sp.Parity = Parity.None;
                    sp.DataBits = 8;
                    sp.StopBits = StopBits.Two;
                    sp.Open();
                    this.comPortName = comPortName;
                    sp.BaseStream.Flush();
                    sp.DataReceived += new SerialDataReceivedEventHandler(sp_DataReceived);
                    sp.ErrorReceived += Sp_ErrorReceived;
                    mgr.SetDataChannel(sp);
                    return true;
                }
                catch (Exception ex)
                {
                    sp = null;
                    Console.WriteLine(ex.ToString());
                    return false;
                }
            }
            return false;
        }

        private void Sp_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            Console.WriteLine("Serial port error " + e.EventType.ToString());
            //Disconnect();
            //Thread.Sleep(500);
            //Connect(comPortName);
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
                Console.WriteLine(ex.ToString(), "On receive");
            }
        }

        public void Disconnect()
        {
            if (sp != null)
            {
                if (sp.IsOpen) { 
                    sp.BaseStream.Flush();
                    sp.Close();
                }
                sp.Dispose();
            }
            sp = null;
        }

        public bool IsConnected()
        {
            if (sp == null)
                return false;
            return sp.IsOpen;
        }

        public void ClearQueues()
        {
            mgr.packetsToSend.Clear();
            mgr.receivedPackets.Clear();
        }

        public void setEnabled(byte num, bool enabled)
        {
            Packet_02 p2 = new Packet_02(num,enabled);
            mgr.packetsToSend.Enqueue(p2);
        }

        public HardwareState getHardwareState()
        {
            return new HardwareState(mgr.currentState);
        }

        public void setTimersConfiguration(TimersConfiguration timersConfiguration)
        {
            Packet_04 p4 = new Packet_04(timersConfiguration);
            mgr.packetsToSend.Enqueue(p4);           

        }

        public void searchStart(int periodStart, int periodStop, int delay)
        {
            Packet_06 p = new Packet_06(periodStart,periodStop,delay);
            mgr.packetsToSend.Enqueue(p);
        }

        public void searchStop()
        {
            Packet_08 p = new Packet_08();
            mgr.packetsToSend.Enqueue(p);
        }

        public void searchGeneratePWM(int period)
        {
            Packet_0A p = new Packet_0A(period);
            mgr.packetsToSend.Enqueue(p);
        }
    }

}

