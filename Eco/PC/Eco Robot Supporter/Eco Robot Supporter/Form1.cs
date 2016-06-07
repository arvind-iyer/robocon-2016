using System;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace Eco_Robot_Supporter {
    public partial class Main : Form {

        private SerialPort port;

        public Main() {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e) {

        }

        private void but_connect_1_Click(object sender, EventArgs e) {
            port = new SerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            open_port();
        }

        private void but_connect_2_Click(object sender, EventArgs e) {
            port = new SerialPort("COM15", 115200, Parity.None, 8, StopBits.One);
            open_port();
        }

        private void open_port() {
            console.AppendText("Opening " + port.PortName + "...\r\n");
            try {
                port.Open();
            } catch (IOException ex) {
                console.AppendText("Failed to open " + port.PortName + "...\r\n");
                console.AppendText(ex.Message + "\r\n");
            }
        }
    }
}
