using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication.Packets
{
    /// <summary>
    /// Ошибка приема пакета железякой
    /// </summary>
    public class Packet_03 : AbstractInPacket
    {

        byte code = 0;

        public override void ApplyBody(byte[] buf, int offset, int size)
        {
            code = buf[offset];
        }

        public override void Process()
        {
            Console.WriteLine(DateTime.Now.ToString("HH:mm:ss ") + "Ошибка приема пакета железом: " + getErrorCode());
        }

        private string getErrorCode()
        {
            switch (code)
            {
                case 1:
                    return "Неправильный стартовый пакет";
                case 2:
                    return "Маленький размер пакета";
                case 3:
                    return "Большой размер пакета";
                case 4:
                    return "Ошибка CRC";
                case 6:
                    return "Таймаут получения";
                default:
                    return code.ToString();
            }
        }
    }
}
