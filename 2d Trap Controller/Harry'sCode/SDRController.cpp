// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "SDRController.h"
#include <fstream>
#include <iostream>
#include <chrono>

#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd.h>
#include <sched.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

using namespace std;

bool has_underflow = false;
mutex notification_mutex;
condition_variable notification_cv;

SDRController::SDRController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain) {
	sampleRate = sample_rate;
	centerFreq = center_freq;
	gain = tx_gain;

	connected = false;
	shouldDisconnect = false;

	sendThread = NULL;

	if (shouldConnect) {
		initializeUHD();
		startStreaming();
	}
}

bool SDRController::isConnected() {
	return connected;
}

void handle_underflow(uhd::msg::type_t type, const std::string &msg) {
	if (type == uhd::msg::type_t::status) {
		cout << msg;
	}
	else if (type == uhd::msg::type_t::error) {
		cout << "Error: " << msg << endl;
	}
	else if (type == uhd::msg::type_t::fastpath) {
		cout << "Fastpath: " << msg << endl;
		has_underflow = true;
	}
	else if (type == uhd::msg::type_t::warning) {
		cout << "Warning: " << msg << endl;
	}
}

void SDRController::initializeUHD() {
	if (hasInitialized) {
		return;
	}

	uhd::msg::register_handler(&handle_underflow);

	// create a usrp device
	std::cout << boost::format("Creating the USRP device...\n");
	std::string args;
	usrp = uhd::usrp::multi_usrp::make(args);

	// lock clocks
	usrp->set_clock_source("internal");

	// send our desired sample rate and center frequency
	std::cout << boost::format("Using Device: %s\n") % usrp->get_pp_string();
	std::cout << boost::format("Setting TX Rate: %f Msps...\n") % (sampleRate / 1E6);
	usrp->set_tx_rate(sampleRate);

	std::cout << boost::format("Actual TX Rate: %f Msps...\n\n") % (usrp->get_tx_rate() / 1E6);
	std::cout << boost::format("Setting TX Freq: %f MHz...\n") % (centerFreq / 1E6);
	uhd::tune_request_t tune_request(centerFreq);
	usrp->set_tx_freq(tune_request, 0);
	std::cout << boost::format("Actual TX Freq: %f MHz...\n\n") % (usrp->get_tx_freq(0) / 1E6);

	// allow for some setup time
	std::this_thread::sleep_for(std::chrono::seconds(1));

	// check Ref and LO lock detect
	std::vector<std::string> sensor_names;
	sensor_names = usrp->get_tx_sensor_names(0);
	if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked") != sensor_names.end())
	{
		uhd::sensor_value_t lo_locked = usrp->get_tx_sensor("lo_locked", 0);
		std::cout << boost::format("Checking TX: %s ...\n") % lo_locked.to_pp_string();
		UHD_ASSERT_THROW(lo_locked.to_bool());
	}

	// create a transmit streamer
	uhd::stream_args_t stream_args("fc32", "sc16");

	
	stream_args.channels.push_back(0);
	tx_stream = usrp->get_tx_stream(stream_args);

	// Send the maximum number of samples per packet.
	samples_per_packet = tx_stream->get_max_num_samps();
	std::cout << "Sending max number of samples per packet: " << samples_per_packet << endl;

	uhd::gain_range_t gain_range = usrp->get_tx_gain_range(0);
	std::cout << "Gain range: " << gain_range.to_pp_string() << std::endl;

	usrp->set_tx_gain(gain);
	std::cout << "Current gain: " << usrp->get_tx_gain() << std::endl;

	hasInitialized = true;

	this_thread::sleep_for(chrono::seconds(1));
}

void SDRController::startStreaming() {
	queueLock.lock();
	connected = true;
	hasStartedStreaming = false;
	shouldDisconnect = false;
	sendThread = new thread(&SDRController::sendData, this);
}

void SDRController::disconnect() {
	shouldDisconnect = true;

	if (sendThread) {
		sendThread->join();
	}

	connected = false;
}

void SDRController::joinSendThread() {
	sendThread->join();
}

void SDRController::pushWaveform(Waveform waveform) {
	if (waveform.shouldNotifyAfterSending) {
		notificationHasBeenSent = false;
	}
	if (hasStartedStreaming) {
		queueLock.lock();
	}
	localWaveforms.push(waveform);
	waveformQueue.push(&localWaveforms.back());
	queueLock.unlock();
}

void SDRController::pushWaveform(Waveform *waveform) {
	if (waveform->shouldNotifyAfterSending) {
		notificationHasBeenSent = false;
	}
	if (hasStartedStreaming) {
		queueLock.lock();
	}
	waveformQueue.push(waveform);
	queueLock.unlock();
}

void SDRController::pushWaveforms(vector<Waveform> waveforms) {
	if (hasStartedStreaming) {
		queueLock.lock();
	}
	for (int i = 0; i < waveforms.size(); i++) {
		if (waveforms[i].shouldNotifyAfterSending) {
			notificationHasBeenSent = false;
		}

		localWaveforms.push(waveforms[i]);

		waveformQueue.push(&localWaveforms.back());
	}
	queueLock.unlock();
}

void SDRController::pushWaveforms(vector<Waveform *> waveforms) {
	if (hasStartedStreaming) {
		queueLock.lock();
	}
	for (int i = 0; i < waveforms.size(); i++) {
		if (waveforms[i]->shouldNotifyAfterSending) {
			notificationHasBeenSent = false;
		}

		waveformQueue.push(waveforms[i]);
	}
	queueLock.unlock();
}

bool SDRController::changeGain(double newGain) {
	if (connected) {
		usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
		this_thread::sleep_for(chrono::seconds(1));

		usrp->set_tx_gain(newGain);
		this_thread::sleep_for(chrono::seconds(1));
		usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
	}

	return connected;
}

double SDRController::getGain() {
	if (connected) {
		return usrp->get_tx_gain();
	} else {
		return -1.0;
	}
}


static int record_index = 0;
static const int NUM_RECORDS = 10000;
static double timings[NUM_RECORDS];

void SDRController::sendData() {
	//uhd::set_thread_priority_safe();
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(15, &cpuset);

	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

	sched_param sch;
	int policy;
	pthread_getschedparam(pthread_self(), &policy, &sch);
	sch.sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch);
	
	cout << "Set thread priority: " << sch.sched_priority << endl;

	// setup the metadata flags
	uhd::tx_metadata_t md;
	md.start_of_burst = true;
	md.end_of_burst = false;
	md.has_time_spec = false;
	md.time_spec = usrp->get_time_now() + uhd::time_spec_t(0.1);

	// Wait until queue is not empty.
	queueLock.lock();

	cout << "Starting waveform." << endl;

	
	uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
	//cmd.time_spec = usrp->get_time_now() + uhd::time_spec_t(1.0);
	cmd.stream_now = true;
	usrp->issue_stream_cmd(cmd);
	
	
	// Send waveform from queue.
	// If there is only one element in the queue, then repeat this waveform until new ones are pushed onto the queue.
	Waveform *waveform = NULL;
	std::vector<std::complex<float>> *dataVector;
	while (!shouldDisconnect) {
		// Select waveform to output:
		// If the queue lock can be acquired, then check to see if we should start using a new waveform or we should
		// just stick with the same one. If the queue lock cannot be acquired, then just use the same waveform
		// we've been using.
		if (!hasStartedStreaming || queueLock.try_lock()) {
			if (waveformQueue.size() > 1) {
				if (localWaveforms.size() > 0 && waveformQueue.front() == &localWaveforms.front()) {
					localWaveforms.pop();
				}
				Waveform *waveformToRemove = waveformQueue.front();
				if (waveformToRemove->shouldNotifyAfterSending) {
					notificationHasBeenSent = true;
				}
				waveformQueue.pop();
			}
			waveform = waveformQueue.front();
			hasStartedStreaming = true;
			queueLock.unlock();
		}

		dataVector = &(waveform->dataVector);


		// Compute number of packets to fully transmit this waveform.
		size_t num_packets = ceil((float)dataVector->size() / samples_per_packet);

		//md.start_of_burst = true;
		for (size_t i = 0; i < num_packets; i++) {
			size_t num_samples_to_send = samples_per_packet;

			// If there is not a full packet-worth of data to send at the end of this waveform,
			// send just the remaining bytes.
			if ((i + 1) * num_samples_to_send > dataVector->size()) {
				num_samples_to_send = dataVector->size() - i*samples_per_packet;
			}

			// Send packet.
			size_t num_samples_sent = tx_stream->send(&dataVector->at(i*samples_per_packet), num_samples_to_send, md);
			//this_thread::sleep_for(1ms);
			// If we timed out and the packet wasn't received, then step back to send this same packet again.
			if (num_samples_sent == 0) {
				i--;
			}

			md.start_of_burst = false;
			md.has_time_spec = false;
		}
	}

	md.end_of_burst = true;
	tx_stream->send("", 0, md);
}

void SDRController::waitOnWaveformNotification() {
	while (!notificationHasBeenSent) {
		this_thread::sleep_for(chrono::milliseconds(1));
	}
}
