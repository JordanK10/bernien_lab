#include "AWGController.h"

AWGController::AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain) {
	sampleRate = sample_rate;
	centerFreq = center_freq;
	gain = tx_gain;

	// if (shouldConnect) {
	// 	initializeUHD();
	// 	startStreaming();
	// }
}

	void AWGController::disconnect() {
		shouldDisconnect = true;

		// if (sendThread) {
		// 	sendThread->join();
		// }

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

	bool AWGController::isConnected() {
		return connected;
	}
