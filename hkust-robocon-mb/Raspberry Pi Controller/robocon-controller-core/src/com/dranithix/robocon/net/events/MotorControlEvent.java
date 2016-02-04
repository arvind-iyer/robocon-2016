package com.dranithix.robocon.net.events;

import com.badlogic.gdx.utils.Array;
import com.dranithix.robocon.net.NetworkEvent;
import com.dranithix.robocon.net.NetworkEventOpcode;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class MotorControlEvent extends NetworkEvent {
	private Array<Integer> motorValues = new Array<Integer>();

	public MotorControlEvent(Array<Integer> motorValues) {
		super(NetworkEventOpcode.MOTOR_CONTROL);

		this.motorValues = motorValues;

		initEvent();
	}

	@Override
	protected void initEvent() {
		for (Integer magnitude : motorValues) {
			putInt(magnitude);
		}
	}

}
