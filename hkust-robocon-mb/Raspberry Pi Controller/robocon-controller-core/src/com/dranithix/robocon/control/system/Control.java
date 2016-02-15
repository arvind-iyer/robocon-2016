package com.dranithix.robocon.control.system;

import java.util.ArrayList;

public class Control {

	public ArrayList<PID> queue = new ArrayList<PID>();

	public Control() {

	}

	public void addQueue(Target t) {
		queue.add(new PID(t));
	}

	public void addCurve(ArrayList<Target> control) {
		for (double t = 0; t <= 1; t = t + 0.2) {
			float x = 0;
			float y = 0;
			for (int i = 0; i != control.size(); i++) {
				double c = binomial(control.size() - 1, i) * Math.pow(1 - t, control.size() - i - 1) * Math.pow(t, i);
				x = (float) (x + c * control.get(i).getPosition().getX());
				y = (float) (y + c * control.get(i).getPosition().getY());
			}
			this.addQueue(new Target(new Position((int) (x), (int) (y), 0), new Threshold(50, 360), 20));
		}
	}

	public void _execute() {
		queue.get(0).straight();
		if (_dist(new Position(),
				queue.get(0).getTarget().getPosition()) > queue.get(0).getTarget().getThreshold().dist_err) {
			queue.remove(0);
		}
	}

	private int _dist(Position o, Position t) {
		return (int) (Math.sqrt(Math.pow(t.x - o.x, 2) + Math.pow(o.x - o.y, 2)));
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
