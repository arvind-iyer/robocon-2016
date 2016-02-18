package com.pk.robocon.main;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;
import com.dranithix.robocon.RobotSerialManager;
import com.dranithix.robocon.net.events.MotorControlEvent;
import com.pk.robocon.system.Path;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

public class ControlPID {
	public static final int MAX_PWM_VELOCITY = 140;

	private static ArrayList<Path> queue = new ArrayList<Path>();

	private static Position currentPosition = new Position(new Vector2(0, 0), 0);

	public static ArrayList<Path> getQueue() {
		return queue;
	}

	public static Position getCurrentPosition() {
		return currentPosition;
	}

	public static void addQueue(Target t) {
		queue.add(new Path(t));
	}

	public static void addCurve(ArrayList<Target> control) {
		for (double t = 0; t <= 1; t = t + 0.2) {
			float x = 0;
			float y = 0;
			for (int i = 0; i != control.size(); i++) {
				double c = binomial(control.size() - 1, i)
						* Math.pow(1 - t, control.size() - i - 1)
						* Math.pow(t, i);
				x = (float) (x + c * control.get(i).getPosition().getX());
				y = (float) (y + c * control.get(i).getPosition().getY());
			}
			addQueue(new Target(new Position(new Vector2((int) (x), (int) (y)),
					0), new Threshold(50, 360), 20));
		}
	}

	public static void updateGyroPosition(Vector2 gyroPos, int gyroAngle) {
		ControlPID.currentPosition.setPos(gyroPos.cpy());
		ControlPID.currentPosition.setBearing(gyroAngle);

		System.out.println(ControlPID.currentPosition.getX() + " "
				+ ControlPID.currentPosition.getY());
	}

	public static boolean execute() {
		if (queue.size() > 0)
			queue.get(0).straight();
		return queue.size() > 0;
	}

	public static void nextPID() {
		if (!queue.isEmpty()) {
			queue.remove(0);
		}
	}

	public static void moveRobot(int M, int bearing, int W) {
		double xComponent = (double) (M * Math.sin(bearing * Math.PI / 180)
				* MAX_PWM_VELOCITY / 100);
		double yComponent = (double) (M * Math.cos(bearing * Math.PI / 180)
				* MAX_PWM_VELOCITY / 100);
		int M1 = (int) ((-W - xComponent * 2) / 3);
		int M2 = (int) ((-W * 0.577f + xComponent * 0.577f - yComponent) / 1.73f);
		int M3 = -W - M1 - M2;
		sendMotorCommands(M1, M2, M3);
	}

	public static void sendMotorCommands(int M1, int M2, int M3) {
		RobotSerialManager.getInstance().sendEvent(
				new MotorControlEvent(M1, M2, M3, 0, 0, 0));
	}

	private static int binomial(int n, int k) {
		if (k > n - k) {
			k = n - k;
		}
		int b = 1;
		for (int i = 1, m = n; i <= k; i++, m--)
			b = b * m / i;
		return b;
	}

}