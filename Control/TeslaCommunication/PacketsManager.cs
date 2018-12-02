using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Windows;
using TeslaCommunication.Packets;

namespace TeslaCommunication
{
    public class PacketsManager : IDisposable
    {

        public Queue<AbstractInPacket> receivedPackets = new Queue<AbstractInPacket>();
        public Queue<AbstractOutPacket> packetsToSend = new Queue<AbstractOutPacket>();
        Thread thread;
        bool timerEnabled = false;
        bool shouldStop = false;
        public PacketsManager()
        {
            thread = new Thread(new ThreadStart(run));
            thread.Start();
        }


        public void Dispose()
        {
            shouldStop = true;
            thread.Join();
        }

        void run()
        {

            try
            {


                while (!shouldStop)
                {
                    if (timerEnabled)
                    {//эмитация таймера
                        timerCounter++;
                        if (timerCounter > RECEIVE_TIMEOUT)
                        {
                            CommState = ReceiverStates.ReceivingTimeout;
                        }
                    }
                    if (receivedPackets.Count > 0)
                    {
                        processIncomingPacket();
                    }
                    if (packetsToSend.Count > 0)
                    {
                        sendOutPacket();
                    }
                    Thread.Sleep(10);
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                shouldStop = true;
            }
        }

        public StateStruct currentState;

        void sendOutPacket()
        {
            if (sp.IsOpen && packetsToSend.Count > 0)
            {
                AbstractOutPacket packet = packetsToSend.Dequeue();
                byte[] buf = packet.ToArray();
                sp.BaseStream.Write(buf, 0, buf.Length);
                sp.BaseStream.Flush();
            }
        }


        private void processIncomingPacket()
        {
            while (receivedPackets.Count > 0)
            {
                AbstractInPacket pack = receivedPackets.Dequeue();
                if (pack.Command == 0x01)
                {
                    currentState = ((Packet_01)pack).state;
                }
                else
                {
                    pack.Process();
                }
            }
        }

        public void AddBytes(byte[] buf)
        {
            int rxBytesNow = buf.Length;
            int length = MAX_BUF_SIZE_RX - rxIndex;
            if (rxBytesNow > length)
            {
                create_err("Накопилось много данных");
                return;
            }
            Array.Copy(buf, 0, rxBuf, rxIndex, rxBytesNow);
            rxIndex += rxBytesNow;
            packetFormerCheck();
        }

 

        //region packet former

        static int MAX_BUF_SIZE_RX = 300;
        static int RECEIVE_TIMEOUT = 50; //500ms
        static int EMPTY_SIZE = 6;
        byte[] rxBuf = new byte[MAX_BUF_SIZE_RX];
        int rxIndex = 0;
        int timerCounter = 0;

        SerialPort sp;

        public enum ReceiverStates
        {
            WaitingStart,
            ReceivingSize,
            ReceivingPacket,
            ReceivingTimeout,
            Processing
        }


        ReceiverStates CommState = ReceiverStates.WaitingStart;
        int rxPackSize = 0;
        void packetFormerCheck()
        {

            if (CommState == ReceiverStates.WaitingStart)
            {
                //ждем новый пакет
                //статус может изменить таймер таймаута
                if (rxIndex == 0)
                {
                    return;
                }
                if (rxIndex > 0 && rxBuf[0] != Utils.PACKET_START)
                {
                    rxIndex = 0;
                    create_err("Неправильный стартовый пакет");
                    return;
                }
                else
                {
                    CommState = ReceiverStates.ReceivingSize;
                    rxPackSize = 0;
                    timerEnabled = true;
                    timerCounter = 0;
                }
            }
            if (CommState == ReceiverStates.ReceivingSize)
            {
                if (rxIndex > 3)
                {
                    int inPacksize = rxBuf[1] | rxBuf[2] << 8;
                    if (inPacksize < EMPTY_SIZE)
                    {
                        create_err("Слишком маленький пакет");
                        return;
                    }
                    if (inPacksize > MAX_BUF_SIZE_RX - EMPTY_SIZE)
                    {
                        create_err("Входящий пакет с слишком большим размером");                        
                        return;
                    }
                    rxPackSize = inPacksize;
                    CommState = ReceiverStates.ReceivingPacket;
                }
                else
                {
                    return;
                }
            }
            if (CommState == ReceiverStates.ReceivingPacket)
            {
                if (rxIndex >= rxPackSize)
                {
                    //Пакет пришел. останавливаем таймер и проверяем его
                    timerEnabled = false;
                    CommState = ReceiverStates.Processing;
                }

            }
            if (CommState == ReceiverStates.ReceivingTimeout)
            {
                create_err("Таймаут получения пакета");
            }
            if (CommState == ReceiverStates.Processing)
            {
                //останавливаем таймер таймаута так как пришел весь пакет
                timerEnabled = false;
                //check CRC
                byte crc = 0;
                //265-2=263
                for (int i = 0; i < rxPackSize - 2; i++)
                {
                    crc += rxBuf[i];
                }
                byte crcXOR = (byte)(crc ^ (byte)0xAA);
                //если контрольная сумма сошлась
                if (crc == rxBuf[rxPackSize - 2] &&
                         crcXOR == rxBuf[rxPackSize - 1])
                {
                    enqeueIncomingPacket();
                    rxIndex = 0;
                    CommState = ReceiverStates.WaitingStart;
                }
                else
                {//crc error
                    create_err("Ошибка CRC");
                    return;
                }
            }
        }

        internal void SetDataChannel(SerialPort sp)
        {
            this.sp = sp;
        }

        private void enqeueIncomingPacket()
        {
            int inPacksize = rxBuf[1] | rxBuf[2] << 8;
            string packetNumber = rxBuf[3].ToString("X2");
            string packetName = "TeslaCommunication.Packets.Packet_" + packetNumber;
            try
            {
                AbstractInPacket pack = null;                
                Type t = Type.GetType(packetName);
                ConstructorInfo c = t.GetConstructor(new Type[] { });
                pack = (AbstractInPacket)c.Invoke(new object[] { });
                
                if (pack != null)
                {
                    pack.Command = rxBuf[3];
                    int bodySize = inPacksize - EMPTY_SIZE;
                    if (bodySize>0)
                    {
                        pack.BodySize = bodySize;
                        pack.ApplyBody(rxBuf, 4, bodySize);
                    }
                    receivedPackets.Enqueue(pack);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error instantiate packet " + packetName);
            }
        }

 

        void create_err(string msg)
        {
            rxIndex = 0;
            CommState = ReceiverStates.WaitingStart;
            timerEnabled = false;
            Console.WriteLine(msg);
        }

    }
}
