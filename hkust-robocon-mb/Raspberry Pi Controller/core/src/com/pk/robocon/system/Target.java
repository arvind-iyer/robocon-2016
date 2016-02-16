package com.pk.robocon.system;

public class Target {

	private Position p;
	private Threshold err;
	private int vel;

	public Target(Position p, Threshold err, int vel) {
		this.p = p;
		this.err = err;
		this.vel = vel;
	}
	
	public Position getPosition() {
		return this.p;
	}

	public Threshold getThreshold() {
		return this.err;
	}
	
	public int getVel() {
		return this.vel;
	}

}