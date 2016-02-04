package com.dranithix.robocon;

/**
 * 
 * @author Christopher PK & Kenta Iwasaki
 *
 */
public class RoboconBrushless {

	private static final int BRUSHLESS_MIN = 400;
	private static final int BRUSHLESS_MAX = 1050;

	public static void brushlessControl(int SERVO_ID, int magnitude) {
		int vel = (BRUSHLESS_MAX - BRUSHLESS_MIN) * magnitude / 100 + BRUSHLESS_MIN;
		// TODO: servo_control(SERVO SERVO_ID, pwm vel);
	}

}
