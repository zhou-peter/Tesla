using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VimeoDownloader.JsClasses
{
    public class container
    {
        public ulong clip_id { get; set; }
        public string base_url { get; set; }
        [JsonProperty("video")]
        public video[] videos;
        [JsonProperty("audio")]
        public audio[] audios;
    }
}
