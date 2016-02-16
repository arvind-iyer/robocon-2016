package com.pk.robocon.system;

public class Threshold {

	private int dist_err;
	private int angle_err;

	public Threshold(int dist, int angle) {
		this.dist_err = dist;
		this.angle_err = angle;
	}

	public int getDist_err() {
		return dist_err;
	}

	public int getAngle_err() {
		return angle_err;
	}

}
