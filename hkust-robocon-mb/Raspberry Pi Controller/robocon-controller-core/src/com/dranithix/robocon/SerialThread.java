package com.dranithix.robocon;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.regex.Pattern;

import com.badlogic.gdx.utils.Disposable;
import com.dranithix.robocon.net.NetworkEvent;
import com.fazecast.jSerialComm.SerialPort;

public class SerialThread implements Disposable, Runnable {
	private GraphFilter filter = new GraphFilter();

	private SerialPort comPort;
	private BufferedReader in;
	private PrintWriter out;

	public boolean isRunning() {
		return comPort.isOpen();
	}

	public boolean toggleConnection() {
		boolean running;
		if (comPort != null && comPort.isOpen()) {
			comPort.closePort();
			running = false;
		} else {
			comPort = SerialPort.getCommPort(Robocon.COM_PORT_ADDRESS);
			comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 1000, 0);
			comPort.setBaudRate(115200);

			if (running = comPort.openPort()) {
				System.out.format("Robocon: Server started on %s." + System.lineSeparator(), Robocon.COM_PORT_ADDRESS);

				in = new BufferedReader(new InputStreamReader(comPort.getInputStream()));
				out = new PrintWriter(comPort.getOutputStream(), false);
			} else {
				System.out.println("Robocon: Unable to start server on " + Robocon.COM_PORT_ADDRESS + ".");
			}
		}

		return running;
	}

	public void run() {
		while (true) {
			if (comPort != null && comPort.isOpen()) {
				try {
					final String line = in.readLine();
					if (line.isEmpty())
						continue;
					String[] contents = line.split(Pattern.quote("|"));
					switch (contents[0]) {

					}
					System.out.println(Arrays.toString(contents));
				} catch (IOException ex) {
					continue;
				} catch (NumberFormatException ex) {
					continue;
				} catch (Exception ex) {
					ex.printStackTrace();
				}
			}
		}
	}

	public void sendEvent(final NetworkEvent packet) {
		if (out != null) {
			out.write(packet.getRawPacket());
			out.flush();
		}
	}

	@Override
	public void dispose() {
		if (comPort != null)
			comPort.closePort();
	}
}