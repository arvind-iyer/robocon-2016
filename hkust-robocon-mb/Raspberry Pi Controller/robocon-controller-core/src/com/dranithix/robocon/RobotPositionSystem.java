package com.dranithix.robocon;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.utils.Array;

public class RobotPositionSystem {

	private static final int MAXVEL = 140;
	private static final int STOP_DISTANCE = 2000;

	public static int MOTOR1;
	public static int MOTOR2;
	public static int MOTOR3;
	public static int MOTOR4;
	public static int MOTOR5;
	public static int MOTOR6;

	private static Array<Integer[]> queue = new Array<Integer[]>();

	private static int target_x;
	private static int target_y;
	private static int target_a;
	private static int distanceThreshold;
	private static int angleThreshold;
	private static int vel;

	private static int lastX;
	private static int lastY;
	private static int lastBearing;
	private static float err;

	/**
	 * RESETS DEFAULTS
	 * <p>
	 */
	public static void _init() {
		_clearQueue();
		MOTOR1 = 0;
		MOTOR2 = 0;
		MOTOR3 = 0;
		MOTOR4 = 0;
		MOTOR5 = 0;
		MOTOR6 = 0;
		lastX = 0;
		lastY = 0;
		lastBearing = 0;
		err = 1;
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
	public static void _addQueue(int x, int y, int bearing, int d_error, int a_error, int velocity) {
		Integer[] target = new Integer[6];
		target[0] = x;
		target[1] = y;
		target[2] = bearing;
		target[3] = d_error;
		target[4] = a_error;
		target[5] = vel;
		queue.add(target);
	}

	/**
	 * RESETS QUEUE
	 * <p>
	 */
	public static void _clearQueue() {
		while (queue.size != 0) {
			queue.removeIndex(0);
		}
	}

	/**
	 * LOADS NEXT TARGET FROM QUEUE
	 * <p>
	 * 
	 * @return true on success
	 */
	public static boolean _nextTarget() {
		if (queue.size != 0) {
			int x = queue.get(0)[0];
			int y = queue.get(0)[1];
			int angle = queue.get(0)[2];
			int d_error = queue.get(0)[3];
			int a_error = queue.get(0)[4];
			int velocity = queue.get(0)[5];
			_setTarget(x, y, angle, d_error, a_error, velocity);
			queue.removeIndex(0);
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
	public static void _setTarget(int x, int y, int bearing, int d_error, int a_error, int velocity) {
		err = 1;
		target_x = x;
		target_y = y;
		target_a = bearing;
		distanceThreshold = d_error;
		angleThreshold = a_error;
		vel = velocity;
	}

	/**
	 * MOVES ROBOT TO TARGET WITH PID
	 * <p>
	 */
	public static void _straight() {
		int M = 0;
		int A = 0;
		int W = 0;
		int dist = _dist(_getX(), _getY(), target_x, target_y);
		System.out.println("dist=" + dist);
		if (dist > distanceThreshold || Math.abs(_angleDiff(_getBearing(), target_a)) > angleThreshold) {
			if (vel != 0) {
				M = vel;
			} else {
				M = dist * 100 / STOP_DISTANCE;
				if (M > 100) {
					M = 100;
				}
			}
			W = _angleDiff(_getBearing(), target_a) * 100 / 180;
		}
		if (dist > STOP_DISTANCE) {
			// TODO: pid
		} else {
			A = (int) (90 - MathUtils.atan2(target_y - _getY(), target_x - _getX()) * MathUtils.radiansToDegrees);
		}
		if (M * err > 100) {
			err = 100 / M;
		}
		if (W * err > 100) {
			err = 100 / W;
		}
		M = (int) (M * err);
		W = (int) (W * err);
		System.out.println("Target: " + target_x + ", " + target_y);
		System.out.println("M=" + M);
		System.out.println("A=" + A);
		System.out.println("W=" + W);
		_move(M, A, W);
	}

	/**
	 * UPDATES ERR SCALE, TO BE CALLED AFTER EVERY MOVEMENT
	 * <p>
	 */
	public static void _errUpdate() {
		if (_dist(_getX(), _getY(), target_x, target_y) >= _dist(lastX, lastY, target_x, target_y)
				|| Math.abs(_angleDiff(_getBearing(), target_a)) >= Math.abs(_angleDiff(lastBearing, target_a))) {
			err = err + 0.03f;
		} else {
			err = err * 0.8f + 0.2f;
		}
		lastX = _getX();
		lastY = _getY();
		lastBearing = _getBearing();
	}

	/**
	 * SETS ROBOT IN MOTION
	 * <p>
	 * 
	 * @param M
	 *            magnitude scaled 0->100
	 * @param A
	 *            local bearing of motion in degrees
	 * @param W
	 *            angular speed scaled -100->100
	 */
	public static void _move(int M, int A, int W) {
		float X = M * MathUtils.sinDeg(A) * MAXVEL / 100;
		float Y = M * MathUtils.cosDeg(A) * MAXVEL / 100;
		float _M1 = (-W - X * 2) / 3;
		float _M2 = (-W * 0.577f + X * 0.577f - Y) / 1.73f;
		float _M3 = -W - _M1 - _M2;
		MOTOR1 = (int) (_M1);
		MOTOR2 = (int) (_M2);
		MOTOR3 = (int) (_M3);
	}

	/**
	 * CALCULATES DISTANCE BETWEEN TWO POINTS
	 * <p>
	 * 
	 * @param x1
	 *            x-coordinate of POINT_1
	 * @param y1
	 *            y-coordinate of POINT_1
	 * @param x2
	 *            x-coordinate of POINT_2
	 * @param y2
	 *            y-coordinate of POINT_2
	 * @return distance of two points
	 */
	private static int _dist(int x1, int y1, int x2, int y2) {
		return (int) (Math.sqrt(Math.pow(y2 - y1, 2) + Math.pow(x2 - x1, 2)));
	}

	/**
	 * CALCULATE DIFFERENCE OF TWO ANGLES
	 * <p>
	 * 
	 * @param angle_o
	 *            current orientation
	 * @param angle_t
	 *            target orientation
	 * @return angle difference scaled -180->180
	 */
	private static int _angleDiff(int angle_o, int angle_t) {
		int diff = angle_t - angle_o;
		if (diff < -180) {
			diff = diff + 360;
		}
		if (diff > 180) {
			diff = diff - 360;
		}
		return diff;
	}

	private static int _getX() {
		// TODO: gyro
		return 0;
	}

	private static int _getY() {
		// TODO: gyro
		return 0;
	}

	private static int _getBearing() {
		// TODO: gyro
		return 0;
	}

	/**
	 * DEBUG
	 * <p>
	 * 
	 * @param args
	 */
	public static void main(String[] args) {
		_init();
		_addQueue(0, 100, 0, 0, 0, 0);
		// _addQueue(0, 0, 0, 0, 0, 0);
		while (_nextTarget()) {
			_straight();
		}
	}

}
