package com.dranithix.robocon.actors;

import com.kotcrab.vis.ui.widget.VisTable;

/**
 * 
 * @author Kenta Iwasaki
 *
 */
public class GameMapActor extends VisTable {

	// private static ShapeRenderer renderer = new ShapeRenderer();
	// private static ShapeRenderer tileRenderer = new ShapeRenderer();
	//
	// private List<GridCell> path;
	// private GameTileActor[][] map;
	//
	// public GameMapActor() {
	// super();
	// }
	//
	// public void updateMap(final GameTileActor[][] map) {
	// this.map = map;
	//
	// this.clearChildren();
	// for (int y = 0; y < map[0].length; y++) {
	// for (int x = 0; x < map.length; x++) {
	//
	// this.add(map[x][y]).fill().expand();
	// }
	// this.row();
	// }
	// }
	//
	// public void updatePath(final List<GridCell> path) {
	// this.path = path;
	// }
	//
	// @Override
	// public void draw(Batch batch, float parentAlpha) {
	// // TODO Auto-generated method stub
	// super.draw(batch, parentAlpha);
	//
	// batch.end();
	//
	// if (path != null) {
	// float[] polyLine = new float[path.size() * 2];
	// int x = 0;
	//
	// renderer.begin(ShapeType.Filled);
	// for (GridCell cell : path) {
	// GameTileActor tile = map[cell.x][cell.y];
	// Vector2 pos = new Vector2(tile.getX() + tile.getWidth() / 2,
	// tile.getY() + tile.getHeight() / 2);
	// localToStageCoordinates(pos);
	//
	// polyLine[x++] = pos.x;
	// polyLine[x++] = pos.y;
	//
	// renderer.circle(pos.x, pos.y, 5);
	// }
	// renderer.end();
	//
	// renderer.begin(ShapeType.Line);
	// renderer.polyline(polyLine);
	// renderer.end();
	// }
	//
	// batch.begin();
	// }
	//
	// public static class GameTileActor extends Widget {
	// public static final int TILE_EMPTY = 0;
	// public static final int TILE_OBSTACLE = 1;
	// public static final int TILE_START = 2;
	// public static final int TILE_END = 3;
	//
	// private GridCell cell;
	// private int type;
	//
	// public GameTileActor(int x, int y, int type) {
	// super();
	//
	// this.cell = new GridCell(x, y, true);
	// setType(type);
	//
	// this.addListener(new InputListener() {
	//
	// @Override
	// public boolean touchDown(InputEvent event, float x, float y,
	// int pointer, int button) {
	// if (getType() == GameTileActor.TILE_EMPTY) {
	// setType(GameTileActor.TILE_OBSTACLE);
	// } else if (getType() == GameTileActor.TILE_OBSTACLE) {
	// setType(GameTileActor.TILE_EMPTY);
	// }
	// return super.touchDown(event, x, y, pointer, button);
	// }
	//
	// });
	// }
	//
	// public GridCell getCell() {
	// return cell;
	// }
	//
	// public int getType() {
	// return type;
	// }
	//
	// public void setType(int type) {
	// this.type = type;
	// cell.setWalkable(type != GameTileActor.TILE_OBSTACLE);
	// }
	//
	// @Override
	// public void draw(Batch batch, float parentAlpha) {
	// batch.end();
	//
	// tileRenderer.setTransformMatrix(batch.getTransformMatrix());
	// tileRenderer.setProjectionMatrix(batch.getProjectionMatrix());
	//
	// tileRenderer.begin(ShapeType.Filled);
	// switch (type) {
	// case GameTileActor.TILE_EMPTY:
	// tileRenderer.setColor(Color.CLEAR);
	// break;
	// case GameTileActor.TILE_OBSTACLE:
	// tileRenderer.setColor(Color.GREEN);
	// break;
	// case GameTileActor.TILE_START:
	// tileRenderer.setColor(Color.BLUE);
	// break;
	// case GameTileActor.TILE_END:
	// tileRenderer.setColor(Color.GREEN);
	// break;
	// }
	// tileRenderer.rect(getX(), getY(), getWidth(), getHeight());
	// tileRenderer.end();
	//
	// tileRenderer.begin(ShapeType.Line);
	// tileRenderer.setColor(Color.WHITE);
	// tileRenderer.rect(getX(), getY(), getWidth(), getHeight());
	// tileRenderer.end();
	//
	// batch.begin();
	//
	// super.draw(batch, parentAlpha);
	// }
	//
	// }
}
