using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VimeoDownloader.JsClasses
{
    public class audio
    {
        public static string path = "audio/";
        public ulong id { get; set; }
        public string base_url { get; set; }
        [JsonProperty("segments")]
        public segment[] segments { get; set; }

        [JsonProperty("init_segment")]
        public byte[] segment0 { get; set; }
    }
}
