package com.dranithix.robocon.systems;

import java.util.ArrayList;

import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.Array;
import com.badlogic.gdx.utils.Timer.Task;
import com.dranithix.robocon.RobotSerialManager;
import com.dranithix.robocon.RobotTarget;
import com.dranithix.robocon.net.events.MotorControlEvent;

/**
 * 
 * @author Christopher PK & Kenta Iwasaki
 *
 */
public class RobotControlSystem extends Task {

	public static final int MAX_VELOCITY = 140;
	public static final int STOP_DISTANCE = 2000;

	private Array<Integer> motorValues = new Array<Integer>();
	private Array<RobotTarget> targetQueue = new Array<RobotTarget>();

	private Vector2 startPos = new Vector2();
	private Vector2 lastPos = new Vector2();

	private Vector2 currentPos;
	private float currentBearing;

	private Vector2 targetPos = null;
	private float targetBearing;

	private int distanceThreshold;
	private int bearingThreshold;
	private int robotVelocity;

	private float lastBearing = 0;
	private float positionError = 1;

	private RobotSerialManager serial;

	public RobotControlSystem(RobotSerialManager serial) {
		this.serial = serial;

		for (int i = 0; i < 6; i++)
			motorValues.add(0);
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
	public void addQueue(Vector2 targetPos, int bearing, int maxDistanceError, int maxBearingError, int velocity) {
		RobotTarget target = new RobotTarget(targetPos, bearing, maxDistanceError, maxBearingError, velocity);
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
			RobotTarget nextTarget = targetQueue.removeIndex(0);
			setTarget(nextTarget);
			return true;
		} else {
			targetPos = null;
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
	public void setTarget(RobotTarget target) {
		positionError = 1;

		this.targetPos = target.getTargetPos().cpy();
		this.startPos = currentPos.cpy();

		targetBearing = target.getTargetBearing();
		distanceThreshold = target.getMaxDistanceError();
		bearingThreshold = target.getMaxBearingError();
		robotVelocity = target.getVelocity();
	}

	/**
	 * MOVES ROBOT TO TARGET WITH PID
	 * <p>
	 */
	public void moveStraightToTarget() {
		float velocity = 0;
		float robotBearing = 90
				- MathUtils.atan2(targetPos.y - currentPos.y, targetPos.x - currentPos.x) * MathUtils.radiansToDegrees;

		float angularVelocity = 0;
		float targetDistance = currentPos.dst(targetPos);
		// System.out.println("dist=" + targetDistance);
		if (targetDistance > distanceThreshold
				|| Math.abs(angleDifference(currentBearing, targetBearing)) > bearingThreshold) {
			if (robotVelocity != 0) {
				velocity = robotVelocity;
			} else {
				velocity = MathUtils.clamp(targetDistance * 100 / STOP_DISTANCE, -1000, 1000);
			}
			angularVelocity = angleDifference(currentBearing, targetBearing) * 100 / 180;
			if (targetDistance > STOP_DISTANCE) {
				float px = targetPos.x - startPos.x;
				float py = targetPos.y - startPos.y;
				float dab = px * px + py * py;
				float u = ((currentPos.x - startPos.x) * px + (currentPos.y - startPos.y) * py) / dab;
				Vector2 distPos = new Vector2(startPos.x + u * px, startPos.y + u * py);
				float dir1 = calculateResultantBearing(currentPos.dst(distPos), calculateBearing(startPos, distPos),
						targetDistance, robotBearing);
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
		// System.out.println("Target: " + targetPos.x + ", " + targetPos.y);
		// System.out.println("M=" + velocity);
		// System.out.println("A=" + robotBearing);
		// System.out.println("W=" + angularVelocity);
		moveRobot(velocity, robotBearing, angularVelocity);
	}

	/**
	 * 
	 * @param control
	 */
	public void _curve(ArrayList<RobotTarget> control) {
		for (double t = 0; t <= 1; t = t + 0.2) {
			float x = 0;
			float y = 0;
			for (int i = 0; i != control.size(); i++) {
				double c = binomial(control.size() - 1, i) * Math.pow(1 - t, control.size() - i - 1) * Math.pow(t, i);
				x = (float) (x + c * control.get(i).targetPos.x);
				y = (float) (y + c * control.get(i).targetPos.y);
			}
			this.addQueue(new Vector2(x, y), 0, 50, 360, 20);
			// System.out.println("added x=" + x + " y=" + y);
		}
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
	public void moveRobot(float velocity, float robotBearing, float angularVelocity) {
		float xComponent = velocity * MathUtils.sinDeg(robotBearing) * MAX_VELOCITY / 100;
		float yComponent = velocity * MathUtils.cosDeg(robotBearing) * MAX_VELOCITY / 100;
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
		float b = 90 - MathUtils.atan2(currentPos.y - startPos.y, currentPos.x - startPos.x);
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
		return calculateBearing(new Vector2(0, 0), new Vector2(f * MathUtils.sinDeg(g) + m2 * MathUtils.sinDeg(b2),
				f * MathUtils.cosDeg(g) / 10000 + m2 * MathUtils.cosDeg(b2)));
	}

	@Override
	public void run() {
		if (serial.isRunning()) {
			// Check if current position had been received from the main-board
			// beforehand.
			while (currentPos != null && targetPos != null) {
				moveStraightToTarget();
			}
		}
	}

	public void updateRobotPosition(Vector2 currentPos, int bearing) {
		currentPos.y *= -1;
		// System.out.println(currentPos + " " + (bearing / 10));
		this.currentPos = currentPos.cpy();
		this.currentBearing = bearing / 10;
	}

	private int binomial(int n, int k) {
		if (k > n - k)
			k = n - k;

		int b = 1;
		for (int i = 1, m = n; i <= k; i++, m--)
			b = b * m / i;
		return b;
	}
}