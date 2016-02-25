package com.pk.robocon.main;

public class ControlPneumatics extends Control {
	
	public static final boolean OPEN = true;
	public static final boolean CLOSE = false;
	
	public void executePneumatics(boolean state){
		super.sendPneumatics(state);
	}
		
}
