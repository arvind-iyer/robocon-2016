package com.pk.robocon.main;


public class ControlClimb {

	private static final int CLIMB_UP = 1799;
	private static final int CLIMB_STOP = 0;
	private static final int CLIMB_DOWN = -600;

	public ControlClimb() {

	}

	/**
	 * magnitude of brushless scaled 0->100
	 */
	public static void executeBrushless(int M1, int M2) {

	}

	private static void sendClimb(int W) {
//		motorsClimb[0].send(W);
//		motorsClimb[1].send(W);
//		motorsClimb[2].send(W);
	}
}
