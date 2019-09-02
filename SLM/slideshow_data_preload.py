# -*- coding: utf-8 -*-

#--------------------------------------------------------------------#
#                                                                    #
# Copyright (C) 2018 HOLOEYE Photonics AG. All rights reserved.      #
# Contact: https://holoeye.com/contact/                              #
#                                                                    #
# This file is part of HOLOEYE SLM Display SDK.                      #
#                                                                    #
# You may use this file under the terms and conditions of the        #
# "HOLOEYE SLM Display SDK Standard License v1.0" license agreement. #
#                                                                    #
#--------------------------------------------------------------------#


# Plays a slideshow on the SLM with pre-calculated 2d phase fields consisting of vertical blazed gratings with different periods.
# The data fields are pre-calculated and uploaded to the GPU once, and then each frame is shown on the SLM by selecting the
# appropriate phase values field on the GPU directly to reach higher performance.

import sys, time, math

# Import the SLM Display SDK:
import detect_heds_module_path
from holoeye import slmdisplaysdk

# Import helper function to print timing statistics of the display duration of the handles:
from slideshow_preload_print_stats import printStat

if not slmdisplaysdk.supportNumPy:
    print("Please install numpy to make this example work on your system.")
    sys.exit()

# Import numpy:
import numpy as np

# Make some enumerations available locally to avoid too much code:
ErrorCode = slmdisplaysdk.SLMDisplay.ErrorCode
ShowFlags = slmdisplaysdk.SLMDisplay.ShowFlags
State = slmdisplaysdk.SLMDisplay.State

# Detect SLMs and open a window on the selected SLM:
slm = slmdisplaysdk.SLMDisplay()

# Open the SLM preview window (might have an impact on performance):
slm.utilsSLMPreviewShow()

# Configure slideshow (steer the laser beam from left to right and back):
gratingPeriodMin = 8
gratingPeriodMax = 64
gratingPeriodStepSize = 4
dataDisplayDurationMilliSec = 100  # duration of each data frame in ms
repeatSlideshow = 3  # <= 0 (e. g. -1) repeats until Python process gets killed

gratingPeriodList = np.arange(-gratingPeriodMin, -(gratingPeriodMax+1), -gratingPeriodStepSize)
gratingPeriodList = np.concatenate((gratingPeriodList, np.arange(gratingPeriodMax, gratingPeriodMin-1, -gratingPeriodStepSize)))
gratingPeriodList = np.concatenate((gratingPeriodList, np.arange(gratingPeriodMin, gratingPeriodMax+1, gratingPeriodStepSize)))
gratingPeriodList = np.concatenate((gratingPeriodList, np.arange(-gratingPeriodMax, -(gratingPeriodMin-1), gratingPeriodStepSize)))

print("gratingPeriodList = " + str(gratingPeriodList))
print("len(gratingPeriodList) = " + str(len(gratingPeriodList)))


# Calculate the data we want to show:
print("Calculating data ...")
start_time = time.time()

# Pre-calculate the phase fields in full SLM size:
phaseModulation = 2.0*math.pi  # radian
dataWidth = slm.width_px
dataHeight = slm.height_px # Can be set to 1 to have a faster calculation in case of vertical blazed gratings.
print("dataWidth = " + str(dataWidth))
print("dataHeight = " + str(dataHeight))

durationInFrames = int((float(dataDisplayDurationMilliSec)/1000.0) * slm.refreshrate_hz)
if durationInFrames <= 0:
    durationInFrames = 1  # The minimum duration is one video frame of the SLM

print("slm.refreshrate_hz = " + str(slm.refreshrate_hz))
print("durationInFrames = " + str(durationInFrames))


dataHandles = []
calcPercent = -1

nHandles = 0  # total number of images loaded to GPU
for blazePeriod in gratingPeriodList:

    # Print progress:
    percent = int(float(nHandles) / len(gratingPeriodList) * 100)
    if int(percent / 5) > calcPercent:
        calcPercent = int(percent / 5)
        print(str(percent) + "%")

    # Calculate next phase field:

    row = np.matrix(np.zeros([1, dataWidth], dtype=np.float32))
    for x in range(dataWidth):
        row[0, x] = float(phaseModulation * (x - dataWidth / 2) / blazePeriod)

    phaseData = np.array(np.matrix(np.ones([dataHeight, 1], dtype=np.float32)) * row, dtype=np.float32)

#  Slow, but general version:
#     phaseData = holoeye.createFieldSingle(dataWidth, dataHeight)
#     for y in range(dataHeight):
#         row = phaseData[y]
#         for x in range(dataWidth):
#             row[x] = float(phaseModulation * (x - dataWidth / 2) / blazePeriod)

    # Load data to GPU using automatic wrapping and values in radian:
    error, handle = slm.loadPhasevalues(phaseData)

    if error == ErrorCode.OutOfVideoMemory:
        print("No video memory left at " + str(nHandles) + ". Skipping the rest")
        break

    slm.datahandleSetDuration(handle, durationInFrames)

    assert error == ErrorCode.NoError, slm.errorString(error)

    nHandles += 1
    dataHandles.append(handle)

print("100%")
end_time = time.time()
print("Calculation took "+ str("%0.3f" % (end_time - start_time)) +" seconds\n")

# Show the pre-calculated data:
print("Showing data...")

# Play complete slideshow:
n = 0
while (n < repeatSlideshow) or (repeatSlideshow <= 0):
    n += 1

    print("Show data for the " + str(n) + ". time ...")

    # Play slideshow once:
    for handle in dataHandles:
        error = slm.showDatahandle(handle, ShowFlags.PresentAutomatic)
        assert error == ErrorCode.NoError, slm.errorString(error)

    # Update the handles to the latest state:
    for handle in dataHandles:
        error = slm.updateDatahandle(handle)
        assert error == ErrorCode.NoError, slm.errorString(error)

    # Print the actual statistics (last data handle has wrong visible time before any other data was shown):
    print("Showing timing statistics...")
    printStat("loadingTimeMs", dataHandles[0:-1])
    printStat("conversionTimeMs", dataHandles[0:-1])
    printStat("processingTimeMs", dataHandles[0:-1])
    printStat("transferTimeMs", dataHandles[0:-1])
    printStat("renderTimeMs", dataHandles[0:-1])
    printStat("visibleTimeMs", dataHandles[0:-1])

# One last image to clear the SLM screen after the slideshow playback:
# (Also possible by just calling slm.showBlankscreen(128))
data = slmdisplaysdk.createFieldUChar(1, 1)
data[0, 0] = 128
error, dh = slm.loadData(data)
assert error == ErrorCode.NoError, slm.errorString(error)

error = slm.showDatahandle(dh, ShowFlags.PresentAutomatic)
assert error == ErrorCode.NoError, slm.errorString(error)

# Release handles and their data to free up video memory:
dataHandles = None

# Wait a few seconds after the slideshow has finished:
time.sleep(4)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()