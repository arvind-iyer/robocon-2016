package com.pk.robocon.robot;

public class Brushless{
	
	private int ID;
	private int servo=0;
	
	public Brushless(int ID) {
		this.setID(ID);
	}
	
	public int getID() {
		return this.ID;
	}

	public void setID(int ID) {
		this.ID = ID;
	}

	public void setServo(int servo) {
		this.servo=servo;
	}
	
	public int getServo() {
		return this.servo;
	}
	
}
