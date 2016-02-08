package com.dranithix.robocon;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.regex.Pattern;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.Disposable;
import com.dranithix.robocon.net.NetworkEvent;
import com.fazecast.jSerialComm.SerialPort;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class RobotSerialManager implements Disposable, Runnable {
	private RobotSignalProcessor filter = new RobotSignalProcessor();

	private SerialPort comPort;
	private BufferedReader in;
	private PrintWriter out;

	private Robocon robocon;

	private boolean running = false;

	private long lastFlush = 0;

	public RobotSerialManager(Robocon robocon) {
		this.robocon = robocon;

		comPort = SerialPort.getCommPort(Robocon.COM_PORT_ADDRESS);
		comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 1000,
				100);
		comPort.setBaudRate(115200);
	}

	public synchronized boolean isRunning() {
		return running;
	}

	public void toggleConnection() {
		if (isRunning()) {
			System.out.println("Robocon: Shutted down server.");
			comPort.closePort();
			in = null;
			out = null;
			running = false;
		} else {
			if (running = comPort.openPort()) {
				System.out.format(
						"Robocon: Server started on %s."
								+ System.lineSeparator(),
						Robocon.COM_PORT_ADDRESS);

				in = new BufferedReader(new InputStreamReader(
						comPort.getInputStream()));
				out = new PrintWriter(comPort.getOutputStream(), false);
			} else {
				System.out.println("Robocon: Unable to start server on "
						+ Robocon.COM_PORT_ADDRESS + ".");
				comPort.closePort();
			}
		}
	}

	@Override
	public void run() {
		try {
			while (true) {
				if (isRunning() && in != null) {
					final String line = in.readLine();
					if (line == null || line.isEmpty())
						continue;
					String[] contents = line.split(Pattern.quote("|"));
					switch (contents[0]) {
					case "STATE":
						Vector2 currentPos = new Vector2(
								Integer.decode(contents[1]),
								Integer.decode(contents[2]));
						int currentBearing = Integer.decode(contents[3]);

						robocon.updateRobotPosition(currentPos, currentBearing);
						break;
					default:
						System.out.println(line);
						break;
					}
				}
			}
		} catch (Exception ex) {
			if (ex.getMessage() != null
					&& ex.getMessage().contains(
							"Underlying input stream returned zero bytes")) {
				System.out.println("Robocon: Lost connection with the robot.");
				toggleConnection();
			} else {
				ex.printStackTrace();
			}
		}
	}

	public void sendEvent(final NetworkEvent packet) {
		if (out != null) {
			synchronized (out) {
				if (isRunning()) {
					out.write(packet.getRawPacket());
					out.flush();
				}
			}
		}
	}

	@Override
	public void dispose() {
		if (comPort != null)
			comPort.closePort();
	}
}