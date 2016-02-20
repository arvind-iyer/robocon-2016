package com.pk.robocon.main;

import com.badlogic.gdx.math.Vector2;
import com.dranithix.robocon.RobotSerialManager;
import com.dranithix.robocon.net.events.MotorControlEvent;
import com.dranithix.robocon.net.events.ServoControlEvent;
import com.dranithix.robocon.net.events.ServoControlEvent.ServoType;
import com.pk.robocon.robot.Brushless;
import com.pk.robocon.robot.Wheel;
import com.pk.robocon.system.Position;

public class Control {
	public static final int MAX_VELOCITY = 140;

	protected Wheel[] motorsPID = { new Wheel(1), new Wheel(2), new Wheel(3) };
	protected Brushless[] motorsBrushless = { new Brushless(1), new Brushless(2) };
	protected Wheel[] motorsClimb = { new Wheel(4), new Wheel(5), new Wheel(6) };

	public static Position currentPos;

	public static void currentPosUpdate(int x, int y, int bearing) {
		currentPos = new Position(new Vector2(x, y), bearing);
	}

	public static Position getCurrentPos() {
		return currentPos;
	}

	protected void setBrushless(int W1, int W2) {
		this.motorsBrushless[0].setServo(W1);
		this.motorsBrushless[1].setServo(W2);
	}

	protected void setClimb(int W) {
		this.motorsClimb[0].setVelocity(W);
		this.motorsClimb[1].setVelocity(W);
		this.motorsClimb[2].setVelocity(W);
	}

	public void setMotorValues(int M1, int M2, int M3) {
		motorsPID[0].setVelocity(M1);
		motorsPID[1].setVelocity(M2);
		motorsPID[2].setVelocity(M3);
	}

	public void setMotorValues(Integer[] motorValues) {
		for (int i = 0; i < 3; i++) {
			motorsPID[i].setVelocity(motorValues[i]);
		}
	}

	/**
	 * Sends motor commands, pid and climbing included
	 */
	public void sendMotorCommands() {
		RobotSerialManager.getInstance()
				.sendEvent(new MotorControlEvent(motorsPID[0].getVelocity(), motorsPID[1].getVelocity(),
						motorsPID[2].getVelocity(), motorsClimb[0].getVelocity(), motorsClimb[1].getVelocity(),
						motorsClimb[2].getVelocity()));
	}

	/**
	 * sends servo commands, brushless motor
	 */
	public void sendBrushlessCommands() {
		RobotSerialManager.getInstance()
				.sendEvent(new ServoControlEvent(ServoType.SERVO_1, motorsBrushless[0].getServo()));
		RobotSerialManager.getInstance()
				.sendEvent(new ServoControlEvent(ServoType.SERVO_2, motorsBrushless[1].getServo()));
	}

}
