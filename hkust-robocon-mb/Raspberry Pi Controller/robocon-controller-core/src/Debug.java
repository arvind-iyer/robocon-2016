import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;
import com.dranithix.robocon.RobotTarget;
import com.dranithix.robocon.systems.RobotControlSystem;

public class Debug {

	public static void main(String[] args) {
		RobotControlSystem Test=new RobotControlSystem(null);
		ArrayList<RobotTarget> control=new ArrayList<RobotTarget>();
		control.add(new RobotTarget(new Vector2(0, 0), 0, 0, 0, 0));
		control.add(new RobotTarget(new Vector2(0, 1000), 0, 0, 0, 0));
		Test._curve(control);
	}
	
}
