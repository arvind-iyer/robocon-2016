package com.dranithix.robocon.ui;

import com.badlogic.gdx.scenes.scene2d.Actor;
import com.kotcrab.vis.ui.building.OneRowTableBuilder;
import com.kotcrab.vis.ui.building.utilities.CellWidget;
import com.kotcrab.vis.ui.building.utilities.Padding;
import com.kotcrab.vis.ui.building.utilities.layouts.ActorLayout;

public class RowLayout implements ActorLayout {
	private Padding padding;

	public RowLayout(Padding padding) {
		this.padding = padding;
	}

	@Override
	public Actor convertToActor(Actor... widgets) {
		return convertToActor(CellWidget.wrap(widgets));
	}

	@Override
	public Actor convertToActor(CellWidget<?>... widgets) {
		OneRowTableBuilder builder = new OneRowTableBuilder(padding);

		for (CellWidget<?> widget : widgets)
			builder.append(widget);

		return builder.build();
	}
}