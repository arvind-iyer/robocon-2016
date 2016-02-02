package com.dranithix.robocon;

import java.util.Scanner;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.utils.Array;

public class RobotPositionSystem {

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
	 */
	public static void _init() {
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
	 *            min velocity throughout course
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
	 *            min velocity throughout course
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
	 * SETS ROBOT IN MOTION
	 * <p>
	 * 
	 * @param M
	 *            magnitude scaled 0-100
	 * @param A
	 *            angle of robot in degrees scaled 0-360
	 * @param W
	 *            angular velocity
	 */
	public static void _move(float M, float A, float W) {
		float X = M * MathUtils.cosDeg(A);
		float Y = M * MathUtils.sinDeg(A);
		int _M1 = (int) ((-W - X * 2) / 3);
		int _M2 = (int) ((-W * 0.577f + X * 0.577f - Y) / 1.73f);
		int _M3 = (int) (-W - _M1 - _M2);

		// TODO: send packet
	}

	/**
	 * CALCULATES DISTANCE BETWEEN TWO POINTS
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
	}

}
