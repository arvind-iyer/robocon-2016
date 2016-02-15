package com.dranithix.robocon;

import com.badlogic.gdx.math.Vector2;

public class RobotTarget {
	public Vector2 targetPos;
	public int targetBearing;
	public int maxDistanceError;
	public int maxBearingError;
	public int velocity;

	public RobotTarget(Vector2 targetPos, int targetBearing,
			int maxDistanceError, int maxBearingError, int velocity) {
		this.targetPos = targetPos;
		this.targetBearing = targetBearing;
		this.maxDistanceError = maxDistanceError;
		this.maxBearingError = maxBearingError;
		this.velocity = velocity;
	}

	public Vector2 getTargetPos() {
		return targetPos;
	}

	public void setTargetPos(Vector2 targetPos) {
		this.targetPos = targetPos;
	}

	public int getTargetBearing() {
		return targetBearing;
	}

	public void setTargetBearing(int targetBearing) {
		this.targetBearing = targetBearing;
	}

	public int getMaxDistanceError() {
		return maxDistanceError;
	}

	public void setMaxDistanceError(int maxDistanceError) {
		this.maxDistanceError = maxDistanceError;
	}

	public int getMaxBearingError() {
		return maxBearingError;
	}

	public void setMaxBearingError(int maxBearingError) {
		this.maxBearingError = maxBearingError;
	}

	public int getVelocity() {
		return velocity;
	}

	public void setVelocity(int velocity) {
		this.velocity = velocity;
	}

}