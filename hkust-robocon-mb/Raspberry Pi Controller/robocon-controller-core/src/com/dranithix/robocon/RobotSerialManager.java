package com.dranithix.robocon;

import java.util.regex.Pattern;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.Disposable;
import com.dranithix.robocon.net.NetworkEvent;
import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortPacketListener;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class RobotSerialManager implements Disposable, Runnable,
		SerialPortPacketListener {
	private RobotSignalProcessor filter = new RobotSignalProcessor();

	private SerialPort comPort;

	private Robocon robocon;

	private boolean running = false;

	public RobotSerialManager(Robocon robocon) {
		this.robocon = robocon;

		comPort = SerialPort.getCommPort(Robocon.COM_PORT_ADDRESS);
		comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 100,
				100);
		comPort.setBaudRate(38400);
	}

	public synchronized boolean isRunning() {
		return running;
	}

	public void toggleConnection() {
		if (isRunning()) {
			System.out.println("Robocon: Shutted down server.");
			comPort.closePort();
			running = false;
		} else {
			if (running = comPort.openPort()) {
				System.out.format(
						"Robocon: Server started on %s."
								+ System.lineSeparator(),
						Robocon.COM_PORT_ADDRESS);

				comPort.addDataListener(this);
			} else {
				System.out.println("Robocon: Unable to start server on "
						+ Robocon.COM_PORT_ADDRESS + ".");
				comPort.closePort();
			}
		}
	}

	@Override
	public void run() {
		while (true) {
			if (!comPort.isOpen()) {
				running = false;
			}
		}
	}

	public void sendEvent(final NetworkEvent packet) {
//		System.out.print(packet.getRawPacket());
		if (isRunning()) {
			int sendStatus = comPort.writeBytes(packet.getRawPacket()
					.getBytes(), packet.getRawPacket().length());
			if (sendStatus == -1) {
				System.out.println("Error occurred sending packet "
						+ packet.getRawPacket().replace("\n", "")
						+ ". Resending...");
				sendEvent(packet);
			}
		}
	}

	@Override
	public void dispose() {
		if (comPort != null)
			comPort.closePort();
	}

	@Override
	public int getListeningEvents() {
		return SerialPort.LISTENING_EVENT_DATA_RECEIVED;
	}

	@Override
	public int getPacketSize() {
		return 1;
	}

	StringBuffer receivedPacket = new StringBuffer();

	@Override
	public void serialEvent(SerialPortEvent event) {
		if (event.getEventType() == SerialPort.LISTENING_EVENT_DATA_RECEIVED) {
			receivedPacket.append(new String(event.getReceivedData()));

			String line = receivedPacket.toString();
			try {
				if (line.contains("\n")) {
					String[] contents = line.replace("\n", "").split(
							Pattern.quote("|"));
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
					receivedPacket = new StringBuffer();
				}
			} catch (NumberFormatException ex) {
				// Ignore; incomplete packet received.
			}
		}
	}
}