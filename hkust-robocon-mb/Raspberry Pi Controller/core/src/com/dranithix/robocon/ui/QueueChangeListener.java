package com.dranithix.robocon.ui;

public class QueueChangeListener {
	private QueueChangeInterface changeInterface;
	private boolean queueAddCheck, queueRemoveCheck;

	public QueueChangeListener(QueueChangeInterface queueInterface) {
		changeInterface = queueInterface;
		queueAddCheck = false;
		queueRemoveCheck = false;
	}

	public void update() {
		if (queueAddCheck) {
			changeInterface.callUpdateQueue();
		}
		if (queueRemoveCheck) {
			changeInterface.removeTargetListener();
		}
		queueRemoveCheck = false;
		queueAddCheck = false;
	}

	public void setInterfaceCheck(boolean interfaceCheck) {
		this.queueAddCheck = interfaceCheck;
	}

	public void setQueueRemoveCheck(boolean queueRemoveCheck) {
		this.queueRemoveCheck = queueRemoveCheck;
	}

}
