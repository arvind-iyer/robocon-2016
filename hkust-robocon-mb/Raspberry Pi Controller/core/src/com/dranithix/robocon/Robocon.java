package com.dranithix.robocon;

import java.util.ArrayList;

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
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.utils.viewport.FitViewport;
import com.dranithix.robocon.RobotSerialManager.SerialCommandListener;
import com.dranithix.robocon.ui.SettingsWindow;
import com.kotcrab.vis.ui.VisUI;
import com.kotcrab.vis.ui.widget.VisTable;
import com.dranithix.robocon.ui.LabelsList;
import com.dranithix.robocon.ui.OperationUI;
import com.dranithix.robocon.ui.PositionListView;
import com.dranithix.robocon.ui.QueueChangeInterface;
import com.dranithix.robocon.ui.QueueChangeListener;
import com.pk.robocon.main.Control;
import com.pk.robocon.main.ControlPID;
import com.pk.robocon.system.Path;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class Robocon extends ControllerAdapter implements ApplicationListener, 
SerialCommandListener, QueueChangeInterface{
	
	public static final int COM_PORT = 8;
	public static final String COM_PORT_ADDRESS = "COM".concat(Integer.toString(COM_PORT));

	private SettingsWindow settingsWindow;

	public ControlPID controlPid = new ControlPID();

	public ControlPID getControlPid() {
		return this.controlPid;
	}

	private Stage stage;
	
	private QueueChangeListener queueListener;

	Vector3 mousePos = new Vector3(0, 0, 0);
	OrthographicCamera cam;
	
	LabelsList labelList;
	PositionListView positionList;

	SpriteBatch batch;
	BitmapFont font;

	@Override
	public void create() {
		Thread thread = new Thread(RobotSerialManager.getInstance());
		thread.start();

		RobotSerialManager.getInstance().addSerialListener(this);
		queueListener = new QueueChangeListener(this);

		VisUI.load();

		stage = new Stage(new FitViewport(800, 600));
		Gdx.input.setInputProcessor(stage);

		VisTable layout = new VisTable(true);
		layout.defaults().fill().expand().pad(10);

		VisTable settingsLayout = new VisTable(true);
		settingsLayout.add(settingsWindow = new SettingsWindow());
		layout.add(settingsLayout);
		layout.row();
		
		labelList = new LabelsList();
		positionList = new PositionListView(labelList, this);

		Table table = new Table();
		table.setFillParent(true);
		table.add(positionList).fill().grow().size(540, 250);
		table.add(labelList).fill().grow().size(240, 250);

		layout.add(table);
		
		layout.setFillParent(true);

		stage.addActor(layout);

		Controllers.addListener(this);

		cam = new OrthographicCamera(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
		cam.position.set(Gdx.graphics.getWidth() / 2, Gdx.graphics.getHeight() / 2, 0);

		batch = new SpriteBatch();
		font = new BitmapFont();

	}
	
	public void callUpdateQueue() {
		positionList.updateQueueArray();
	}
	public void removeTargetListener(){
		positionList.deleteTopMostEntry();
	}
	
	/*public void queueChange(Path path, OperationUI receivedOperation.operationNum){
		OperationUI operation = receivedOperation;
		
	}*/
	
	@Override
	public void render() {
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

		mousePos.set(Gdx.input.getX(), Gdx.input.getY(), 0);
		cam.unproject(mousePos);

		settingsWindow.updateConnectionStatus(RobotSerialManager.getInstance().isRunning());
		
		if (Controllers.getControllers().size > 0 && !controlPid.execute(queueListener)) {
			Controller controller = Controllers.getControllers().first();

			float leftYAxis = -controller.getAxis(LEFT_X_AXIS);
			float leftXAxis = controller.getAxis(LEFT_Y_AXIS);

			float rightXAxis = controller.getAxis(RIGHT_Y_AXIS);

			Vector2 leftPoint = new Vector2(leftXAxis, leftYAxis);
			int leftJoystickAngle = 90 - (int) leftPoint.angle();
			leftJoystickAngle = leftJoystickAngle < 0 ? 360 + leftJoystickAngle : leftJoystickAngle;

			int velocity = (int) MathUtils.clamp((leftPoint.len() / Math.sqrt(2)) * 100, 0, 100);

			int angularVelocity = (int) rightXAxis * 100;
			controlPid.calculateMotorValues((int) (velocity * 0.5f), leftJoystickAngle, angularVelocity);
		}
		
		try {
			positionList.updateLabelsList(positionList.snapshotPath);
		} catch (java.lang.NullPointerException e) {
		}
		queueListener.update();
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

	boolean pidState = false;

	@Override
	public boolean buttonDown(Controller controller, int buttonCode) {
		switch (buttonCode) {
		case 0: // Button 1
			pidState = !pidState;
			if (pidState == true) {
				ArrayList<Vector2> control=new ArrayList<Vector2>();
				control.add(new Vector2(500, 500));
				control.add(new Vector2(0, 1000));
				controlPid.addCurve(control, 45);
				queueListener.setInterfaceCheck(true);
				//Position targetPos = new Position(new Vector2(0, 1000), 45);
				//controlPid.addQueue(new Target(targetPos, new Threshold(0, 0), 0));
			} else {
				controlPid.getQueue().clear();
			}
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

			Control.currentPosUpdate(Integer.decode(contents[1]), Integer.decode(contents[2]), currentBearing);

			// robocon.updateRobotPosition(currentPos,
			// currentBearing);
			break;
		default:
			break;
		}
	}
}
