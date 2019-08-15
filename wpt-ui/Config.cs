using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

namespace wpt_ui
{
    [DataContract]
    public class Config
    {
        [DataMember]
        public string role;
        [DataMember]
        public string server;
        [DataMember]
        public int port;

        public Config()
        {
            role = "client";
            server = "0.0.0.0:2222";
            port = 3389;
        }

        public void ReadConfig()
        {
            string cs = System.IO.File.ReadAllText("config.txt");
            var ms = new MemoryStream(Encoding.UTF8.GetBytes(cs));
            var ser = new DataContractJsonSerializer(this.GetType());
            var desConfig = new Config();
            desConfig = ser.ReadObject(ms) as Config;
            ms.Close();
            Assign(desConfig);
        }

        public void SaveConfig()
        {
            var ms = new MemoryStream();
            var ser = new DataContractJsonSerializer(this.GetType());
            ser.WriteObject(ms, this);
            ms.Position = 0;
            var sr = new StreamReader(ms);
            System.IO.File.WriteAllText("config.txt", sr.ReadToEnd());
        }

        public void Assign(Config c)
        {
            role = c.role;
            server = c.server;
            port = c.port;
        }
    }
}
