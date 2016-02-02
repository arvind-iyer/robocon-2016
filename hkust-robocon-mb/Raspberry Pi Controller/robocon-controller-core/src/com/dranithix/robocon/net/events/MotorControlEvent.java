package com.dranithix.robocon.net.events;

import com.dranithix.robocon.net.NetworkEvent;
import com.dranithix.robocon.net.NetworkEventOpcode;

public class MotorControlEvent extends NetworkEvent {
	private int w1, w2, w3;

	public MotorControlEvent(int w1, int w2, int w3) {
		super(NetworkEventOpcode.MOTOR_CONTROL);
		
		this.w1 = w1;
		this.w2 = w2;
		this.w3 = w3;
	
		initEvent();
	}
	@Override
	protected void initEvent() {
		putInt(w1);
		putInt(w2);
		putInt(w3);
	}

}
