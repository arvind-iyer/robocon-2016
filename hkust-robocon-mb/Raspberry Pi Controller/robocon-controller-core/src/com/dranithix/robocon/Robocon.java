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
import com.badlogic.gdx.utils.Timer;
import com.badlogic.gdx.utils.viewport.FitViewport;
import com.dranithix.robocon.ui.SettingsWindow;
import com.kotcrab.vis.ui.VisUI;
import com.kotcrab.vis.ui.widget.VisTable;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class Robocon extends ControllerAdapter implements ApplicationListener {
	public static final int COM_PORT = 6;
	public static final String COM_PORT_ADDRESS = "COM".concat(Integer
			.toString(COM_PORT));

	private SettingsWindow settingsWindow;

	private RobotSerialManager serial;
	private Stage stage;

	Vector3 mousePos = new Vector3(0, 0, 0);
	OrthographicCamera cam;

	SpriteBatch batch;
	BitmapFont font;

	RobotControlSystem controlSystem;

	@Override
	public void create() {
		Thread thread = new Thread(serial = new RobotSerialManager(this));
		thread.start();

		controlSystem = new RobotControlSystem(serial);
		Timer.instance().scheduleTask(controlSystem, 0, 0.001f);

		VisUI.load();

		stage = new Stage(new FitViewport(800, 600));
		Gdx.input.setInputProcessor(stage);

		VisTable layout = new VisTable(true);
		layout.defaults().fill().expand().pad(10);

		VisTable settingsLayout = new VisTable(true);
		settingsLayout.add(settingsWindow = new SettingsWindow(serial));
		layout.add(settingsLayout);

		layout.setFillParent(true);

		stage.addActor(layout);

		Controllers.addListener(this);

		cam = new OrthographicCamera(Gdx.graphics.getWidth(),
				Gdx.graphics.getHeight());
		cam.position.set(Gdx.graphics.getWidth() / 2,
				Gdx.graphics.getHeight() / 2, 0);

		batch = new SpriteBatch();
		font = new BitmapFont();

	}

	public void updateRobotPosition(Vector2 currentPos, int bearing) {
		controlSystem.updateRobotPosition(currentPos, bearing);
	}

	@Override
	public void render() {
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

		mousePos.set(Gdx.input.getX(), Gdx.input.getY(), 0);
		cam.unproject(mousePos);

		stage.act();
		stage.draw();
	}

	@Override
	public void dispose() {
		stage.dispose();
		serial.dispose();

		VisUI.dispose();
	}

	@Override
	public boolean buttonDown(Controller controller, int buttonCode) {
		switch (buttonCode) {
		case 0: // Button 1
			break;
		case 1: // Button 2
			break;
		case 4: // L1
			break;
		case 5: // R1
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
		float yAxis = -controller.getAxis(LEFT_X_AXIS);
		float xAxis = controller.getAxis(LEFT_Y_AXIS);

		Vector2 point = new Vector2(xAxis, yAxis);
		int angle = 90 - (int) point.angle();
		angle = angle < 0 ? 360 + angle : angle;

		int velocity = (int) MathUtils
				.clamp((point.len() / Math.sqrt(2)) * 100, 0, 100);

		controlSystem.moveRobot(velocity, angle, 0);

		//
		// System.out.println(magnitude + " Angle: " + angle);
		//
		// serialThread.sendEvent(new MotorControlEvent(angle, magnitude));

		// serialThread.sendEvent(new MotorControlEvent(
		// MotorControlEvent.MOTOR_LEFT, leftMotorDir, leftMotorScaled));
		// serialThread
		// .sendEvent(new MotorControlEvent(MotorControlEvent.MOTOR_RIGHT,
		// rightMotorDir, rightMotorScaled));
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
}
