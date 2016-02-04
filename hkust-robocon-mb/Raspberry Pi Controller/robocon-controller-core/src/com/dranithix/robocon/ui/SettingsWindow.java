package com.dranithix.robocon.ui;

import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.scenes.scene2d.Actor;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import com.dranithix.robocon.RobotSerialManager;
import com.kotcrab.vis.ui.building.OneColumnTableBuilder;
import com.kotcrab.vis.ui.building.TableBuilder;
import com.kotcrab.vis.ui.building.utilities.CellWidget;
import com.kotcrab.vis.ui.building.utilities.Padding;
import com.kotcrab.vis.ui.util.TableUtils;
import com.kotcrab.vis.ui.widget.VisLabel;
import com.kotcrab.vis.ui.widget.VisTextButton;
import com.kotcrab.vis.ui.widget.VisWindow;

public class SettingsWindow extends VisWindow {
	private VisLabel connectLabel;
	private VisTextButton btnConnect;
	
	private RobotSerialManager serial;

	public SettingsWindow(RobotSerialManager serial) {
		super("Settings");
		
		this.serial = serial;

		TableUtils.setSpacingDefaults(this);
		TableBuilder builder = new OneColumnTableBuilder(new Padding(2, 3));

		RowLayout rowLayout = new RowLayout(new Padding(0, 0, 0, 5));
		
		connectLabel = new VisLabel(
				"Disconnected.");
		connectLabel.setColor(Color.RED);

		builder.append(
				rowLayout,
				CellWidget.builder().fillX().expandX(),
				CellWidget.of(
						connectLabel).wrap(),
				CellWidget.of(btnConnect = new VisTextButton("Connect")).expandX().fillX()
						.wrap());
		builder.row();
		
		btnConnect.addCaptureListener(new ChangeListener() {

			@Override
			public void changed(ChangeEvent event, Actor actor) {
				toggleConnection();
			}
			
		});

		Table table = builder.build();
		add(table).expand().fill();

		pack();
	}
	
	public void toggleConnection() {
		connectLabel.setColor(Color.YELLOW);
		connectLabel.setText("Connecting...");
		
		boolean connected = serial.toggleConnection();
		if (connected) {
			connectLabel.setColor(Color.GREEN);
			connectLabel.setText("Connected.");
		} else {
			connectLabel.setColor(Color.RED);
			connectLabel.setText("Disconnected.");
		}
		
	}
}
