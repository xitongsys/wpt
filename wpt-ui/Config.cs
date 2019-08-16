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
        public ArrayList client_tun_ports;
        [DataMember]
        public ArrayList server_tun_ports;
        [DataMember]
        public int direction;

        public Config()
        {
            role = "client";
            server = "0.0.0.0:2222";
            direction = 0;
            client_tun_ports = new ArrayList();
            server_tun_ports = new ArrayList();
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
            direction = c.direction;
            client_tun_ports = c.client_tun_ports;
            server_tun_ports = c.server_tun_ports;
        }
    }
}
