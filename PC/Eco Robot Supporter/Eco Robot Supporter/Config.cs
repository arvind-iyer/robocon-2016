using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Eco_Robot_Supporter {
    public class Config {
        public const int SECTION = 6;
        public const int CALI = 3;
        public const int REGIONS = SECTION * CALI;

        public int[,,] colors = new int[2, REGIONS, 3];//[Color=1/White=0][REGION][RGB]
    }
}
