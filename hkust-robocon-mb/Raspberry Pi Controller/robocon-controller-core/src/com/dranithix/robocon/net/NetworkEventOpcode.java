package com.dranithix.robocon.net;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public enum NetworkEventOpcode {
	MOTOR_CONTROL(0);
	
	private int opcode;
	
	private NetworkEventOpcode(int opcode) {
		this.opcode = opcode;
	}
	
	public int getOpcode() {
		return opcode;
	}
}
