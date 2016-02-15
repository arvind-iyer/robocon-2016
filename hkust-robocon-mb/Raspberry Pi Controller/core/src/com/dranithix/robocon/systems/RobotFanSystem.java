package com.dranithix.robocon.systems;

import com.badlogic.gdx.utils.Timer.Task;
import com.dranithix.robocon.RobotSerialManager;
import com.dranithix.robocon.net.events.ServoControlEvent;
import com.dranithix.robocon.net.events.ServoControlEvent.ServoType;

/**
 * 
 * @author Christopher PK & Kenta Iwasaki
 *
 */
public class RobotFanSystem extends Task {
	private static final int FAN_PWM_STOP = 500;
	private static final int FAN_PWM_MIN = 595;
	private static final int FAN_PWM_MAX = 1050;

	private RobotSerialManager serial;

	private boolean fanStopped = true;
	private int magnitude = FAN_PWM_MIN;

	private long initStartTime = -1;

	public RobotFanSystem(RobotSerialManager serial) {
		this.serial = serial;
	}

	public int getPwmValue(int magnitude) {
		return (FAN_PWM_MAX - FAN_PWM_MIN) * magnitude / 100 + FAN_PWM_MIN;
	}

	public void fanControl(int magnitude) {
		System.out.println(getPwmValue(magnitude));

		this.magnitude = getPwmValue(magnitude);
	}

	public void toggleFan(boolean start) {
		if (!start) {
			fanStopped = false;
			System.out.println("Brushless has started.");
		} else if (start) {
			fanStopped = true;
			System.out.println("Brushless has stopped.");
		}
	}
	
	public boolean isFanStopped() {
		return fanStopped;
	}

	@Override
	public void run() {
		if (serial != null) {
			serial.sendEvent(new ServoControlEvent(ServoType.SERVO_1, fanStopped ? FAN_PWM_STOP : magnitude));
			serial.sendEvent(new ServoControlEvent(ServoType.SERVO_2, fanStopped ? FAN_PWM_STOP : magnitude));
		}
	}

}
