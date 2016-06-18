using System;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace Eco_Robot_Supporter {
    public partial class Main : Form {

        public SerialPort port;
        private Panel[,,] colorBoxes;
        private Label[,] labels;
        public Config config = new Config();
        public RichTextBox console_box;
        delegate void updateDisplayDelegate(String msg);

        public Main() {
            InitializeComponent();
            colorBoxes = new Panel[,,] { { { RS1_white, RS1_color }, { RS2_white, RS2_color}, { RS3_white, RS3_color} },
                                          { { BS1_white, BS1_color}, { BS2_white, BS2_color}, { BS3_white, BS3_color} },
                                          { { GU1_white, GU1_color }, { GU2_white, GU2_color}, { GU3_white, GU3_color } },
                                          { { OH1_white, OH1_color }, { OH2_white, OH2_color}, { OH3_white, OH3_color } },
                                          { { RB1_white, RB1_color }, { RB2_white, RB2_color}, { RB3_white, RB3_color } },
                                          { { DS1_white, DS1_color }, { DS2_white, DS2_color}, { DS3_white, DS3_color } } };

            labels = new Label[,] { { RS1_label, RS2_label, RS3_label },
                                           { BS1_label, BS2_label, BS3_label },
                                           { GU1_label, GU2_label, GU3_label },
                                           { OH1_label, OH2_label, OH3_label },
                                           { RB1_label, RB2_label, RB3_label },
                                           { DS1_label, DS2_label, DS3_label } };
            console_box = console;
        }

        private void Form1_Load(object sender, EventArgs e) {
            connect_label.ForeColor = Color.IndianRed;
        }

        private void but_connect_1_Click(object sender, EventArgs e) {
            port = new SerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            openPort();
        }

        private void but_connect_2_Click(object sender, EventArgs e) {
            port = new SerialPort("COM21", 115200, Parity.None, 8, StopBits.One);
            openPort();
        }

        private void openPort() {
            console.AppendText("Opening " + port.PortName + "...\r\n");
            try {
                port.Open();
            } catch (IOException ex) {
                console.AppendText("Failed to open " + port.PortName + "...\r\n");
                console.AppendText(ex.Message + "\r\n");
            }
            console.AppendText("Port opened.\r\n");
            connect_label.Text = "Status: Connected";
            connect_label.ForeColor = Color.Green;
            port.DataReceived += colorDataReceiver;
        }

        public void updateDisplay(String msg) {
            if (InvokeRequired) {
                this.BeginInvoke(new updateDisplayDelegate(updateDisplay), new object[] {msg});
                return;
            }
            for (int section = 0; section < Config.SECTION; section++) {
                for (int cali = 0; cali < Config.CALI; cali++) {
                    for (int part = 0; part < 2; part++) {
                        colorBoxes[section, cali, part].BackColor = Color.FromArgb(
                            (config.colors[part, section * Config.CALI + cali, 0] /2)>255?255: (config.colors[part, section * Config.CALI + cali, 0] / 2),
                             (config.colors[part, section * Config.CALI + cali, 1] / 2) > 255 ? 255 : (config.colors[part, section * Config.CALI + cali, 1] / 2),
                             (config.colors[part, section * Config.CALI + cali, 2] / 2) > 255 ? 255 : (config.colors[part, section * Config.CALI + cali, 2] / 2));
                    }
                    labels[section, cali].Text = String.Format("R{0} G{1} B{2}\nR{3} G{4} B{5}",
                        config.colors[1, section * Config.CALI + cali, 0], config.colors[1, section * Config.CALI + cali, 1],
                        config.colors[1, section * Config.CALI + cali, 2], config.colors[0, section * Config.CALI + cali, 0],
                        config.colors[0, section * Config.CALI + cali, 1], config.colors[0, section * Config.CALI + cali, 2]);
                }
            }
            console.AppendText(msg);
        }

        private void colorDataReceiver(Object sender, SerialDataReceivedEventArgs e) {
            while ((sender as SerialPort).BytesToRead>=3) {
                byte[] buffer = new byte[3];
                (sender as SerialPort).Read(buffer, 0, 3);
                int region = buffer[0] & 31;            //Mask of 00011111 0~18
                int type = (buffer[0] & 128)>>7;        //Mask of 10000000 0-White 1-Color
                int rgb = ((buffer[0] & 96)>>5) - 1;    //Mask of 01100000 01-Red 10-Green 11-Blue
                int color = (buffer[1]<<8) | buffer[2];
                String msg = String.Format("Received! Region: {0}, Type: {1}, RGB: {2}, Color: {3}, Raw: {4} {5}\n",
                        region, type, rgb, color, buffer[0], buffer[1], buffer[2]);
                config.colors[type, region, rgb] = color;
                Console.Write(msg);
                updateDisplay(msg);
            }
        }

        private void but_save_Click(object sender, EventArgs e) {
            FileHandle fileHandle = new FileHandle(this);
            fileHandle.saveConfig();
        }

        private void but_load_Click(object sender, EventArgs e) {
            FileHandle fileHandle = new FileHandle(this);
            fileHandle.loadConfig();
        }

        private void but_pass_Click(object sender, EventArgs e) {

        }
    }
}
