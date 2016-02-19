package com.pk.robocon.system;

import com.badlogic.gdx.math.MathUtils;
import com.pk.robocon.main.Control;

public class Path {

	private static final int STOP_DISTANCE = 2000;

	private float M = 0;
	private int bearing = 0;
	private float W = 0;

	private Target target;
	private float errAngle = 1f;
	private float errDist = 1f;
	private int lastDist = 0;
	private int lastAngleDiff = 0;

	public Path(Target t) {
		this.errDist = 1f;
		this.errAngle = 1f;
		this.target = t;
	}

	public float translationPID_M() {
		float M = 0;
		if (target.getVel() != 0) {
			M = target.getVel();
		} else {
			M = dist(Control.getCurrentPos(), target.getPosition()) * 100.0f / STOP_DISTANCE;
			if (M > 100f) {
				M = 100f;
			}
		}
		//System.out.println("preM: " + M);
		return M;
	}

	public int translationPID_bearing() {
		return bearing(Control.getCurrentPos(), target.getPosition());
	}

	public float OrientationPID() {
		return angleDiff(Control.getCurrentPos(), target.getPosition()); // *100/180
	}

	public void calculateLinearPath() {
		this.bearing = this.translationPID_bearing();
		if (!this.completed()) {
			this.M = this.translationPID_M();
			this.W = this.OrientationPID();
		}
		this.updateErr();
		this.M = this.M * this.errDist;
		this.W = this.W * this.errAngle;
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
		// System.out.println("err: " + errDist + " " + errAngle);
	}

	public float getErrAngle() {
		return this.errAngle;
	}

	public float getErrDist() {
		return this.errDist;
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
		this.errAngle = 1f;
		this.errDist = 1f;
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

	public float getM() {
		return this.M;
	}

	public int getBearing() {
		return this.bearing;
	}

	public float getW() {
		return this.W;
	}

}