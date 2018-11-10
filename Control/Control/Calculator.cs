using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Control
{
    public class Calculator
    {
        public const int clock = 24000000;  //частота ядра
        public const int maxPeriod = 0xFFFF;//разрешение таймера

        public CalculationResult calculate(int freq, int prescaler, double delta, double width)
        {
            /*
     u32 freq=(u32)((float)Env.pwmFreq/100.0F);
    if (freq%2!=0)freq--;//четная частота
    freq/=2;//чтобы в этот период влезла вдвое ниже частота
    u16 prescaler=1;
    while ((SystemCoreClock/(freq*prescaler))>maxPeriod) prescaler++;
    int clock=SystemCoreClock/prescaler;
    u16 period=clock/freq;
    period/=2;
             */
            

            if (prescaler%2!=0)
            {
                MessageBox.Show("prescaler должен быть четным");
                return null;
            }

            if (freq % prescaler != 0)
            {
                while ((freq % prescaler != 0)) freq--;
                MessageBox.Show("ближайшая частота " + freq);
            }

            //теперь мы уверены что частота и предделитель кратны
            CalculationResult result = new CalculationResult();
            result.clock_prescaler = 1;
            while ((clock / (freq * prescaler)) >= maxPeriod) result.clock_prescaler++;//2

            result.freq_F1 = freq;

            if (result.clock_prescaler%2!=0)
            {
                result.clock_prescaler++;
            }
            result.main_clock = clock / result.clock_prescaler;//12 000 000

            //к примеру при частоте 300кГц и предделителе 2 будет 40
            result.period_F1 = clock / freq;//40
            if (result.period_F1 % 2 != 0)
            {
                result.period_F1--;
            }
            result.duty_F1 = 50.0F;

            //F0
            int multipF0 = prescaler * 2;
            result.freq_F0 = result.freq_F1 / multipF0;
            result.duty_F0 = 50.0F;
            result.period_F0 = result.period_F1 * multipF0;

            //F2
            result.freq_F2 = result.freq_F1 / prescaler;            
            result.period_F2 = result.period_F1 * prescaler;
            double periodsPerPercent = (double)result.period_F2 / 100F;
            result.duty_F2_start = (int)(delta * periodsPerPercent);
            result.duty_F2_stop = (int)((delta+width) * periodsPerPercent);
            //сколько периодов в проломе
            int periodsPegGap = result.duty_F2_stop - result.duty_F2_start;
            //сколько наносекунда длится период на этой частоте
            double f2TimeNano = (1.0F / result.freq_F2) * 1000000000;
            //время одного периода
            double periodTimeF2 = f2TimeNano / result.period_F2;
            //ширина пролома в наносекундах
            result.widthTime = (int)(periodsPegGap * periodTimeF2);

            //ширина полпериода F1
            double f1TimeNano = (1.0F / result.freq_F1) * 1000000000;
            //время одного периода
            double periodTimeF1 = f1TimeNano / result.period_F1;
            //время одного периода
            result.widthTimeF1 = (int)(0.5F * periodTimeF1);

            return result;
        }
    }
}
