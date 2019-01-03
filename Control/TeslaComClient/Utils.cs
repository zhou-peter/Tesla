using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace TeslaComClient
{

    internal static class Utils
    {
        public static void Copy(object from, object to)
        {
            
            foreach (var toPropInfo in to.GetType().GetProperties())
            {
                PropertyInfo fromPropInfo = from.GetType().GetProperty(toPropInfo.Name);
                if (fromPropInfo != null)
                {
                    toPropInfo.SetValue(to, fromPropInfo.GetValue(from), null);
                }
            }
            /*
            foreach (var toPropInfo in to.GetType().GetFields())
            {
                FieldInfo fromPropInfo = from.GetType().GetField(toPropInfo.Name);
                if (fromPropInfo != null)
                {
                    toPropInfo.SetValue(to, fromPropInfo.GetValue(from));
                }
            }
            */
        }
    }
}
