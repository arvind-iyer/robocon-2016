package com.dranithix.robocon.control.system;

public class PID {

	private int startX;
	private int startY;
	public Target target;

	public PID(Target t) {
		this.target = t;
	}

	public Target getTarget() {
		return target;
	}

	public void setTarget(Target target) {
		this.target = target;
	}

	public void straight() {

	}

	private int angleDiff(Position o, Position t) {
		int diff = t.bearing - o.bearing;
		if (diff < -180) {
			diff = diff + 360;
		}
		if (diff > 180) {
			diff = diff - 360;
		}
		return diff;
	}

	private int _getX() {
		return 0;
	}

	private int _getY() {
		return 0;
	}

}
