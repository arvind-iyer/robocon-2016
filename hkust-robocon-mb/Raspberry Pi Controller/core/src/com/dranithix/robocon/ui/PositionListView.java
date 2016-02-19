package com.dranithix.robocon.ui;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.scenes.scene2d.Actor;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import com.badlogic.gdx.scenes.scene2d.utils.Drawable;
import com.badlogic.gdx.utils.Array;
import com.dranithix.robocon.Robocon;
import com.kotcrab.vis.ui.VisUI;
import com.kotcrab.vis.ui.layout.HorizontalFlowGroup;
import com.kotcrab.vis.ui.util.TableUtils;
import com.kotcrab.vis.ui.util.adapter.ArrayAdapter;
import com.kotcrab.vis.ui.util.form.FormValidator;
import com.kotcrab.vis.ui.widget.ListView;
import com.kotcrab.vis.ui.widget.ListView.ItemClickListener;
import com.kotcrab.vis.ui.widget.ListView.UpdatePolicy;
import com.kotcrab.vis.ui.widget.VisLabel;
import com.kotcrab.vis.ui.widget.VisTable;
import com.kotcrab.vis.ui.widget.VisTextButton;
import com.kotcrab.vis.ui.widget.VisValidatableTextField;
import com.kotcrab.vis.ui.widget.VisWindow;
import com.pk.robocon.main.Control;
import com.pk.robocon.main.ControlPID;
import com.pk.robocon.system.Path;
import com.pk.robocon.system.Position;
import com.pk.robocon.system.Target;
import com.pk.robocon.system.Threshold;

/**
 * 
 * @author Kristian Suhartono
 *
 */
public class PositionListView extends VisWindow {
	private LabelsList labels;
	public Path snapshotPath;
	private Robocon robocon;
	private ArrayList<Path> listPathArray;
	private Array<Path> pathArray;
	private final TestAdapter adapter;

	public PositionListView(final LabelsList labels, Robocon robocon) {
		super("Positions List:");
		this.labels = labels;
		this.robocon = robocon;

		TableUtils.setSpacingDefaults(this);
		columnDefaults(0).left();

		pathArray = new Array<Path>();

		adapter = new TestAdapter(pathArray);
		ListView<Path> view = new ListView<Path>(adapter);
		view.setUpdatePolicy(UpdatePolicy.ON_DRAW);

		final VisValidatableTextField xField = new VisValidatableTextField();
		final VisValidatableTextField yField = new VisValidatableTextField();
		final VisValidatableTextField angleField = new VisValidatableTextField();
		final VisValidatableTextField distanceErrorField = new VisValidatableTextField();
		final VisValidatableTextField angleErrorField = new VisValidatableTextField();
		VisTextButton addButton = new VisTextButton("Add");
		VisTextButton removeButton = new VisTextButton("Remove");

		FormValidator xValidator = new FormValidator(addButton);
		xValidator.notEmpty(xField, "");
		xValidator.notEmpty(yField, "");
		xValidator.notEmpty(angleField, "");
		xValidator.notEmpty(distanceErrorField, "");
		xValidator.notEmpty(angleErrorField, "");

		add(new VisLabel("New DataPoint:")).row();
		HorizontalFlowGroup flowfuck = new HorizontalFlowGroup(5);
		flowfuck.addActor(xField);
		flowfuck.addActor(yField);
		flowfuck.addActor(angleField);
		add(flowfuck).expandX().fillX().row();
		HorizontalFlowGroup thebitcheshavearrived = new HorizontalFlowGroup(5);
		thebitcheshavearrived.addActor(distanceErrorField);
		thebitcheshavearrived.addActor(angleErrorField);
		thebitcheshavearrived.addActor(addButton);
		thebitcheshavearrived.addActor(removeButton);
		add(thebitcheshavearrived).expandX().fillX().row();

		addButton.addListener(new ChangeListener() {
			@Override
			public void changed(ChangeEvent event, Actor actor) {
				// by changing array using adapter view will be invalidated
				// automatically
				int x,y,bearing,vel, distErr,angleErr;
				
				x = Integer.parseInt(xField.getText());
				y = Integer.parseInt(yField.getText());
				bearing = Integer.parseInt(angleField.getText());
				distErr = Integer.parseInt(distanceErrorField.getText());
				angleErr = Integer.parseInt(angleErrorField.getText());
				try{vel = (int) snapshotPath.getM();}
				catch(java.lang.NullPointerException e){
					vel = 0;
				}
				
				Target t = new Target(new Position(new Vector2(x,y), bearing)
						, new Threshold(distErr, angleErr), vel);
				Path path = new Path(t);
				robocon.getControlPid().addQueue(t);
				adapter.add(path);
				/*
				 * adapter.add(new Path(Integer.parseInt(xField.getText()),
				 * Integer.parseInt(yField.getText()),
				 * Integer.parseInt(angleField.getText()),
				 * Integer.parseInt(distanceErrorField.getText()),
				 * Integer.parseInt(angleErrorField.getText()),
				 * Integer.parseInt(velocityField.getText())));
				 * xField.setText(""); yField.setText("");
				 * angleField.setText(""); distanceErrorField.setText("");
				 * angleErrorField.setText(""); velocityField.setText("");
				 */
			}
		});

		removeButton.addListener(new ChangeListener() {
			@Override
			public void changed(ChangeEvent event, Actor actor) {
				Array<Path> item = adapter.getSelection();
				for (Path location : item) {
					pathArray.removeValue(location, true);
					adapter.itemsDataChanged();
				}

			}
		});

		add(view.getMainTable()).grow().expand().fill().center().row();
		view.setItemClickListener(new ItemClickListener<Path>() {
			@Override
			public void clicked(Path item) {
				labels.setTargX(Integer.toString(item.getTarget().getPosition().getX()));
				labels.setTargY(Integer.toString(item.getTarget().getPosition().getY()));
				labels.setTargAngle(Integer.toString(item.getTarget().getPosition().getBearing()));
				snapshotPath = item;
			}
		});
	}

	/*
	 * public static class Position { protected int x; protected int y;
	 * protected int angle; protected int distError; protected int angleError;
	 * protected int vel;
	 * 
	 * public Position(int x, int y, int angle, int angleError, int distError,
	 * int vel) { this.x = x; this.y = y; this.angle = angle; this.distError =
	 * distError; this.angleError = angleError; this.vel = vel; } }
	 */

	private static class TestAdapter extends ArrayAdapter<Path, VisTable> {
		private final Drawable bg = VisUI.getSkin().getDrawable("window-bg");
		private final Drawable selection = VisUI.getSkin().getDrawable("list-selection");

		public TestAdapter(Array<Path> array) {
			super(array);
			setSelectionMode(SelectionMode.MULTIPLE);
		}

		@Override
		protected VisTable createView(Path item) {
			VisLabel labelX = new VisLabel(Integer.toString(item.getTarget().getPosition().getX()) + ",");
			VisLabel labelY = new VisLabel(Integer.toString(item.getTarget().getPosition().getY()) + ",");
			VisLabel labelA = new VisLabel(Integer.toString(item.getTarget().getPosition().getBearing()) + ",");
			VisLabel labelDistErr = new VisLabel(Float.toString(item.getErrDist()) + ",");
			VisLabel labelAngleErr = new VisLabel(Float.toString(item.getErrAngle()) + ",");
			VisLabel labelVelocity = new VisLabel(Integer.toString(item.getTarget().getVel()));

			VisTable table = new VisTable();
			table.left();
			table.add(labelX);
			table.add(labelY);
			table.add(labelA);
			table.add(labelDistErr);
			table.add(labelAngleErr);
			table.add(labelVelocity);

			return table;
		}

		@Override
		protected void updateView(VisTable view, Path item) {
			super.updateView(view, item);
		}

		@Override
		protected void selectView(VisTable view) {
			view.setBackground(selection);
		}

		@Override
		protected void deselectView(VisTable view) {
			view.setBackground(bg);
		}
	}

	public void updateLabelsList(Path item) {
		this.labels.valueLabelX.setText(Integer.toString((Control.getCurrentPos().getX())));
		this.labels.valueLabelY.setText(Integer.toString((Control.getCurrentPos().getY())));
		this.labels.valueLabelAngle.setText(Integer.toString((Control.getCurrentPos().getBearing())));
		this.labels.valueTargetLabelDistanceError.setText(Float.toString(item.getErrDist()));
		this.labels.valueTargetLabelAngleError.setText(Float.toString(item.getErrAngle()));
		this.labels.valueTargetLabelVelocity.setText(Integer.toString(item.getTarget().getVel()));
	}

	public void updateQueueArray() {
		listPathArray = robocon.getControlPid().getQueue();
		for (Path path : listPathArray) {
			pathArray.add(path);
		}
		adapter.itemsDataChanged();

	}

	public void deleteTopMostEntry() {
		pathArray.removeIndex(0);
		if(pathArray.size > 0) 
			{snapshotPath = pathArray.get(0);}
		adapter.itemsDataChanged();
	}
}
