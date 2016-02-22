package com.pk.robocon.main;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;
import com.dranithix.robocon.ui.QueueChangeListener;
import com.pk.robocon.system.Path;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

public class ControlPID extends Control {

	private static final float errIncrement = 0.07f;

	private int lockedBearing = 0;
	private boolean orientationLock = false;;
	private float errAngle = 1;
	private int lastAngleDiff = 0;

	private ArrayList<Path> queue = new ArrayList<Path>();

	public ArrayList<Path> getQueue() {
		return queue;
	}

	/**
	 * Adds Target to top of queue
	 * 
	 * @param t
	 *            Target Object
	 */
	public void overrideQueue(Target t) {
		if (queue.size() != 0) {
			queue.get(0).setErrDist(1);
			queue.get(0).setErrAngle(1);
		}
		queue.add(0, new Path(t));
	}

	/**
	 * Adds a single Target
	 * 
	 * @param t
	 *            Target object
	 */
	public void addQueue(Target t) {
		queue.add(new Path(t));
	}

	/**
	 * Adds multiple Vector2 to form curve
	 * 
	 * @param control
	 *            Control points in bezier curve
	 */
	public void addCurve(ArrayList<Vector2> control, int finalBearing) {
		control.add(0, Control.getCurrentPos().getVector());
		for (double t = 0; t != 1; t = t + 0.2) {
			float x = 0;
			float y = 0;
			for (int i = 0; i != control.size(); i++) {
				double c = binomial(control.size() - 1, i) * Math.pow(1 - t, control.size() - i - 1) * Math.pow(t, i);
				x = (float) (x + c * control.get(i).x);
				y = (float) (y + c * control.get(i).y);
			}
			addQueue(new Target(new Position(new Vector2((int) (x), (int) (y)), finalBearing), new Threshold(50, 360),
					20));
		}
		addQueue(new Target(new Position(control.get(control.size() - 1), finalBearing), new Threshold(1, 1), 0));
	}

	public void orientationUnlock() {
		this.orientationLock = false;
	}

	public void toggleOrientationLock() {
		this.lockedBearing = Control.getCurrentPos().getBearing();
		this.orientationLock = !this.orientationLock;
	}

	public boolean getOrientationLockState() {
		return this.orientationLock;
	}

	public int getOrientationLockW() {
		this.updateErrAngle();
		return (int) (angleDiff(Control.getCurrentPos().getBearing(), this.lockedBearing) * this.errAngle / 1.8f); // *100/180
	}

	private void updateErrAngle() {
		if (!this.checkErrAngle()) {
			errAngle = errAngle + errIncrement;
		} else {
			errAngle = errAngle * 0.8f + 0.2f;
		}
		this.lastAngleDiff = Math.abs(angleDiff(Control.getCurrentPos().getBearing(), this.lockedBearing));
	}

	private boolean checkErrAngle() {
		if (Math.abs(angleDiff(Control.getCurrentPos().getBearing(), this.lockedBearing)) >= this.lastAngleDiff) {
			return false;
		}
		return true;
	}

	/**
	 * Updates PID motor values
	 */
	public boolean execute(QueueChangeListener queueListener) {
		if (queue.size() > 0) {
			// queue.get(0).straight();
			queue.get(0).calculateLinearPath();
			int M = (int) queue.get(0).getM();
			int bearing = queue.get(0).getBearing();
			int W = (int) queue.get(0).getW();
			System.out.println("GYRO: " + Control.getCurrentPos().getX() + "," + Control.getCurrentPos().getY() + " "
					+ Control.getCurrentPos().getBearing());
			System.out.println("Target: " + queue.get(0).getTarget().getPosition().getX() + ","
					+ queue.get(0).getTarget().getPosition().getY() + " "
					+ queue.get(0).getTarget().getPosition().getBearing());
			System.out.println("move(" + M + "[" + queue.get(0).getErrDist() + "], " + bearing + ", " + W + "["
					+ queue.get(0).getErrAngle() + "])");
			this.calculateMotorValues(M, bearing, W);
			// this.sendMotorCommand;
			// super.setMotorValues(queue.get(0).getM1(), queue.get(0).getM2(),
			// queue.get(0).getM3());
			if (queue.get(0).completed()) {
				System.out.println("LOADING NEXT TARGET");
				queue.remove(0);
				queueListener.setQueueRemoveCheck(true);

			}
		} else {
			System.out.println("NO MORE TARGETS");
		}
		return queue.size() > 0;
	}

	/**
	 * Removes the first Target from queue
	 */
	public void nextPID() {
		if (!queue.isEmpty()) {
			queue.remove(0);
		}
	}

	/**
	 * Updates the motor values
	 * <p>
	 * In manual control, call move(), param magnitude, bearing,
	 * orientationPID()
	 * 
	 * @param M
	 *            magnitude scaled 0->100, given by this.translationPID_M()
	 * @param bearing
	 *            bearing scaled -180->180, given by
	 *            this.translationPID_bearing()
	 * @param W
	 *            angular velocity scaled -100->100, given by
	 *            this.orientationPID()
	 */
	public void calculateMotorValues(int M, int bearing, int W) {
		double xComponent = (double) (M * Math.sin(bearing * Math.PI / 180) * MAX_VELOCITY / 100);
		double yComponent = (double) (M * Math.cos(bearing * Math.PI / 180) * MAX_VELOCITY / 100);
		int M1 = (int) ((-W - xComponent * 2) / 3);
		int M2 = (int) ((-W * 0.577f + xComponent * 0.577f - yComponent) / 1.73f);
		int M3 = -W - M1 - M2;
		super.setMotorValues(M1, M2, M3);
	}

	private static int binomial(int n, int k) {
		if (k > n - k) {
			k = n - k;
		}
		int b = 1;
		for (int i = 1, m = n; i <= k; i++, m--)
			b = b * m / i;
		return b;
	}

	private static int angleDiff(int o, int t) {
		int diff = t - o;
		if (diff < -180) {
			diff = diff + 360;
		}
		if (diff > 180) {
			diff = diff - 360;
		}
		return diff;
	}

}