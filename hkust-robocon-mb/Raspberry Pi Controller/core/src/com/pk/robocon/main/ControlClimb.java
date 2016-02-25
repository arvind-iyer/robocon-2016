package com.pk.robocon.main;

public class ControlClimb extends Control {

	public static final int CLIMB_UP = 1799;
	public static final int CLIMB_STOP = 0;
	public static final int CLIMB_DOWN = -600;

	/**
	 * magnitude of wheel
	 */
	public void executeClimb(int W) {
		super.setClimb(W);
	}

}
