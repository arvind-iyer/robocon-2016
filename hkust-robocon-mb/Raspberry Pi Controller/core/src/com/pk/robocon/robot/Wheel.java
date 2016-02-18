package com.pk.robocon.robot;

public class Wheel {

	private int ID;
	private int velocity = 0;

	public Wheel(int ID) {
		this.setID(ID);
	}
	
	public int getID() {
		return this.ID;
	}

	public void setID(int ID) {
		this.ID = ID;
	}

	public void setVelocity(int vel) {
		this.velocity=vel;
	}
	
	public int getVelocity() {
		return this.velocity;
	}

}
