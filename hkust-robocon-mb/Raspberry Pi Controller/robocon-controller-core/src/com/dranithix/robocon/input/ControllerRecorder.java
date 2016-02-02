package com.dranithix.robocon.input;

import java.util.Arrays;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.utils.Array;
import com.badlogic.gdx.utils.Timer;
import com.badlogic.gdx.utils.Timer.Task;
import com.dranithix.robocon.SerialThread;
import com.dranithix.robocon.net.events.MotorControlEvent;

public class ControllerRecorder {
	private FileHandle movesFile;

	public ControllerRecorder(String type) {
		movesFile = Gdx.files.local(type + ".xor");
	}

	public void clearMovesFile() {
		if (movesFile.exists())
			movesFile.delete();
	}

	public void logMove(ControllerEvent event) {
		movesFile.writeString(event.toString() + "\n", true);
	}

	public Array<ControllerEvent> loadMoves() {
		Array<ControllerEvent> events = new Array<ControllerEvent>();
		if (movesFile.exists()) {
			for (String line : movesFile.readString().split("\n")) {
				line = line.trim();
				if (!line.isEmpty()) {
					String[] content = line.split("@");
					System.out.println(Arrays.toString(content));
					events.add(new ControllerEvent(
							Integer.parseInt(content[0]), Integer
									.parseInt(content[1]), Integer
									.parseInt(content[2]), Integer
									.parseInt(content[3]), Integer
									.parseInt(content[4])));
				}
			}
		}
		return events;
	}
}
