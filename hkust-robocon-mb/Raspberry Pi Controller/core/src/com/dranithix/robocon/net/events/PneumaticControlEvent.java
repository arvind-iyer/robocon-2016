package com.dranithix.robocon.net.events;

import com.dranithix.robocon.net.NetworkEvent;
import com.dranithix.robocon.net.NetworkEventOpcode;

public class PneumaticControlEvent extends NetworkEvent{
	public enum PneumaticType{
		PNEUMATIC_1, PNEUMATIC_2;
	}
	
	private PneumaticType pneumaticId;
	private int state;
	
	public PneumaticControlEvent(PneumaticType pneumaticId, int state){
		super(NetworkEventOpcode.PNEUMATIC_CONTROL);
		
		this.pneumaticId = pneumaticId;
		this.state = state;
		
		initEvent();
	}
	
	@Override
	protected void initEvent(){
		putInt(pneumaticId.ordinal());
		putInt(state);
	}

}
