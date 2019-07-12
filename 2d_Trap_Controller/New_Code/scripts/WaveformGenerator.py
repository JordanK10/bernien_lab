import numpy as np
import matplotlib.pyplot as plt
import struct
import os
import random
import threading
from scipy.interpolate import Rbf


def phase(t, T, f1, f2):
    t_less_than_half_T = (np.sign(T/2.0 - t) + 1) / 2
    t_greater_than_T = (np.sign(t - T) + 1) / 2

    less_than_val_2pi =  f1*t + (2.0/3.0) * (f2-f1) * t**3.0 / T**2.0
    greater_than_val_2pi = (1.0/6.0) * (f2-f1)*T + (2.0*f1 - f2)*t + 2.0*(f2-f1) * t**2.0 / T - (2.0/3.0) * (f2-f1) * t**3.0 / T**2.0

    res_2pi = less_than_val_2pi * t_less_than_half_T + greater_than_val_2pi * (1 - t_less_than_half_T)


    greater_than_t = np.pi*(f1 + f2)*T + 2*np.pi*f2*(t-T)

    return (res_2pi * 2 * np.pi) * (1 - t_greater_than_T) + t_greater_than_T * greater_than_t

# Phase trajectory for minimal jerk trajectory.
# Boundary conditions:
# Sixth derivative of f(t) = 0 (defines minimal jerk).
# f(0) = f1
# f(T) = f2
# f'(0) = f'(T) = 0
# f''(0) = f''(T) = 0
def phaseMinimalJerk(t, T, f1, f2):
	phase_2pi =  f1 * t - 2.5 * (f1-f2) * (t**4.0)/(T**3.0) + 3.0 * (f1-f2) * (t**5.0) / (T**4.0) - (f1 - f2) * (t**6.0) / (T**5.0)
	return 2.0 * np.pi * phase_2pi



def instantaneous_freq(t, T, f1, f2):
    t_less_than_half_T = (np.sign(T/2.0 - t) + 1) / 2
    t_greater_than_T = (np.sign(t - T) + 1) / 2

    less_than_val = f1 + 2 * (f2 - f1) * (t/T) ** 2.0
    greater_than_val = f2 - (t - T)**2.0 * 2 * (f2 - f1) / T**2.0

    res = less_than_val * t_less_than_half_T + greater_than_val * (1 - t_less_than_half_T)

    return res * (1 - t_greater_than_T) + t_greater_than_T * f2


def create_amplitude_landscape(filename, should_smooth=False):
	# The goal is to interpolate from good amplitude settings for 100 traps.
	# We initially load in data points from a good configuration file.
	directory = "../bin/DefaultTrapConfigurations/"
	landmark_traps = np.loadtxt("%s/%s" %(directory, filename))

	landmark_freqs = landmark_traps[:, 0] * 1.0E6
	landmark_amps = landmark_traps[:, 1]
	center_freq = 74.0E6

	landmark_freqs -= center_freq


	dense_freqs = np.arange(landmark_freqs[0], landmark_freqs[-1]+0.0001, 10000.0)

	# We want to interpolate the amplitudes to get a smooth landscape, but we also don't want
	# to include every bit of high frequency noise in the amplitude data. Let's start out by smoothing
	# the amplitude data, and then we can interpolate. We will smooth by taking averages of neighboring points.
	smooth_landmark_amps = np.zeros(len(landmark_amps))
	num_to_avg = 3
	for i in range(len(landmark_freqs)):
		val = 0.0

		for neighbor in range(-num_to_avg, num_to_avg+1):
			ind = neighbor + i
			if ind < 0:
				ind = 0
			if ind >= len(landmark_amps):
				ind = len(landmark_amps) - 1

			val += landmark_amps[ind]
		num_avged_samples = 2*num_to_avg + 1.0

		smooth_landmark_amps[i] = val / num_avged_samples



	# Use smoothed landmarks to produce a dense landscape (dense spacing of points).

	if should_smooth:
		rbf = Rbf(landmark_freqs, smooth_landmark_amps, function='quintic')
		residual_amplitudes = landmark_amps - smooth_landmark_amps

	else:
		rbf = Rbf(landmark_freqs, landmark_amps, function='quintic')
		residual_amplitudes = 0.0 * landmark_amps




	dense_amps = rbf(dense_freqs)

	# Also compute residuals.


	# plt.plot(dense_freqs + center_freq, dense_amps)
	# plt.scatter(landmark_freqs + center_freq, landmark_amps)
	# plt.xlabel("Trap Position (MHz)")
	# plt.ylabel("Amplitude")
	# plt.title("Amplitude Landscape")
	# plt.show()

	# plt.scatter(landmark_freqs + center_freq, residual_amplitudes)
	# plt.xlabel("Trap Position (MHz)")
	# plt.ylabel("Residual Amplitudes")
	# plt.title("Residual Amplitudes from smooth landscape")
	# plt.show()

	# Now we have a dense landscape to work with.
	return (dense_freqs, dense_amps, residual_amplitudes)


def amplitude(t, T, amp1, amp2):
	t_less_than_half_T = (np.sign(T/2.0 - t) + 1) / 2
	t_greater_than_T = (np.sign(t - T) + 1) / 2

	less_than_val = amp1 + 2 * (amp2 - amp1) * (t/T) ** 2.0
	greater_than_val = amp2 - (t - T)**2.0 * 2 * (amp2 - amp1) / T**2.0

	res = less_than_val * t_less_than_half_T + greater_than_val * (1 - t_less_than_half_T)

	return res * (1 - t_greater_than_T) + t_greater_than_T * amp2


def amplitudeFromLandscape(t, T, f1, f2, amplitudeLandscape, start_amplitude, end_amplitude):
	freq = instantaneous_freq(t, T, f1, f2)
	ind = np.searchsorted(amplitudeLandscape[0], freq)
	amplitude = amplitudeLandscape[1][ind]

	start_ind = np.searchsorted(amplitudeLandscape[0], f1)
	end_ind = np.searchsorted(amplitudeLandscape[0], f2)

	# Account for residuals?
	starting_residual = start_amplitude - amplitudeLandscape[1][start_ind]
	ending_residual = end_amplitude - amplitudeLandscape[1][end_ind]

	# starting_residual, ending_residual = amplitudeLandscape[2][start_index], amplitudeLandscape[2][end_index]
	# print "starting, ending residuals:", starting_residual, ending_residual

	return amplitude + (starting_residual) + (ending_residual - starting_residual) * (t/T)

	# return amplitude


def find_eps(f1, f2, tau, delta_phi):
    if f1 == f2:
        return 0

    lamb = delta_phi - (f1 + f2)/2.0 * tau
    fractional_part = np.modf(lamb)[0]
    if fractional_part < 0:
        fractional_part += 1.0

    if f2 < f1:
        if (fractional_part > 0):
            fractional_part -= 1

    return fractional_part * 2 / (f2 - f1)

def precomputeExponential():
	num_samples = 1E6

	fractional_phases = np.linspace(0, 1, num_samples, endpoint=False)
	exp = np.exp(2.0j * np.pi * fractional_phases)

	return (num_samples, exp)

def usePrecomputedExponential(phase, precomputedExponential):
	num_samples = precomputedExponential[0]
	exp = precomputedExponential[1]

	phase_2pi = phase / (2.0 * np.pi)
	# Ensure that this fractional_phase is non-negative, without affecting the exponential.
	phase_2pi += 1.0

	phase_indices = (np.modf(phase_2pi)[0] * num_samples).tolist()

	return exp[phase_indices]



def generateWaveform(f1, f2, amp1, amp2, phase1, phase2, tau, amplitudeLandscape=None, precomputedExponential=None):
	eps = find_eps(f1, f2, tau, phase2 - phase1);

	sample_rate = 1.024E9
	sample_times = np.arange(0, tau, 100.0 / sample_rate)

	T = tau - eps
	phases = phase(sample_times, T, f1, f2);

	if amplitudeLandscape==None:
		amplitudes = amplitude(sample_times, T, amp1, amp2)
	else:
		amplitudes = amplitudeFromLandscape(sample_times, T, f1, f2, amplitudeLandscape, amp1, amp2)

	if precomputedExponential != None:
		waveform = usePrecomputedExponential(phases + 2*np.pi*phase1, precomputedExponential)
		vals = amplitudes * waveform

		return vals
	else:
		return amplitudes * np.exp(1j * (phases + 2*np.pi * phase1))


# Calculate waveform to sweep from one freq(amp) to another freq(amp) using a minimal jerk trajectory.
# Don't worry about phase matching at the end of the sweep.
def generateWaveformMinimalJerk(f1, f2, amp1, amp2, phase1, phase2, tau, amplitudeLandscape=None, precomputedExponential=None,
								start_index=0, end_index=0):
	sample_rate = 1.0E8
	sample_times = np.arange(0, tau, 1.0 / sample_rate)

	T = tau

	phases = phaseMinimalJerk(sample_times, T, f1, f2);

	if amplitudeLandscape==None:
		amplitudes = amplitude(sample_times, T, amp1, amp2)
	else:
		amplitudes = amplitudeFromLandscape(sample_times, T, f1, f2, amplitudeLandscape, amp1, amp2)

	if precomputedExponential != None:
		waveform = usePrecomputedExponential(phases + 2*np.pi*phase1, precomputedExponential)
		vals = amplitudes * waveform

		return vals
	else:
		return amplitudes * np.exp(1j * (phases + 2*np.pi * phase1))




def saveWaveform(waveform, filename):
	num_samples = len(waveform)

	f = open(filename, 'wb')
	f.write(struct.pack('I', num_samples))

	real_parts = np.real(waveform)
	imag_parts = np.imag(waveform)

	interwoven_waveform = np.empty(2*num_samples)
	interwoven_waveform[0::2] = real_parts
	interwoven_waveform[1::2] = imag_parts

	f.write(struct.pack('f' * num_samples * 2, *interwoven_waveform));
	f.close()



def read_traps(filename):
	#f = open("/home/control/Desktop/TrapController/DefaultTrapConfigurations/seventy_2016_03_31_optimized_3.txt", "r")
	f = open("../bin/DefaultTrapConfigurations/%s" %filename, "r")
	contents = f.read()
	f.close()

	lines = contents.split("\n")

	# Should be adjusted depending on which set of traps we want to work with, and what center frequency we
	# will be operating with.
	center_freq = 74.0

	freqs = []
	amps = []
	phases = []

	lines = contents.split("\n")
	for line in lines:
		if line.strip() == "":
			continue

		components = line.split(" ")
		freqs.append(round((float(components[0]) - center_freq) * 1.0E6))
		amps.append(float(components[1]))
		phases.append(float(components[2]))


	return freqs, amps, phases

# All traps on, static waveform.
def write_static_waveform(filename, freqs, amps, phases, duration, precomputedExponential=None):
	waveforms = []
	for trap_index in range(len(freqs)):
		waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
									amps[trap_index], amps[trap_index],
									phases[trap_index], phases[trap_index],
									duration, precomputedExponential=precomputedExponential)

		waveforms.append(waveform)

	combined_waveform = np.sum(waveforms, axis=0)

	saveWaveform(combined_waveform, "../bin/StaticWaveforms/%s_static" %filename)


# Keeping left traps static, fading in right traps.
# def write_fade_in_waveforms(freqs, amps, phases, duration, precomputedExponential=None):
# 	waveforms = []
# 	for trap_index in range(len(freqs)):
# 		waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 									amps[trap_index], amps[trap_index],
# 									phases[trap_index], phases[trap_index],
# 									duration, precomputedExponential=precomputedExponential)
# 		waveforms.append(waveform)

# 	static_waveform = np.sum(waveforms, axis=0)


# 	current_waveform = static_waveform
# 	saveWaveform(static_waveform, "fade_in_0_traps_%.1fms_%d" %(duration*1000.0, len(freqs)))

# 	for num_to_fade in range(1, len(freqs) + 1):
# 		# Fade in rightmost trap
# 		trap_index = len(freqs) - num_to_fade
# 		print trap_index
# 		waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 										0.0, amps[trap_index],
# 										phases[trap_index], phases[trap_index],
# 										duration, precomputedExponential=precomputedExponential)

# 		if_it_stayed = generateWaveform(freqs[trap_index], freqs[trap_index],
# 										amps[trap_index], amps[trap_index],
# 										phases[trap_index], phases[trap_index],
# 										duration, precomputedExponential=precomputedExponential)

# 		current_waveform = np.array(waveform) - np.array(if_it_stayed) + np.array(current_waveform)

# 		filename = "fade_in_%d_traps_%.1fms_%d" %(num_to_fade, duration*1000.0, len(freqs))
# 		saveWaveform(current_waveform, filename)

# Keep left traps on, but don't fade in right traps
def write_static_left_traps_waveform(freqs ,amps, phases, duration, precomputedExponential=None):
	waveforms = []

	for num_traps_to_keep in range(1, len(freqs) + 1):
		print "keeping left %d traps" %num_traps_to_keep
		trap_index = num_traps_to_keep - 1
		waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
									amps[trap_index], amps[trap_index],
									phases[trap_index], phases[trap_index],
									duration, precomputedExponential=precomputedExponential)
		waveforms.append(waveform)

		combined_waveform = np.sum(waveforms, axis=0)
		filename = "static_left_%d_traps_%.1fms_%d" %(num_traps_to_keep, duration*1000.0, len(freqs))
		saveWaveform(combined_waveform, filename)


# Computing the difference between a static trap and a fading out trap.
# Plan: start with all traps static waveform, then subtract the "fade_out" difference waveform for
# each trap to fade out.
def write_fade_out_waveforms(configuration_filename, freqs, amps, phases, duration, precomputedExponential=None):
	print "Calculating fade out trajectories...",
	for trap_index in range(len(freqs)):
		static_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
											amps[trap_index], amps[trap_index],
											phases[trap_index], phases[trap_index],
											duration, precomputedExponential=precomputedExponential)

		fade_out_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
											amps[trap_index], 0.0,
											phases[trap_index], phases[trap_index],
											duration, precomputedExponential=precomputedExponential)

		diff_waveform = fade_out_waveform - static_waveform

		filename = "fade_out_%d_trap_difference" %(trap_index)
		directory = "/home/control/Desktop/TrapWaveforms/%s_fade_out_in_%.1fms/" %(configuration_filename, duration*1000.0)
		saveWaveform(diff_waveform, directory + filename)

	print "Done!"

# Computing the difference between a static trap and a fading out trap.
# Plan: start with all traps static waveform, then subtract the "fade_out" difference waveform for
# each trap to fade out.
def write_fade_in_waveforms(configuration_filename, freqs, amps, phases, duration, precomputedExponential=None):
	print "Calculating fade in trajectories...",
	for trap_index in range(len(freqs)):
		static_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
											amps[trap_index], amps[trap_index],
											phases[trap_index], phases[trap_index],
											duration, precomputedExponential=precomputedExponential)

		fade_out_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
											0.0, amps[trap_index],
											phases[trap_index], phases[trap_index],
											duration, precomputedExponential=precomputedExponential)

		diff_waveform = fade_out_waveform - static_waveform

		filename = "fade_in_%d_trap_difference" %(trap_index)
		directory = "/home/control/Desktop/TrapWaveforms/%s_fade_in_in_%.1fms/" %(configuration_filename, duration*1000.0)
		saveWaveform(diff_waveform, directory + filename)

	print "Done!"


# Compute fading from one set of traps to another.
def fade_between_trap_configs(filename1, filename2, duration, precomputedExponential=None):
	print "Calculating fading between %s and %s in %.1fms..." %(filename1, filename2, duration*1000)
	freqs1, amps1, phases1 = read_traps("%s.txt" %filename1)
	freqs2, amps2, phases2 = read_traps("%s.txt" %filename2)


	waveforms = []
	for trap_index in range(len(freqs1)):
		waveform = generateWaveform(freqs1[trap_index], freqs1[trap_index],
									amps1[trap_index], amps2[trap_index],
									phases1[trap_index], phases1[trap_index],
									duration, precomputedExponential=precomputedExponential)

		waveforms.append(waveform)

	combined_waveform = np.sum(waveforms, axis=0)

	saveWaveform(combined_waveform, "/home/control/Desktop/TrapWaveforms/fading_between_configurations/%s_to_%s_in_%.1fms" %(filename1, filename2, duration*1000.0))



# Compute rearrangement waveforms.
def write_rearrangement_waveforms(configuration_filename, freqs, amps, phases, duration,
									amplitudeLandscape = None, startIndex=0, endIndex=-1,
									waveformFunction=generateWaveform):
	if endIndex == -1:
		endIndex = len(freqs)

	for start_index in range(startIndex, endIndex):
		print "Computing rearrangement for start index %d" %start_index
		for end_index in range(len(freqs)):
			waveform = waveformFunction(freqs[start_index], freqs[end_index],
										amps[start_index], amps[end_index],
										phases[start_index], phases[end_index],
										duration, amplitudeLandscape, None, start_index, end_index)

			filename = "rearrange_%d_to_%d" %(start_index, end_index)

			saveWaveform(waveform, "../bin/TrapWaveforms/%s_to_%s_in_%.1fms/%s" %(configuration_filename,
																								configuration_filename,
																								duration*1000.0,
																								filename))


# Compute rearrangement waveforms.
def write_rearrangement_waveforms_between_two_configs(filename_start, filename_end,
                                                        freqs1, amps1, phases1,
                                                        freqs2, amps2, phases2,
                                                        duration,
                                                        amplitudeLandscape = None):

    directory_path = "../bin/TrapWaveforms/%s_to_%s_in_%.1fms" %(filename_start, filename_end, duration*1000.0)
    os.mkdir(directory_path)

    for start_index in range(len(freqs1)):
        print "Computing rearrangement for start index %d" %start_index
        for end_index in range(len(freqs2)):
            waveform = generateWaveform(freqs1[start_index], freqs2[end_index],
                    amps1[start_index], amps2[end_index],
                    phases1[start_index], phases2[end_index],
                    duration, amplitudeLandscape, None)

            filename = "rearrange_%d_to_%d" %(start_index, end_index)

            saveWaveform(waveform, "%s/%s" %(directory_path, filename))



def write_rapidly_modulated_waveform():
	freq1 = 20.0E6
	amp1 = 0.008

	freq2 = 20.5E6
	amp2 = 0.008

	waveform_on_samples = 10000

	# Rapidly alternate between two waveforms.


	waveform1 = generateWaveform(freq1, freq1,
								 amp1, amp1,
								 0, 0,
								 duration)

	waveform2 = generateWaveform(freq2, freq2,
								 amp2, amp2,
								 0, 0,
								 duration)

	final_waveform = np.zeros(len(waveform1), dtype=np.complex_)

	for i in range(len(waveform1) / waveform_on_samples):
		start = waveform_on_samples*i
		end = waveform_on_samples*(i+1)

		if i % 2 == 0:
			final_waveform[start:end] = waveform1[start:end]
		else:
			final_waveform[start:end] = waveform2[start:end]

	saveWaveform(final_waveform, "modulate_20,20.5MHz_trap_%d_samples" %waveform_on_samples)

def write_slow_motion_expansion_waveform(freqs, amps, phases, amplitudeLandscape = None):
	duration = 0.5
	starting_configuration = np.array([0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0,
       0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0,
       1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1,
       0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0,
       1, 0, 0, 0, 1, 1, 0, 1])


	dest = 0

	final_waveform = np.zeros(duration * 1E8, dtype=np.complex_)

	for ind in range(100):
		if starting_configuration[ind] != 1:
			continue
		print "targetIndices.push_back(%d);" %ind
		continue
		start_index = ind
		end_index = dest
		final_waveform += generateWaveform(freqs[start_index], freqs[end_index],
									amps[start_index], amps[end_index],
									phases[start_index], phases[end_index],
									duration, amplitudeLandscape)

		dest += 1

	return
	filename = "slow_rearrangement_%.1fs" %(duration)

	saveWaveform(final_waveform, filename)




filename = str(raw_input("Trapfile: "))
for ending_filename in [
       	filename,
    ]:
    #ending_filename = "70(0.61)"

# filename = "70(0.55)"
    freqs, amps, phases = read_traps("%s.txt" %filename)

    ending_freqs, ending_amps, ending_phases = read_traps("%s.txt" %ending_filename)

    print "Rearranging from %s to %s..." %(filename, ending_filename)


#print freqs

    amplitude_landscape = create_amplitude_landscape("%s.txt" %filename, should_smooth=False);
# # precomputedExponential = precomputeExponential();

    duration = 0.005


    write_static_waveform(ending_filename, ending_freqs, ending_amps, ending_phases, 0.001) #Always should use 1ms duration. # SHOULD RE-ENABLE

    write_rearrangement_waveforms_between_two_configs(filename, ending_filename,
                                                        freqs, amps, phases,
                                                        ending_freqs, ending_amps, ending_phases,
                                                        duration, amplitude_landscape)




# filename1 = "40(0.98)_deep"
# filename2 = "40(0.98)_shallow"
# fading_duration = 0.003
# fade_between_trap_configs(filename1, filename2, fading_duration)
# fade_between_trap_configs(filename2, filename1, fading_duration)






#write_slow_motion_expansion_waveform(freqs, amps, phases, amplitude_landscape)
# write_fade_in_waveforms(filename, freqs, amps, phases, 0.001)
# write_fade_out_waveforms(filename, freqs, amps, phases, 0.001)
#write_rapidly_modulated_waveform();



#Slow rearrangement sample
# duration = 0.4

# atoms_present = []
# for trap_index in range(len(freqs)):
# 	if random.randint(0, 2):
# 		atoms_present.append(True)
# 	else:
# 		atoms_present.append(False)


# print "Computing static waveform."

# waveforms = []
# for trap_index in range(len(freqs)):
# 	print trap_index
# 	waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 								amps[trap_index], amps[trap_index],
# 								phases[trap_index], phases[trap_index],
# 								duration)
# 	waveforms.append(waveform)
# static_waveform = np.sum(waveforms, axis=0)


# print "Computing fade out."
# # Fade out
# waveforms = []
# for trap_index in range(len(freqs)):
# 	print trap_index

# 	if atoms_present[trap_index]:
# 		continue

# 	s_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 										amps[trap_index], amps[trap_index],
# 										phases[trap_index], phases[trap_index],
# 										duration)

# 	fade_out_waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 										amps[trap_index], 0.0,
# 										phases[trap_index], phases[trap_index],
# 										duration)

# 	diff_waveform = fade_out_waveform - s_waveform
# 	waveforms.append(diff_waveform)

# fade_out_waveform = np.sum(waveforms, axis=0) + static_waveform


# print "Computing moving waveforms."
# # Rearrange
# waveforms = []
# dest = 0
# for trap_index in range(len(freqs)):
# 	print trap_index

# 	if not atoms_present[trap_index]:
# 		continue

# 	waveform = generateWaveform(freqs[trap_index], freqs[dest],
# 							amps[trap_index], amps[dest],
# 							phases[trap_index], phases[dest],
# 							duration)

# 	dest += 1
# 	waveforms.append(waveform)

# move_waveform = np.sum(waveforms, axis=0)


# print "Computing fade in."
# # Fade in
# num_loaded = dest
# num_to_fade = len(freqs) - num_loaded

# waveforms = []
# for trap_index in range(len(freqs)):
# 	print trap_index

# 	if trap_index < len(freqs) - num_to_fade:
# 		start_amp = amps[trap_index]
# 	else:
# 		start_amp = 0.0

# 	waveform = generateWaveform(freqs[trap_index], freqs[trap_index],
# 								start_amp, amps[trap_index],
# 								phases[trap_index], phases[trap_index],
# 								duration)
# 	waveforms.append(waveform)

# fade_in_waveform = np.sum(waveforms, axis=0)



# total_waveform = list(static_waveform) + list(fade_out_waveform) + list(move_waveform) + list(fade_in_waveform) + list(static_waveform)
# total_waveform = list(static_waveform) + list(move_waveform) + list(static_waveform)
# total_waveform = np.array(total_waveform)

# print "Computed waveforms."
# filename = "slow_mo_waveform"
# print "Writing to file: " + filename

# print len(total_waveform)
# saveWaveform(total_waveform, filename)

# print "Wrote result to file!"


print "Done!"
