package com.pk.robocon.main;

import com.badlogic.gdx.math.Vector2;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

public class Debug {

	public static void main(String[] args) {
		ControlPID.addQueue(new Target(new Position(new Vector2(0, 0), -90),
				new Threshold(0, 0), 0));
		ControlPID.resetPathStart();
		ControlPID.execute();
	}

}
