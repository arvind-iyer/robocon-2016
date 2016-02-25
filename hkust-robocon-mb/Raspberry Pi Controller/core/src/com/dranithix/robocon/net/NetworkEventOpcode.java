package com.dranithix.robocon.net;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public enum NetworkEventOpcode {
	MOTOR_CONTROL("MOTOR_CONTROL"), SERVO_CONTROL("SERVO_CONTROL"), PNEUMATIC_CONTROL("PNEUMATIC CONTROL");
	
	private String opcode;
	
	private NetworkEventOpcode(String opcode) {
		this.opcode = opcode;
	}
	
	public String getOpcode() {
		return opcode;
	}
}
