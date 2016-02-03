package com.dranithix.robocon;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.Array;

public class RobotPositionSystem {

	private static final int MAX_VELOCITY = 140;
	private static final int STOP_DISTANCE = 2000;

	public static int MOTOR1;
	public static int MOTOR2;
	public static int MOTOR3;
	public static int MOTOR4;
	public static int MOTOR5;
	public static int MOTOR6;

	private static Array<Integer[]> targetQueue = new Array<Integer[]>();

	private static Vector2 startPos = new Vector2();
	private static Vector2 lastPos = new Vector2();
	private static Vector2 targetPos = new Vector2();
	private static Vector2 currentPos = new Vector2();

	private static float targetBearing;
	private static float currentBearing;

	private static int distanceThreshold;
	private static int angleThreshold;
	private static int robotVelocity;

	private static float lastBearing;
	private static float positionError;

	/**
	 * RESETS DEFAULTS
	 * <p>
	 */
	public static void _init() {
		clearQueue();
		MOTOR1 = 0;
		MOTOR2 = 0;
		MOTOR3 = 0;
		MOTOR4 = 0;
		MOTOR5 = 0;
		MOTOR6 = 0;
		lastPos.set(0, 0);
		lastBearing = 0;
		positionError = 1;
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
	 * @param d_error
	 *            max distance error
	 * @param a_error
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public static void addQueue(int x, int y, int bearing, int d_error,
			int a_error, int velocity) {
		Integer[] target = new Integer[6];
		target[0] = x;
		target[1] = y;
		target[2] = bearing;
		target[3] = d_error;
		target[4] = a_error;
		target[5] = velocity;
		targetQueue.add(target);
	}

	/**
	 * RESETS QUEUE
	 * <p>
	 */
	public static void clearQueue() {
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
	public static boolean _nextTarget() {
		if (targetQueue.size != 0) {
			int x = targetQueue.get(0)[0];
			int y = targetQueue.get(0)[1];
			int angle = targetQueue.get(0)[2];
			int d_error = targetQueue.get(0)[3];
			int a_error = targetQueue.get(0)[4];
			int velocity = targetQueue.get(0)[5];
			setTarget(x, y, angle, d_error, a_error, velocity);
			targetQueue.removeIndex(0);
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
	 * @param d_error
	 *            max distance error
	 * @param a_error
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public static void setTarget(int x, int y, int bearing, int d_error,
			int a_error, int velocity) {
		positionError = 1;
		targetPos.set(x, y);
		startPos.set(currentPos);

		targetBearing = bearing;
		distanceThreshold = d_error;
		angleThreshold = a_error;
		robotVelocity = velocity;
	}

	/**
	 * MOVES ROBOT TO TARGET WITH PID
	 * <p>
	 */
	public static void _straight() {
		float velocity = 0;
		float robotBearing = 90
				- MathUtils.atan2(targetPos.y - currentPos.y, targetPos.x
						- currentPos.x) * MathUtils.radiansToDegrees;

		float angularVelocity = 0;
		float targetDistance = currentPos.dst(targetPos);
		System.out.println("dist=" + targetDistance);
		if (targetDistance > distanceThreshold
				|| Math.abs(angleDifference(currentBearing, targetBearing)) > angleThreshold) {
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
	public static void fixPositionError() {
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
	 * @param m
	 *            magnitude scaled 0->100
	 * @param a
	 *            local bearing of motion in degrees
	 * @param w
	 *            angular speed scaled -100 -> 100
	 */
	public static void moveRobot(float m, float a, float w) {
		float X = m * MathUtils.sinDeg(a) * MAX_VELOCITY / 100;
		float Y = m * MathUtils.cosDeg(a) * MAX_VELOCITY / 100;
		float _M1 = (-w - X * 2) / 3;
		float _M2 = (-w * 0.577f + X * 0.577f - Y) / 1.73f;
		float _M3 = -w - _M1 - _M2;
		MOTOR1 = (int) (_M1);
		MOTOR2 = (int) (_M2);
		MOTOR3 = (int) (_M3);
	}

	/**
	 * CALCULATE DIFFERENCE OF TWO ANGLES
	 * <p>
	 * 
	 * @param currentBearing2
	 *            current orientation
	 * @param targetBearing2
	 *            target orientation
	 * @return angle difference scaled -180->180
	 */
	private static float angleDifference(float currentBearing2,
			float targetBearing2) {
		float diff = targetBearing2 - currentBearing2;
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
	private static float calculateBearing(Vector2 startPos, Vector2 currentPos) {
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
	private static float calculateResultantBearing(float f, float g, float m2,
			float b2) {
		return calculateBearing(
				new Vector2(0, 0),
				new Vector2(
						f * MathUtils.sinDeg(g) + m2 * MathUtils.sinDeg(b2), f
								* MathUtils.cosDeg(g) / 10000 + m2
								* MathUtils.cosDeg(b2)));
	}

	/**
	 * DEBUG
	 * <p>
	 * 
	 * @param args
	 */
	public static void main(String[] args) {
		_init();
		addQueue(100, 100, 32, 0, 0, 0);
		// _addQueue(0, 0, 0, 0, 0, 0);
		while (_nextTarget()) {
			_straight();
		}
	}

}
