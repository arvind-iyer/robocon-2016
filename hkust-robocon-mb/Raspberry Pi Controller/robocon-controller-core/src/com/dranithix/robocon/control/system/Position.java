package com.dranithix.robocon.control.system;

public class Position {

	public int x;
	public int y;
	public int bearing;

	public Position() {
		this.x = gyro_x();
		this.y = gyro_y();
		this.bearing = gyro_bearing();
	}

	public Position(int x, int y, int bearing) {
		this.x = x;
		this.y = y;
		this.bearing = bearing;
	}
	
	public int getX() {
		return this.x;
	}
	
	public int getY() {
		return this.y;
	}

	private int gyro_x() {
		//TODO: gyro
		return 0;
	}
	
	private int gyro_y() {
		//TODO: gyro
		return 0;
	}
	
	private int gyro_bearing() {
		//TODO: gyro
		return 0;
	}
	
}
