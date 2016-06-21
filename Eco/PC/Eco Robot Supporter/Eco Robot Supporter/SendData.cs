using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Eco_Robot_Supporter {
    class SendData {
        SerialPort port;
        public SendData(SerialPort port) {
            this.port = port;
        }

        public void sendConfig(Config config) {

        }
    }
}
