package com.dranithix.robocon;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.Array;
import com.badlogic.gdx.utils.Timer.Task;
import com.dranithix.robocon.net.events.MotorControlEvent;

/**
 * 
 * @author Christopher PK & Kenta Iwasaki
 *
 */
public class RobotControlSystem extends Task {

	private final int MAX_VELOCITY = 140;
	private final int STOP_DISTANCE = 2000;

	private Array<Integer> motorValues = new Array<Integer>();
	private Array<Integer[]> targetQueue = new Array<Integer[]>();

	private Vector2 startPos = new Vector2();
	private Vector2 lastPos = new Vector2();

	private Vector2 currentPos;
	private float currentBearing;

	private Vector2 targetPos = new Vector2();
	private float targetBearing;

	private int distanceThreshold;
	private int bearingThreshold;
	private int robotVelocity;

	private float lastBearing = 0;
	private float positionError = 1;

	private RobotSerialManager serial;

	public RobotControlSystem(RobotSerialManager serial) {
		this.serial = serial;
	}

	/**
	 * ADDS A TARGET
	 * <p>
	 * 
	 * @param x
	 *            target x-coordinate
	 * @param y
	 *            target y-coordinate
	 * @param bearing
	 *            target orientation
	 * @param maxDistanceError
	 *            max distance error
	 * @param maxBearingError
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public void addQueue(Vector2 targetPos, int bearing, int maxDistanceError,
			int maxBearingError, int velocity) {
		Integer[] target = new Integer[6];
		target[0] = (int) targetPos.x;
		target[1] = (int) targetPos.y;
		target[2] = bearing;
		target[3] = maxDistanceError;
		target[4] = maxBearingError;
		target[5] = velocity;
		targetQueue.add(target);
	}

	/**
	 * RESETS QUEUE
	 * <p>
	 */
	public void clearQueue() {
		while (targetQueue.size != 0) {
			targetQueue.removeIndex(0);
		}
	}

	/**
	 * LOADS NEXT TARGET FROM QUEUE
	 * <p>
	 * 
	 * @return true on success
	 */
	public boolean dequeueNextTarget() {
		if (targetQueue.size != 0) {
			Integer[] nextTarget = targetQueue.removeIndex(0);
			int x = nextTarget[0];
			int y = nextTarget[1];
			int bearing = nextTarget[2];
			int maxDistanceError = nextTarget[3];
			int maxBearingError = nextTarget[4];
			int velocity = nextTarget[5];

			setTarget(new Vector2(x, y), bearing, maxDistanceError,
					maxBearingError, velocity);
			return true;
		} else {
			return false;
		}
	}

	/**
	 * SETS CURRENT TARGET
	 * <p>
	 * 
	 * @param x
	 *            target x-coordinate
	 * @param y
	 *            target y-coordinate
	 * @param bearing
	 *            target orientation
	 * @param maxDistanceError
	 *            max distance error
	 * @param maxBearingError
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public void setTarget(Vector2 targetPos, int bearing, int maxDistanceError,
			int maxBearingError, int velocity) {
		positionError = 1;

		this.targetPos = targetPos.cpy();
		this.startPos = currentPos.cpy();

		targetBearing = bearing;
		distanceThreshold = maxDistanceError;
		bearingThreshold = maxBearingError;
		robotVelocity = velocity;
	}

	/**
	 * MOVES ROBOT TO TARGET WITH PID
	 * <p>
	 */
	public void moveStraightToTarget() {
		float velocity = 0;
		float robotBearing = 90
				- MathUtils.atan2(targetPos.y - currentPos.y, targetPos.x
						- currentPos.x) * MathUtils.radiansToDegrees;

		float angularVelocity = 0;
		float targetDistance = currentPos.dst(targetPos);
		System.out.println("dist=" + targetDistance);
		if (targetDistance > distanceThreshold
				|| Math.abs(angleDifference(currentBearing, targetBearing)) > bearingThreshold) {
			if (robotVelocity != 0) {
				velocity = robotVelocity;
			} else {
				velocity = MathUtils.clamp(
						targetDistance * 100 / STOP_DISTANCE, -1000, 1000);
			}
			angularVelocity = angleDifference(currentBearing, targetBearing) * 100 / 180;
			if (targetDistance > STOP_DISTANCE) {
				float px = targetPos.x - startPos.x;
				float py = targetPos.y - startPos.y;
				float dab = px * px + py * py;
				float u = ((currentPos.x - startPos.x) * px + (currentPos.y - startPos.y)
						* py)
						/ dab;
				Vector2 distPos = new Vector2(startPos.x + u * px, startPos.y
						+ u * py);
				float dir1 = calculateResultantBearing(currentPos.dst(distPos),
						calculateBearing(startPos, distPos), targetDistance,
						robotBearing);
				robotBearing = (dir1 - currentBearing) % 360;
			}
		}
		if (velocity * positionError > 100) {
			positionError = 100 / velocity;
		}
		if (angularVelocity * positionError > 100) {
			positionError = 100 / angularVelocity;
		}
		velocity = velocity * positionError;
		angularVelocity = angularVelocity * positionError;
		System.out.println("Target: " + targetPos.x + ", " + targetPos.y);
		System.out.println("M=" + velocity);
		System.out.println("A=" + robotBearing);
		System.out.println("W=" + angularVelocity);
		moveRobot(velocity, robotBearing, angularVelocity);
	}

	/**
	 * UPDATES ERR SCALE, TO BE CALLED AFTER EVERY MOVEMENT
	 * <p>
	 */
	public void fixPositionError() {
		if (currentPos.angle(targetPos) >= lastPos.angle(targetPos)
				|| Math.abs(angleDifference(currentBearing, targetBearing)) >= Math
						.abs(angleDifference(lastBearing, targetBearing))) {
			positionError = positionError + 0.03f;
		} else {
			positionError = positionError * 0.8f + 0.2f;
		}
		lastPos.set(currentPos);
		lastBearing = currentBearing;
	}

	/**
	 * SETS ROBOT IN MOTION
	 * <p>
	 * 
	 * @param velocity
	 *            magnitude scaled 0->100
	 * @param robotBearing
	 *            local bearing of motion in degrees
	 * @param angularVelocity
	 *            angular speed scaled -100 -> 100
	 */
	public void moveRobot(float velocity, float robotBearing,
			float angularVelocity) {
		float xComponent = velocity * MathUtils.sinDeg(robotBearing)
				* MAX_VELOCITY / 100;
		float yComponent = velocity * MathUtils.cosDeg(robotBearing)
				* MAX_VELOCITY / 100;
		float motor1 = (-angularVelocity - xComponent * 2) / 3;
		float motor2 = (-angularVelocity * 0.577f + xComponent * 0.577f - yComponent) / 1.73f;
		float motor3 = -angularVelocity - motor1 - motor2;
		motorValues.set(0, (int) motor1);
		motorValues.set(1, (int) motor2);
		motorValues.set(2, (int) motor3);

		serial.sendEvent(new MotorControlEvent(motorValues));
	}

	/**
	 * CALCULATE DIFFERENCE OF TWO ANGLES
	 * <p>
	 * 
	 * @param currentBearing
	 *            current orientation
	 * @param targetBearing
	 *            target orientation
	 * @return angle difference scaled -180->180
	 */
	private float angleDifference(float currentBearing, float targetBearing) {
		float diff = targetBearing - currentBearing;
		if (diff < -180) {
			diff = diff + 360;
		}
		if (diff > 180) {
			diff = diff - 360;
		}
		return diff;
	}

	/**
	 * CALCULATES BEARING OF POINT FROM POINT_0
	 * <p>
	 * 
	 * @param x0
	 *            x-coordinate of POINT_0
	 * @param y0
	 *            y-coordinate of POINT_0
	 * @param x
	 *            x-coordinate of POINT
	 * @param y
	 *            y-coordinate of POINT
	 * @return bearing in degrees scaled 0->360
	 */
	private float calculateBearing(Vector2 startPos, Vector2 currentPos) {
		float b = 90 - MathUtils.atan2(currentPos.y - startPos.y, currentPos.x
				- startPos.x);
		if (b < 0) {
			b = b + 360;
		}
		return b;
	}

	/**
	 * CALCULATES RESULTING BEARING OF VECTOR ADDITION
	 * 
	 * @param f
	 *            magnitude of VECTOR_1
	 * @param g
	 *            bearing of VECTOR_1
	 * @param m2
	 *            magnitude of VECTOR_2
	 * @param b2
	 *            bearing of VECTOR_2
	 * @return
	 */
	private float calculateResultantBearing(float f, float g, float m2, float b2) {
		return calculateBearing(
				new Vector2(0, 0),
				new Vector2(
						f * MathUtils.sinDeg(g) + m2 * MathUtils.sinDeg(b2), f
								* MathUtils.cosDeg(g) / 10000 + m2
								* MathUtils.cosDeg(b2)));
	}

	@Override
	public void run() {
		if (serial.isRunning()) {

			// Check if current position had been received from the main-board
			// beforehand.
			while (currentPos != null && dequeueNextTarget()) {
				moveStraightToTarget();
			}
		}
	}

	public void updateRobotPosition(Vector2 currentPos, int bearing) {
		this.currentPos = currentPos.cpy();
		this.currentBearing = bearing;
	}
}
