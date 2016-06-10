using System;
using System.Drawing;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Eco_Robot_Supporter {
    class MyColor {
        int[] raw_rgb = new int[3];
        int[] rgb_255 = new int[3];

        public MyColor(int r, int g, int b) {
            raw_rgb[0] = r;
            raw_rgb[1] = g;
            raw_rgb[2] = b;
        }

        public MyColor(int[] rgb) {
            if (rgb.Length != 3) {
                throw new Exception();
            }
            raw_rgb = rgb;
        }

        private void convert_to_255() {
            for (int i = 0; i < 3; i++) {
                rgb_255[i] = raw_rgb[i] *255 / 512;
            }
        }

        public Color get_color() {
            return Color.FromArgb(rgb_255[0], rgb_255[1], rgb_255[2]);
        }

    }
}
