package com.dranithix.robocon;

import java.util.Scanner;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.utils.Array;

public class RobotPositionSystem {

	private static final int MAXVEL = 140;
	private static final int STOP_DISTANCE = 2000;

	private static Array<Integer[]> queue = new Array<Integer[]>();

	private static int target_x;
	private static int target_y;
	private static int target_a;
	private static int d_e;
	private static int a_e;
	private static int vel;

	private static int lastX;
	private static int lastY;
	private static int lastAngle;
	private static float err;

	/**
	 * RESETS DEFAULTS
	 * <p>
	 */
	public static void _init() {
		_clearQueue();
		lastX = 0;
		lastY = 0;
		lastAngle = 0;
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
	 * @param angle
	 *            target orientation
	 * @param d_error
	 *            max distance error
	 * @param a_error
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public static void _addQueue(int x, int y, int angle, int d_error, int a_error, int velocity) {
		Integer[] target = new Integer[6];
		target[0] = x;
		target[1] = y;
		target[2] = angle;
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
	 */
	public void _nextTarget() {
		if (queue.size != 0) {
			int x = queue.get(0)[0];
			int y = queue.get(0)[1];
			int angle = queue.get(0)[2];
			int d_error = queue.get(0)[3];
			int a_error = queue.get(0)[4];
			int velocity = queue.get(0)[5];
			_setTarget(x, y, angle, d_error, a_error, velocity);
			queue.removeIndex(0);
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
	 * @param angle
	 *            target orientation
	 * @param d_error
	 *            max distance error
	 * @param a_error
	 *            max orientation error in degrees
	 * @param velocity
	 *            velocity throughout course, 0 for auto
	 */
	public static void _setTarget(int x, int y, int angle, int d_error, int a_error, int velocity) {
		target_x = x;
		target_y = y;
		target_a = angle;
		d_e = d_error;
		a_e = a_error;
		vel = velocity;
	}

	/**
	 * MOVES ROBOT TO TARGET
	 * <p>
	 */
	public static void _straight() {
		int M = 0;
		int A = _angleDiff(_getAngle(), target_a);
		int W = 0;
		int dist = _dist(_getX(), _getY(), target_x, target_y);
		if (dist > d_e || Math.abs(_angleDiff(_getAngle(), target_a)) > a_e) {
			if (vel != 0) {
				M = vel;
			} else {
				M = dist / STOP_DISTANCE * 100;
				if (M > 100) {
					M = 100;
				}
			}
			W = _angleDiff(_getAngle(), target_a) * 100 / 180;
		}
		_move(M, A, W);
	}

	/**
	 * SETS ROBOT IN MOTION
	 * <p>
	 * 
	 * @param M
	 *            magnitude scaled 0->100
	 * @param A
	 *            local angle of motion in degrees scaled 0->360
	 * @param W
	 *            angular speed scaled -100->100
	 */
	public static void _move(int M, int A, int W) {
		float X = M * MathUtils.cosDeg(A) * MAXVEL / 100;
		float Y = M * MathUtils.sinDeg(A) * MAXVEL / 100;
		float _M1 = (-W - X * 2) / 3;
		float _M2 = (-W * 0.577f + X * 0.577f - Y) / 1.73f;
		float _M3 = -W - _M1 - _M2;
		int M1 = (int) (_M1);
		int M2 = (int) (_M2);
		int M3 = (int) (_M3);
		// TODO: send packet
		System.out.println(M1 + " " + M2 + " " + M3);
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

	private static int _getAngle() {
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
		Scanner cin = new Scanner(System.in);
		while (true) {
			int m = cin.nextInt();
			int a = cin.nextInt();
			int w = cin.nextInt();
			_move(m, a, w);
		}
	}

}
