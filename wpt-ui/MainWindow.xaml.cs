using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace wpt_ui
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    public class TunPortItems
    {
        public int client { get; set; }
        public int server { get; set; }
    }
    public partial class MainWindow : Window
    {
        public Config cfg;
        public MainWindow()
        {
            InitializeComponent();

            cfg = new Config();
            try
            {
                cfg.ReadConfig();
            }
            catch (Exception e)
            {
                MessageBox.Show("Can't open config.txt: " + e.ToString());

            }

            LoadConfigToUI();
        }

        public void LoadConfigToUI()
        {
            textServer.Text = cfg.server;

            if (cfg.role.ToLower() == "server")
            {
                comboRole.SelectedIndex = 1;
            }
            else
            {
                comboRole.SelectedIndex = 0;
            }


            comboDirection.SelectedIndex = cfg.direction;
            
            for(int i=0; i<cfg.client_tun_ports.Count && i<cfg.server_tun_ports.Count; i++)
            {
                int c = (int)cfg.client_tun_ports[i], s = (int)cfg.server_tun_ports[i];
                listViewTunPorts.Items.Add(new TunPortItems { client = c, server = s });
            }

        }

        public void LoadUItoConfig()
        {
            cfg.server = textServer.Text;
            cfg.role = ((ComboBoxItem)comboRole.SelectedItem).Content.ToString().ToLower();
            cfg.direction = comboDirection.SelectedIndex;
            int pn = listViewTunPorts.Items.Count;
            cfg.client_tun_ports.Clear();
            cfg.server_tun_ports.Clear();
            for (int i = 0; i < pn; i++)
            {
                var it = listViewTunPorts.Items[i] as TunPortItems;
                int c = it.client, s = it.server;
                if(c>0 && s>0 && c<65536 && s < 65536)
                {
                    cfg.client_tun_ports.Add(c);
                    cfg.server_tun_ports.Add(s);
                }
            }
        }

        public Process proc;
        public void RunPangolin()
        {
            ProcessStartInfo psi = new ProcessStartInfo("wpt.exe")
            {
                UseShellExecute = false,
                RedirectStandardInput = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            };

            proc = new Process()
            {
                StartInfo = psi
            };

            proc.Start();
            proc.WaitForExit();
        }

        private void Button_Click_Start(object sender, RoutedEventArgs e)
        {
            buttonStart.IsEnabled = false;
            LoadUItoConfig();
            cfg.SaveConfig();
            Thread thr = new Thread(RunPangolin);
            thr.Start();
        }

        private void Button_Click_Stop(object sender, RoutedEventArgs e)
        {
            buttonStart.IsEnabled = true;
            try
            {
                proc.Kill();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void ButtonAddTunPort_Click(object sender, RoutedEventArgs e)
        {
            listViewTunPorts.Items.Add(new TunPortItems { client = 0, server = 0 });
        }
    }
}
