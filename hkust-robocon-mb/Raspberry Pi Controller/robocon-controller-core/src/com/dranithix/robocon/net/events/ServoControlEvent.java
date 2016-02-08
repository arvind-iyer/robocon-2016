package com.dranithix.robocon.net.events;

import com.dranithix.robocon.net.NetworkEvent;
import com.dranithix.robocon.net.NetworkEventOpcode;

public class ServoControlEvent extends NetworkEvent {
	public enum ServoType {
		SERVO_1, SERVO_2, SERVO_3, SERVO_4;
	}
	
	private ServoType servoId;
	private int magnitude;

	public ServoControlEvent(ServoType servoId, int magnitude) {
		super(NetworkEventOpcode.SERVO_CONTROL);
		
		this.servoId = servoId;
		this.magnitude = magnitude;
		
		initEvent();
	}

	@Override
	protected void initEvent() {
		putInt(servoId.ordinal());
		putInt(magnitude);
	}

}
