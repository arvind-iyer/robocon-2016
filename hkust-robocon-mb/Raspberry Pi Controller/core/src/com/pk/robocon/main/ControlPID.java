package com.pk.robocon.main;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;
import com.pk.robocon.system.Path;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

public class ControlPID extends Control {

	private ArrayList<Path> queue = new ArrayList<Path>();

	public ArrayList<Path> getQueue() {
		return queue;
	}

	/**
	 * Adds a single Target
	 * 
	 * @param t
	 */
	public void addQueue(Target t) {
		queue.add(new Path(t));
	}

	/**
	 * Adds multiple Target to form curve
	 * 
	 * @param control
	 */
	public void addCurve(ArrayList<Target> control) {
		for (double t = 0; t <= 1; t = t + 0.2) {
			float x = 0;
			float y = 0;
			for (int i = 0; i != control.size(); i++) {
				double c = binomial(control.size() - 1, i) * Math.pow(1 - t, control.size() - i - 1) * Math.pow(t, i);
				x = (float) (x + c * control.get(i).getPosition().getX());
				y = (float) (y + c * control.get(i).getPosition().getY());
			}
			addQueue(new Target(new Position(new Vector2((int) (x), (int) (y)), 0), new Threshold(50, 360), 20));
		}
	}

	/**
	 * Updates PID motor values
	 */
	public boolean execute() {
		if (queue.size() > 0) {
			queue.get(0).straight();
			super.setMotorValues(queue.get(0).getM1(), queue.get(0).getM2(), queue.get(0).getM3());
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

	private static int binomial(int n, int k) {
		if (k > n - k) {
			k = n - k;
		}
		int b = 1;
		for (int i = 1, m = n; i <= k; i++, m--)
			b = b * m / i;
		return b;
	}

}