package com.dranithix.robocon.ui;

import com.badlogic.gdx.utils.Align;
import com.kotcrab.vis.ui.layout.VerticalFlowGroup;
import com.kotcrab.vis.ui.util.TableUtils;
import com.kotcrab.vis.ui.widget.VisLabel;
import com.kotcrab.vis.ui.widget.VisTable;
import com.kotcrab.vis.ui.widget.VisWindow;
import com.pk.robocon.main.Control;

/**
 * 
 * @author Kristian Suhartono
 *
 */

public class LabelsList extends VisWindow {
	VisLabel valueLabelX, valueLabelY, valueLabelAngle, valueTargetLabelX, valueTargetLabelY, valueTargetLabelAngle,
			valueTargetLabelDistanceError, valueTargetLabelAngleError, valueTargetLabelVelocity;
	String currX = "0", currY = "0", currAngle = "0", targX = "0", targY = "0", targAngle = "0", targDistError = "0",
			targAngleError = "0", targVelocity = "0";

	public LabelsList() {
		super("Labels List");

		TableUtils.setSpacingDefaults(this);
		columnDefaults(0).left();

		valueLabelX = new VisLabel(currX);
		valueLabelY = new VisLabel(currY);
		valueLabelAngle = new VisLabel(currAngle);
		valueTargetLabelX = new VisLabel(targX);
		valueTargetLabelY = new VisLabel(targY);
		valueTargetLabelAngle = new VisLabel(targAngle);
		valueTargetLabelDistanceError = new VisLabel(targDistError);
		valueTargetLabelAngleError = new VisLabel(targAngleError);
		valueTargetLabelVelocity = new VisLabel(targVelocity);

		VisTable labelTable = new VisTable();
		labelTable.add("Current X Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueLabelX).padLeft(5);
		labelTable.row();

		labelTable.add("Current Y Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueLabelY).padLeft(5);
		labelTable.row();

		labelTable.add("Current Angle Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueLabelAngle).padLeft(5);
		labelTable.row();

		labelTable.add("Target X Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelX).padLeft(5);
		labelTable.row();

		labelTable.add("Target Y Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelY).padLeft(5);
		labelTable.row();

		labelTable.add("Target Angle Value: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelAngle).padLeft(5);
		labelTable.row();

		labelTable.add("Target Distance Error: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelDistanceError).padLeft(5);
		labelTable.row();

		labelTable.add("Target Angle Error: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelAngleError).padLeft(5);
		labelTable.row();

		labelTable.add("Target Velocity: ").align(Align.left);
		labelTable.addSeparator(true);
		labelTable.add(valueTargetLabelVelocity).padLeft(5);
		labelTable.row();

		add(labelTable).expand().grow().fill().row();

	}

	public String getTargX() {
		return targX;
	}

	public void setTargX(String targX) {
		this.targX = targX;
		this.valueTargetLabelX.setText(targX);

	}

	public String getTargY() {
		return targY;
	}

	public void setTargY(String targY) {
		this.targY = targY;
		this.valueTargetLabelY.setText(targY);
	}

	public String getTargAngle() {
		return targAngle;
	}

	public void setTargAngle(String targAngle) {
		this.targAngle = targAngle;
		this.valueTargetLabelAngle.setText(targAngle);
	}

	public String getTargDistError() {
		return targDistError;
	}

	public void setTargDistError(String targDistError) {
		this.targDistError = targDistError;
		this.valueTargetLabelDistanceError.setText(targDistError);
	}

	public String getTargAngleError() {
		return targAngleError;
	}

	public void setTargAngleError(String targAngleError) {
		this.targAngleError = targAngleError;
		this.valueTargetLabelAngleError.setText(targAngleError);
	}

	public String getTargVelocity() {
		return targVelocity;
	}

	public void setTargVelocity(String targVelocity) {
		this.targVelocity = targVelocity;
		this.valueTargetLabelVelocity.setText(targVelocity);
	}

}