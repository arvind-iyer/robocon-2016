package com.dranithix.robocon.desktop;

import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;
import com.dranithix.robocon.Robocon;

public class DesktopLauncher {
	public static void main(String[] arg) {
		LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
		config.title = "HKUST Robocon 2016 :: Controller";
		config.width = 800;
		config.height = 600;
		config.foregroundFPS = 0;
		config.fullscreen = false;
		new LwjglApplication(new Robocon(), config);
	}
}
