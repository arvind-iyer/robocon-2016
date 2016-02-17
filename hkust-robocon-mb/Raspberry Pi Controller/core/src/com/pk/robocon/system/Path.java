package com.pk.robocon.system;

import com.pk.robocon.main.ControlPID;

public class Path {

	private static final int MAX_VELOCITY = 140;
	private static final int STOP_DISTANCE = 2000;

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
		bearing = bearing(ControlPID.getCurrentPosition(), target.getPosition());
		if (!this.completed()) {
			if (target.getVel() != 0) {
				M = target.getVel();
			} else {
				M = dist(ControlPID.getCurrentPosition(), target.getPosition())
						* 100 / STOP_DISTANCE;
				if (M > 100) {
					M = 100;
				}
			}
			W = angleDiff(ControlPID.getCurrentPosition(), target.getPosition()) * 100 / 180;
		}
//		this.updateErr();
//		M = (int) (M * errDist);
//		W = (int) (W * errAngle);
		System.out.println("move(" + M + ", " + bearing + ", " + W + ")");
		this.move(M, bearing, W);
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
		this.lastAngleDiff = Math.abs(angleDiff(
				ControlPID.getCurrentPosition(), target.getPosition()));
		this.lastDist = dist(ControlPID.getCurrentPosition(),
				target.getPosition());
	}

	private boolean checkErrAngle() {
		if (Math.abs(angleDiff(ControlPID.getCurrentPosition(),
				target.getPosition())) > this.lastAngleDiff) {
			return false;
		}
		return true;
	}

	private boolean checkErrDist() {
		if (dist(ControlPID.getCurrentPosition(), target.getPosition()) >= this.lastDist) {
			return false;
		}
		return true;
	}

	public void resetErr() {
		this.errAngle = 1;
		this.errDist = 1;
	}

	public void move(int M, int bearing, int W) {
		double xComponent = (double) (M * Math.sin(bearing * Math.PI / 180)
				* MAX_VELOCITY / 100);
		double yComponent = (double) (M * Math.cos(bearing * Math.PI / 180)
				* MAX_VELOCITY / 100);
		int M1 = (int) ((-W - xComponent * 2) / 3);
		int M2 = (int) ((-W * 0.577f + xComponent * 0.577f - yComponent) / 1.73f);
		int M3 = -W - M1 - M2;
		ControlPID.sendMotorCommands(M1, M2, M3);
	}

	public Target getTarget() {
		return target;
	}

	public boolean completed() {
		if (dist(ControlPID.getCurrentPosition(), target.getPosition()) > target
				.getThreshold().getDist_err()
				|| Math.abs(angleDiff(ControlPID.getCurrentPosition(),
						target.getPosition())) > target.getThreshold()
						.getAngle_err()) {
			return false;
		}
		return true;
	}

	private static int dist(Position o, Position t) {
		return (int) (Math.sqrt(Math.pow(t.getX() - o.getX(), 2)
				+ Math.pow(t.getY() - o.getY(), 2)));
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
		int bearing = (int) (90 - Math.atan2(t.getY() - o.getY(),
				t.getX() - o.getY())
				* 180 / Math.PI);
		bearing = bearing - o.getBearing();
		if (bearing < -180) {
			bearing = bearing + 360;
		}
		if (bearing > 180) {
			bearing = bearing - 360;
		}
		return bearing;
	}

}