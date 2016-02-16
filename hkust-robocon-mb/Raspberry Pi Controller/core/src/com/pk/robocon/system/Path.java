package com.pk.robocon.system;

import com.pk.robocon.main.ControlPID;

public class Path {
	private static final int STOP_DISTANCE = 2000;

	private Target target;
	private float err;

	public Path(Target t) {
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
				M = dist(ControlPID.getCurrentPosition(), target.getPosition()) * 100 / STOP_DISTANCE;
				if (M > 100) {
					M = 100;
				}
			}
			W = angleDiff(ControlPID.getCurrentPosition(), target.getPosition()) * 100 / 180;
		}
		System.out.println("move(" + M + ", " + bearing + ", " + W + ")");
		ControlPID.moveRobot(M, bearing, W);
	}

	public Target getTarget() {
		return target;
	}

	public boolean completed() {
		if (dist(ControlPID.getCurrentPosition(), target.getPosition()) > target.getThreshold().getDist_err()
				|| Math.abs(angleDiff(ControlPID.getCurrentPosition(), target.getPosition())) > target.getThreshold()
						.getAngle_err()) {
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
		int bearing = (int) (90 - Math.atan2(t.getY() - o.getY(), t.getX() - o.getY()) * 180 / Math.PI);
		if (bearing < -180) {
			bearing = bearing + 360;
		}
		if (bearing > 180) {
			bearing = bearing - 360;
		}
		return bearing;
	}

}