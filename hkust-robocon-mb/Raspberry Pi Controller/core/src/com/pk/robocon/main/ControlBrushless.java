package com.pk.robocon.main;

public class ControlBrushless extends Control {

	private final static int BRUSHLESS_MIN = 590;
	private final static int BRUSHLESS_MAX = 1050;

	public ControlBrushless() {

	}

	/**
	 * magnitude of brushless scaled 0->100
	 */
	public void executeBrushless(int M1, int M2) {
		super.setBrushless(M1 * (BRUSHLESS_MAX - BRUSHLESS_MIN) / 100 + BRUSHLESS_MIN,
				M2 * (BRUSHLESS_MAX - BRUSHLESS_MIN) / 100 + BRUSHLESS_MIN);
	}

}
