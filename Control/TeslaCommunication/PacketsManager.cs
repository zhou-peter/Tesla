using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using TeslaCommunication.Packets;

namespace TeslaCommunication
{
    public class PacketsManager
    {

        List<AbstractPacket> receivedPackets = new List<AbstractPacket>();
        List<AbstractPacket> packetsToSend = new List<AbstractPacket>();



        public void run()
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
                    Thread.Sleep(10);
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                shouldStop = true;
            }
        }

        public void AddBytes(byte[] buf)
        {
            int rxBytesNow = buf.Length;
            int length = MAX_BUF_SIZE_RX - rxIndex;
            Array.Copy(buf, 0, rxBuf, rxIndex, length);
            rxIndex += rxBytesNow;
            packetFormerCheck();
        }

 

        //region packet former

        static int MAX_BUF_SIZE_RX = 300;
        static int MAX_BUF_SIZE_TX = 1100;
        static int RECEIVE_TIMEOUT = 100;
        byte[] rxBuf = new byte[MAX_BUF_SIZE_RX];
        byte[] txBuf = new byte[MAX_BUF_SIZE_TX];
        int rxIndex = 0;
        int txIndex = 0;
        int timerCounter = 0;
        bool timerEnabled = false;
        bool shouldStop = false;


        public enum ReceiverStates
        {
            WaitingStart,
            ReceivingSize,
            ReceivingPacket,
            ReceivingTimeout,
            Processing
        }


        static int Processing = 5;
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
                if (rxIndex > 0 && rxBuf[0] != AbstractPacket.PACKET_START)
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
                    if (inPacksize < 6)
                    {
                        create_err("Слишком маленький пакет");
                        timerEnabled = false;
                        return;
                    }
                    if (inPacksize > MAX_BUF_SIZE_RX - 6)
                    {
                        create_err("Входящий пакет с слишком большим размером");
                        timerEnabled = false;
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
                timerEnabled = false;
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
                //если контрольная сумма сошлась
                if (crc == rxBuf[rxPackSize - 2] &&
                        (byte)(crc ^ (byte)0xAA) == rxBuf[rxPackSize - 1])
                {
                    processIncomingPacket();
                }
                else
                {//crc error
                    create_err("Ошибка CRC");
                    timerEnabled = false;
                    return;
                }
                rxIndex = 0;
                CommState = ReceiverStates.WaitingStart;
            }
        }

        private void processIncomingPacket()
        {
            
        }

        void create_err(string msg)
        {
            Console.WriteLine(msg);
        }
    }
}
