using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TeslaDesktopClient
{
    public static class Extensions
    {
        public static void Invoke(this Control control, Action action)
        {
            control.Invoke((Delegate)action);
        }

        public static void SetChecked(this CheckBox chBox, bool check)
        {
            typeof(CheckBox).GetField("checkState", BindingFlags.NonPublic |
                                                    BindingFlags.Instance)
                            .SetValue(chBox, check ? CheckState.Checked :
                                                     CheckState.Unchecked);
            chBox.Invalidate();
        }
    }
}
