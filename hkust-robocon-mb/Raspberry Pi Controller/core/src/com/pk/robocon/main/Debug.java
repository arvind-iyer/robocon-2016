package com.pk.robocon.main;

public class Debug {
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

	public static void main(String[] args) {
		for (int i = 0; i <=180; i=i+30)
			for (int n = 0; n <=180; n=n+30)
				System.out.println(angleDiff(i, n));
	}

}
