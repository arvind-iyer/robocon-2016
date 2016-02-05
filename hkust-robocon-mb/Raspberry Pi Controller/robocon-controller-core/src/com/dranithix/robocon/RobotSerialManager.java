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

	public RobotSerialManager(Robocon robocon) {
		this.robocon = robocon;
		
		comPort = SerialPort.getCommPort(Robocon.COM_PORT_ADDRESS);
		comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING,
				1000, 0);
		comPort.setBaudRate(115200);
	}

	public boolean isRunning() {
		return running;
	}

	public boolean toggleConnection() {
		if (running) {
			System.out.println("Robocon: Shutted down server.");
			comPort.closePort();
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

		return running;
	}

	@Override
	public void run() {
		while (true) {
			System.out.println(running);
			if (running) {
				try {
					final String line = in.readLine();
					System.out.println(line);
					if (line.isEmpty())
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
					}
					System.out.println(Arrays.toString(contents));
				} catch (Exception ex) {
					ex.printStackTrace();
					continue;
				}
			}
		}
	}

	public void sendEvent(final NetworkEvent packet) {
		if (out != null) {
			out.write(packet.getRawPacket());
			System.out.print(packet.getRawPacket());
			out.flush();
		}
	}

	@Override
	public void dispose() {
		if (comPort != null)
			comPort.closePort();
	}
}