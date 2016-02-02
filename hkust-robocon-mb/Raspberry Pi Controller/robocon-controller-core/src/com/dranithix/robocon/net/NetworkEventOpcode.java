package com.dranithix.robocon.net;

public enum NetworkEventOpcode {
	MOTOR_CONTROL(0), TEST(1), GYRO_SHIFT(2);
	
	private int opcode;
	
	private NetworkEventOpcode(int opcode) {
		this.opcode = opcode;
	}
	
	public int getOpcode() {
		return opcode;
	}
}
