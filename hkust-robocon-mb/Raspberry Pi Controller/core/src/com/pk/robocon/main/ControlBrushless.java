package com.pk.robocon.main;

import com.dranithix.robocon.RobotSerialManager;
import com.dranithix.robocon.net.events.ServoControlEvent;
import com.dranithix.robocon.net.events.ServoControlEvent.ServoType;

public class ControlBrushless {

	private final static int BRUSHLESS_MIN = 590;
	private final static int BRUSHLESS_MAX = 1050;

	public ControlBrushless() {

	}

	/**
	 * magnitude of brushless scaled 0->100
	 */
	public static void executeBrushless(int M1, int M2) {
		sendBrushless(M1 * (BRUSHLESS_MAX - BRUSHLESS_MIN) / 100
				+ BRUSHLESS_MIN, M2 * (BRUSHLESS_MAX - BRUSHLESS_MIN) / 100
				+ BRUSHLESS_MIN);
	}

	private static void sendBrushless(int W1, int W2) {
		RobotSerialManager.getInstance().sendEvent(
				new ServoControlEvent(ServoType.SERVO_1, W1));
		RobotSerialManager.getInstance().sendEvent(
				new ServoControlEvent(ServoType.SERVO_2, W2));
	}

}
