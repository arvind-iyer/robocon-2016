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
import com.dranithix.robocon.ui.SettingsWindow;
import com.kotcrab.vis.ui.VisUI;
import com.kotcrab.vis.ui.widget.VisTable;

public class Robocon extends ControllerAdapter implements
		ApplicationListener {
	public static boolean MANUAL_MODE = true;

	public static final int COM_PORT = 6;
	public static final String COM_PORT_ADDRESS = "COM".concat(Integer
			.toString(COM_PORT));
	
	private SettingsWindow settingsWindow;

	private SerialThread serialThread;
	private Stage stage;

	Vector3 mousePos = new Vector3(0, 0, 0);
	OrthographicCamera cam;

	SpriteBatch batch;
	BitmapFont font;

	@Override
	public void create() {
		Thread thread = new Thread(serialThread = new SerialThread());
		thread.start();

		VisUI.load();

		stage = new Stage(new FitViewport(1280, 768));
		Gdx.input.setInputProcessor(stage);

		VisTable layout = new VisTable(true);
		layout.defaults().fill().expand().pad(10);

		VisTable settingsLayout = new VisTable(true);
		settingsLayout.add(settingsWindow = new SettingsWindow(serialThread));
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

	private float START_X = 0, START_Y = 0;
	private float TARGET_X = 100, TARGET_Y = 100;

	public float getPathCorrectionAngle(float x, float y) {
		float diffTargetY = TARGET_Y - y;
		float diffTargetX = TARGET_X - x;
		float angleToTarget = MathUtils.atan2(diffTargetY, diffTargetX);
		float targetDst = (float) Math.sqrt(diffTargetY * diffTargetY
				+ diffTargetX * diffTargetX);

		float diffLineX = TARGET_X - START_X;
		float diffLineY = TARGET_Y - START_Y;
		float u = ((x - START_X) * diffLineX + (y - START_Y) * diffLineY)
				/ ((float) diffLineX * diffLineX + diffLineY * diffLineY);
		float linePointX = START_X + u * diffLineX;
		float linePointY = START_Y + u * diffLineY;

		float diffPointX = linePointX - x;
		float diffPointY = linePointY - y;
		float dstToPoint = (float) Math.sqrt(diffPointX * diffPointX
				+ diffPointY * diffPointY);

		float angleToLine = MathUtils.atan2(diffPointY, diffPointX);

		return MathUtils.radDeg
				* MathUtils.atan2(targetDst * MathUtils.sin(angleToTarget)
						+ dstToPoint * MathUtils.sin(angleToLine),
						targetDst * MathUtils.cos(angleToTarget) + dstToPoint
								* MathUtils.cos(angleToLine));

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
		serialThread.dispose();

		VisUI.dispose();
	}

	@Override
	public boolean buttonUp(Controller controller, int buttonIndex) {
		switch (buttonIndex) {
		case 1:
			// serialThread.sendEvent(new MotorControlEvent(
			// MotorControlEvent.MOTOR_LEFT,
			// MotorControlEvent.MOTOR_FORWARD, 200));
			// serialThread.sendEvent(new MotorControlEvent(
			// MotorControlEvent.MOTOR_RIGHT,
			// MotorControlEvent.MOTOR_FORWARD, 200));
			// System.out.println(System.currentTimeMillis() - lastTick);
			break;
		}
		return super.buttonUp(controller, buttonIndex);
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

//		int magnitude = MathUtils
//				.floor((float) ((point.len() / Math.sqrt(2)) * MAX_MOTOR_SPEED));
//
//		System.out.println(magnitude + " Angle: " + angle);
//
//		serialThread.sendEvent(new MotorControlEvent(angle, magnitude));

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
