using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Control
{
    public class DataGridElement
    {
        string name;
        string value;
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        public string Value
        {
            get { return value; }
            set { this.value = value; }
        }
    }
}
