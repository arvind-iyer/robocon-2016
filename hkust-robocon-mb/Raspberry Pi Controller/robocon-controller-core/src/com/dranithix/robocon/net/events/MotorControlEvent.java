package com.dranithix.robocon.net.events;

import com.dranithix.robocon.net.NetworkEvent;
import com.dranithix.robocon.net.NetworkEventOpcode;

public class MotorControlEvent extends NetworkEvent {
	private int magnitude, direction;

	public MotorControlEvent(int direction, int magnitude) {
		super(NetworkEventOpcode.MOTOR_CONTROL);
		
		setDirection(direction);
		setMagnitude(magnitude);
	
		initEvent();
	}

	public int getMagnitude() {
		return magnitude;
	}

	public void setMagnitude(int magnitude) {
		this.magnitude = magnitude;
	}

	public int getDirection() {
		return direction;
	}

	public void setDirection(int direction) {
		this.direction = direction;
	}

	@Override
	protected void initEvent() {
		putInt(direction);
		putInt(magnitude);
	}

}
