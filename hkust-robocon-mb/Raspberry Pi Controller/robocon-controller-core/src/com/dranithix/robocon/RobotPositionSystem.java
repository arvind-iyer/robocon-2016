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
		//TODO: send packet
	}

	private static int _getX() {

	}

	private static int _getY() {

	}

	private static int _getAngle() {

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
