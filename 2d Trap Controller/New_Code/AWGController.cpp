#include "AWGController.h"

AWGController::AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain) {
	sampleRate = sample_rate;
	centerFreq = center_freq;
	gain = tx_gain;

	// if (shouldConnect) {
	// 		string address = " ";
	// 		hDrv = spcm_hOpen(address);
	// 		if(!hDrv)
	// 			cout << "open of driver failed \n";
}

	void AWGController::disconnect() {
		shouldDisconnect = true;

		// spcm_vClose(hDrv);

		connected = false;
	}

	void AWGController::pushWaveform(Waveform waveform) {
		// if (waveform.shouldNotifyAfterSending) {
		// 	notificationHasBeenSent = false;
		// }
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// localWaveforms.push(waveform);
		// waveformQueue.push(&localWaveforms.back());
		// queueLock.unlock();
	}

	void AWGController::pushWaveform(Waveform *waveform) {
		// if (waveform->shouldNotifyAfterSending) {
		// 	notificationHasBeenSent = false;
		// }
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// waveformQueue.push(waveform);
		// queueLock.unlock();
	}

	void AWGController::pushWaveform(vector<Waveform> waveform){
		return;
	}

	void AWGController::pushWaveforms(vector<Waveform> waveforms) {
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// for (int i = 0; i < waveforms.size(); i++) {
		// 	if (waveforms[i].shouldNotifyAfterSending) {
		// 		notificationHasBeenSent = false;
		// 	}
		//
		// 	localWaveforms.push(waveforms[i]);
		//
		// 	waveformQueue.push(&localWaveforms.back());
		// }
		// queueLock.unlock();
	}

	void AWGController::pushWaveforms(vector<Waveform *> waveforms) {
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// for (int i = 0; i < waveforms.size(); i++) {
		// 	if (waveforms[i]->shouldNotifyAfterSending) {
		// 		notificationHasBeenSent = false;
		// 	}
		//
		// 	waveformQueue.push(waveforms[i]);
		// }
		// queueLock.unlock();
	}


	bool AWGController::isConnected() {
		return connected;
	}
