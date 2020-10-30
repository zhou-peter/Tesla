using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VimeoDownloader.JsClasses
{
    public class video
    {
        public static string path = "video/";
        public string id { get; set; }
        public string base_url { get; set; }

        [JsonProperty("width")]
        public int width { get; set; }

        [JsonProperty("segments")]
        public segment[] segments { get; set; }

        [JsonProperty("init_segment")]
        public byte[] segment0 { get; set; }
    }
}
