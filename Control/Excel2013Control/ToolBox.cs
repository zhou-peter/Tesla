using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;
using Microsoft.Office.Tools.Ribbon;

namespace Excel2013Control
{
    public partial class ToolBox
    {
        private void ToolBox_Load(object sender, RibbonUIEventArgs e)
        {

        }

        void showMessage(string text)
        {
            System.Windows.Forms.MessageBox.Show(text);
        }

        private void button1_Click(object sender, RibbonControlEventArgs e)
        {
            showMessage("Click!");
        }
    }
}
