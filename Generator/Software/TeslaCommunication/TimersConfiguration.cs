using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TeslaCommunication
{
    [Serializable]
    public class TimersConfiguration
    {
        /*
        Период несущей
        Период проломов
        Старт пролома
        Стоп пролома
        Период пачки
        Скважность пачки
        Отступ пролома в рабочей области пачки
        Стоп пролома в рабочей области пачки
        Отступ пропуск верхнее плечо
        Стоп пропуска верхнего плеча
        Отступ пропуск нижнее плечо
        Стоп пропуска нижнее плеча
        */

        //public static readonly int FieldsCount = 12;

        public int periodCarrier;
        public int periodGap;
        public int onGap;
        public int offGap;
        public int periodBunch;
        public int dutyBunch;
        public int startGap;
        public int stopGap;
        public int startHigh;
        public int stopHigh;
        public int startLow;
        public int stopLow;
    }
}
