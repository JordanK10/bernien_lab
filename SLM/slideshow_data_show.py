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


# Plays a slideshow on the SLM with live-calculated 1d float (single) data fields
# consisting of vertical blazed gratings with different periods.
# Each frame is shown on the SLM as soon as the data was transmitted.

import time, math

# Import the SLM Display SDK:
import detect_heds_module_path
from holoeye import slmdisplaysdk

# Make some enumerations available locally to avoid too much code:
ErrorCode = slmdisplaysdk.SLMDisplay.ErrorCode
ShowFlags = slmdisplaysdk.SLMDisplay.ShowFlags

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

gratingPeriodList = []
gratingPeriodList += range(-gratingPeriodMin, -(gratingPeriodMax+1), -gratingPeriodStepSize)
gratingPeriodList += range(gratingPeriodMax, gratingPeriodMin-1, -gratingPeriodStepSize)
gratingPeriodList += range(gratingPeriodMin, gratingPeriodMax+1, gratingPeriodStepSize)
gratingPeriodList += range(-gratingPeriodMax, -(gratingPeriodMin-1), gratingPeriodStepSize)

print("gratingPeriodList = " + str(gratingPeriodList))
print("len(gratingPeriodList) = " + str(len(gratingPeriodList)))


dataWidth = slm.width_px

# Save the start time:
avgFPSStartTime = time.time()

# Prepare data field:
blazeData = slmdisplaysdk.createFieldSingle(dataWidth, 1)
row = blazeData[0]
phaseModulation = 2.0*math.pi  # radian

# Play slideshow:
n = 0
while (n < repeatSlideshow) or (repeatSlideshow <= 0):
    n += 1
    for blazePeriod in gratingPeriodList:

        currentFPSStartTime = time.time()

        # Calculate data:
        for x in range(dataWidth):
            row[x] = float( phaseModulation * (x - dataWidth/2) / blazePeriod)

        outputStr = "n = "+str("%3d" % n) + ": blazePeriod = "+str("%7d" % blazePeriod)

        # Show data on SLM:
        error = slm.showPhasevalues(blazeData)
        assert error == ErrorCode.NoError, slm.errorString(error)
        passedTime = time.time() - currentFPSStartTime

        # Wait for next image. Wait rest of desired time after slm.showDataFromFile() took some time already:
        sleep_duration = float(dataDisplayDurationMilliSec) / 1000.0 - passedTime
        if (sleep_duration > 0.0):
            time.sleep(sleep_duration)

        # Show actual current frames per seconds:
        delta_t = time.time() - currentFPSStartTime
        fps = 1.0 / delta_t
        outputStr += "   FPS = " + str("%5.1f" % fps)

        print(outputStr)

# Show average frames per seconds:
print('--- Average FPS = %6.2f ---' % (float(repeatSlideshow * len(gratingPeriodList)) / (time.time() - avgFPSStartTime)))

# Show a blank screen with gray value 128 as last image:
error = slm.showBlankscreen(128)
assert error == ErrorCode.NoError, slm.errorString(error)

# Wait a few seconds after the slideshow has finished:
time.sleep(4)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()
