package com.dranithix.robocon;

import com.badlogic.gdx.ApplicationListener;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.ControllerAdapter;
import com.badlogic.gdx.controllers.Controllers;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.utils.viewport.FitViewport;
import com.dranithix.robocon.RobotSerialManager.SerialCommandListener;
import com.dranithix.robocon.ui.SettingsWindow;
import com.kotcrab.vis.ui.VisUI;
import com.kotcrab.vis.ui.widget.VisTable;
import com.pk.robocon.main.Control;
import com.pk.robocon.main.ControlPID;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class Robocon extends ControllerAdapter implements ApplicationListener, SerialCommandListener {
	public static final int COM_PORT = 8;
	public static final String COM_PORT_ADDRESS = "COM".concat(Integer.toString(COM_PORT));

	private SettingsWindow settingsWindow;

	private ControlPID controlPid = new ControlPID();

	private Stage stage;

	Vector3 mousePos = new Vector3(0, 0, 0);
	OrthographicCamera cam;

	SpriteBatch batch;
	BitmapFont font;

	@Override
	public void create() {
		Thread thread = new Thread(RobotSerialManager.getInstance());
		thread.start();

		RobotSerialManager.getInstance().addSerialListener(this);

		VisUI.load();

		stage = new Stage(new FitViewport(800, 600));
		Gdx.input.setInputProcessor(stage);

		VisTable layout = new VisTable(true);
		layout.defaults().fill().expand().pad(10);

		VisTable settingsLayout = new VisTable(true);
		settingsLayout.add(settingsWindow = new SettingsWindow());
		layout.add(settingsLayout);

		layout.setFillParent(true);

		stage.addActor(layout);

		Controllers.addListener(this);

		cam = new OrthographicCamera(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
		cam.position.set(Gdx.graphics.getWidth() / 2, Gdx.graphics.getHeight() / 2, 0);

		batch = new SpriteBatch();
		font = new BitmapFont();

	}

	@Override
	public void render() {
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

		mousePos.set(Gdx.input.getX(), Gdx.input.getY(), 0);
		cam.unproject(mousePos);

		settingsWindow.updateConnectionStatus(RobotSerialManager.getInstance().isRunning());

		if (Controllers.getControllers().size > 0 && !controlPid.execute()) {
			Controller controller = Controllers.getControllers().first();

			float leftYAxis = -controller.getAxis(LEFT_X_AXIS);
			float leftXAxis = controller.getAxis(LEFT_Y_AXIS);

			float rightXAxis = controller.getAxis(RIGHT_Y_AXIS);

			Vector2 leftPoint = new Vector2(leftXAxis, leftYAxis);
			int leftAngle = 90 - (int) leftPoint.angle();
			leftAngle = leftAngle < 0 ? 360 + leftAngle : leftAngle;

			int velocity = (int) MathUtils.clamp((leftPoint.len() / Math.sqrt(2)) * 100, 0, 100);

			int angularVelocity = (int) rightXAxis * 100;

			Integer[] motorValues = Control.calculateRobotValues((int) (velocity * 0.5f), leftAngle, angularVelocity);
			controlPid.setMotorValues(motorValues);
		}
		
		controlPid.sendMotorCommands();

		stage.act();
		stage.draw();
	}

	@Override
	public void dispose() {
		stage.dispose();
		VisUI.dispose();

		RobotSerialManager.getInstance().dispose();
	}

	int brushlessStartCounter = 0;
	int brushlessMagnitude = 0;

	@Override
	public boolean buttonDown(Controller controller, int buttonCode) {
		switch (buttonCode) {
		case 0: // Button 1
			Position targetPos = new Position(new Vector2(0, 1000), 45);
			controlPid.addQueue(new Target(targetPos, new Threshold(0, 0), 0));
			// controlPid.();
			break;
		case 1: // Button 2
			break;
		case 4: // L1
			brushlessStartCounter++;
			break;
		case 5: // R1
			brushlessStartCounter++;
			break;
		case 6: // L2
			brushlessMagnitude += 1;
			brushlessMagnitude = MathUtils.clamp(brushlessMagnitude, 0, 100);
			// fanSystem.fanControl(brushlessMagnitude);
			break;
		case 7: // R2
			brushlessMagnitude -= 1;
			brushlessMagnitude = MathUtils.clamp(brushlessMagnitude, 0, 100);
			// fanSystem.fanControl(brushlessMagnitude);
			break;
		}

		if (brushlessStartCounter == 2) {
			// fanSystem.toggleFan(!fanSystem.isFanStopped());
		}
		return false;
	}

	@Override
	public boolean buttonUp(Controller controller, int buttonCode) {
		switch (buttonCode) {
		case 0: // Button 1
			break;
		case 1: // Button 2
			break;
		case 4: // L1
			brushlessStartCounter--;
			break;
		case 5: // R1
			brushlessStartCounter--;
			break;
		case 6: // L2
			break;
		case 7: // R2
			break;
		}
		return false;
	}

	public static final int RIGHT_X_AXIS = 0;
	public static final int RIGHT_Y_AXIS = 1;
	public static final int LEFT_X_AXIS = 2;
	public static final int LEFT_Y_AXIS = 3;

	@Override
	public boolean axisMoved(Controller controller, int axisCode, float value) {

		return false;
	}

	@Override
	public void resize(int width, int height) {
		stage.getViewport().update(width, height, true);
	}

	@Override
	public void pause() {
	}

	@Override
	public void resume() {
	}

	@Override
	public void onReceive(String[] contents) {
		switch (contents[0]) {
		case "STATE":
			int currentBearing = Integer.decode(contents[3]) / 10;

			controlPid.currentPosUpdate(Integer.decode(contents[1]), Integer.decode(contents[2]), currentBearing);

			// robocon.updateRobotPosition(currentPos,
			// currentBearing);
			break;
		default:
			break;
		}
	}
}
