package com.dranithix.robocon.control.system;

public class Target {

	public Position p;
	public Threshold err;
	public int vel;

	public Position getPosition() {
		return p;
	}
	
	public Threshold getThreshold() {
		return err;
	}
	
	public Target(Position p, Threshold err, int vel) {
		this.p = p;
		this.err = err;
		this.vel = vel;
	}

}
