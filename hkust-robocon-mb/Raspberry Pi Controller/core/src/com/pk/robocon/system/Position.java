package com.pk.robocon.system;

import com.badlogic.gdx.math.Vector2;

public class Position {

	private Vector2 pos;
	private int bearing;

	public Position(Vector2 pos, int bearing) {
		this.pos = pos;
		this.bearing = bearing;
	}

	public int getX() {
		return (int) (this.pos.x);
	}

	public int getY() {
		return (int) (this.pos.y);
	}

	public int getBearing() {
		return this.bearing;
	}
}