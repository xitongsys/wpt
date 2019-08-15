using System;
using System.Collections.Generic;
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
            textPort.Text = cfg.port.ToString();
            
            if (cfg.role.ToLower() == "server")
            {
                MessageBox.Show(cfg.role);
                comboRole.SelectedItem = 1;
            }
            else
            {
                comboRole.SelectedItem = 0;
            }
        }

        public void LoadUItoConfig()
        {
            cfg.server = textServer.Text;
            cfg.role = ((ComboBoxItem)comboRole.SelectedItem).Content.ToString().ToLower();
            cfg.port = Int32.Parse(textPort.Text);
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
    }
}
