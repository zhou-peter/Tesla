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

        public CalculationResult calculate(int freq, int prescaler, double delta)
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

            
         
            return result;
        }
    }
}
