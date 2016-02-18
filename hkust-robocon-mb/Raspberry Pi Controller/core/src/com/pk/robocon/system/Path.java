package com.pk.robocon.system;

import com.badlogic.gdx.math.MathUtils;
import com.pk.robocon.main.Control;

public class Path {
	private static final int STOP_DISTANCE = 2000;

	private int M1;
	private int M2;
	private int M3;

	private Target target;
	private float errAngle = 1;
	private float errDist = 1;
	private int lastDist = 0;
	private int lastAngleDiff = 0;

	public Path(Target t) {
		this.errDist = 1;
		this.errAngle = 1;
		this.target = t;
	}

	public void straight() {
		int M = 0;
		int bearing;
		int W = 0;
		bearing = bearing(Control.getCurrentPos(), target.getPosition());
		if (!this.completed()) {
			if (target.getVel() != 0) {
				M = target.getVel();
			} else {
				M = dist(Control.getCurrentPos(), target.getPosition()) * 100 / STOP_DISTANCE;
				if (M > 100) {
					M = 100;
				}
			}
			W = angleDiff(Control.getCurrentPos(), target.getPosition()) * 100 / 180;
		}
		this.updateErr();
		M = (int) (M * errDist);
		W = (int) (W * errAngle);

		Integer[] motorValues = Control.calculateRobotValues(M, bearing, W);
		this.M1 = motorValues[0];
		this.M2 = motorValues[1];
		this.M3 = motorValues[2];
	}

	private void updateErr() {
		if (!this.checkErrAngle()) {
			errAngle = errAngle + 0.03f;
		} else {
			errAngle = errAngle * 0.8f + 0.2f;
		}
		if (!this.checkErrDist()) {
			errDist = errDist + 0.03f;
		} else {
			errDist = errDist * 0.8f + 0.2f;
		}
		this.lastAngleDiff = Math.abs(angleDiff(Control.getCurrentPos(), target.getPosition()));
		this.lastDist = dist(Control.getCurrentPos(), target.getPosition());
	}

	private boolean checkErrAngle() {
		if (Math.abs(angleDiff(Control.getCurrentPos(), target.getPosition())) > this.lastAngleDiff) {
			return false;
		}
		return true;
	}

	private boolean checkErrDist() {
		if (dist(Control.getCurrentPos(), target.getPosition()) >= this.lastDist) {
			return false;
		}
		return true;
	}

	public void resetErr() {
		this.errAngle = 1;
		this.errDist = 1;
	}

	public Target getTarget() {
		return target;
	}

	/**
	 * Returns true if path is completed
	 * 
	 * @return
	 */
	public boolean completed() {
		if (dist(Control.getCurrentPos(), target.getPosition()) > target.getThreshold().getDist_err() || Math
				.abs(angleDiff(Control.getCurrentPos(), target.getPosition())) > target.getThreshold().getAngle_err()) {
			return false;
		}
		return true;
	}

	private static int dist(Position o, Position t) {
		return (int) (Math.sqrt(Math.pow(t.getX() - o.getX(), 2) + Math.pow(t.getY() - o.getY(), 2)));
	}

	private static int angleDiff(Position o, Position t) {
		int diff = t.getBearing() - o.getBearing();
		if (diff < -180) {
			diff = diff + 360;
		}
		if (diff > 180) {
			diff = diff - 360;
		}
		return diff;
	}

	private static int bearing(Position o, Position t) {
		// int bearing = (int) (90 - Math.atan2(t.getY() - o.getY(), t.getX() -
		// o.getX()) * 180 / Math.PI);
		int bearing = (int) (90
				- MathUtils.atan2(t.getY() - o.getY(), t.getX() - o.getX()) * MathUtils.radiansToDegrees);
		bearing = bearing - o.getBearing();
		if (bearing < -180) {
			bearing = bearing + 360;
		}
		if (bearing > 180) {
			bearing = bearing - 360;
		}
		return bearing;
	}

	public int getM1() {
		return this.M1;
	}

	public int getM2() {
		return this.M2;
	}

	public int getM3() {
		return this.M3;
	}

}