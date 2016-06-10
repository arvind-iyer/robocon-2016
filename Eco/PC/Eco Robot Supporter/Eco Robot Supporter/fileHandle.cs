using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace Eco_Robot_Supporter {
    class FileHandle {
        Main main;
        public FileHandle(Main main) {
            this.main = main;
        }

        public void saveConfig() {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Title = "Save Lemon's configuration";
            saveFileDialog.AddExtension = true;
            saveFileDialog.DefaultExt = ".config";
            saveFileDialog.FileName = "config";
            saveFileDialog.Filter = "Configuration (.config)|*.config";
            if (saveFileDialog.ShowDialog() == DialogResult.OK) {
                string json = JsonConvert.SerializeObject(main.config);
                File.WriteAllText(Path.GetFullPath(saveFileDialog.FileName), json);

                String formatted_text = String.Format("File saved. Path: {0}\r\n", Path.GetFullPath(saveFileDialog.FileName));
                main.updateDisplay(formatted_text);
            }
        }

        public void loadConfig() {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "Load Lemon's configuration";
            openFileDialog.Filter = "Configuration (.config)|*.config|All Files (*.*)|*.*";
            openFileDialog.FilterIndex = 1;

            if (openFileDialog.ShowDialog() == DialogResult.OK) {
                String json = File.ReadAllText(Path.GetFullPath(openFileDialog.FileName));
                main.config = JsonConvert.DeserializeObject<Config>(json);

                String formatted_text = String.Format("File loaded. Path: {0}\r\n", Path.GetFullPath(openFileDialog.FileName));
                main.updateDisplay(formatted_text);
            }
        }
    }
}
