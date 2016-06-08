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
            saveFileDialog.ShowDialog();
            if (saveFileDialog.FileName != "") {
                FileStream fileStream = (FileStream)saveFileDialog.OpenFile();
                string json = JsonConvert.SerializeObject(main.config);
                byte[] info = new UTF8Encoding(true).GetBytes(json);
                fileStream.Write(info, 0, info.Length);
                fileStream.Close();
            }
        }
    }
}
