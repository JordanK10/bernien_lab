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


# Plays a slideshow on the SLM using image files from a single folder.
# The image files are shown directly on the SLM as soon as the data was transmitted,
# using the API function showDataFromFile().
# The duration each image is shown can be configured and is maintained by simple sleep commands.
# For holograms, please use image formats which are uncompressed (e.g. BMP) or which use lossless compression, like PNG.

import os, sys, time, math

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

# Configure slideshow:
imageFolder = "C:/Users/Bernien Lab/desktop/DOEs"  # please enter a valid folder path
imageDisplayDurationMilliSec = 500  # please select the duration in ms each image file shall be shown on the SLM
repeatSlideshow = 3  # <= 0 (e. g. -1) repeats until Python process gets killed

# Please select how to scale and transform image files while displaying:
displayOptions = ShowFlags.PresentAutomatic  # PresentAutomatic == 0 (default)
#displayOptions |= ShowFlags.TransposeData
displayOptions |= ShowFlags.PresentTiledCentered  # This makes much sense for holographic images
#displayOptions |= ShowFlags.PresentFitWithBars
#displayOptions |= ShowFlags.PresentFitNoBars
#displayOptions |= ShowFlags.PresentFitScreen

# Search image files in given folder:
filesList = os.listdir(imageFolder)

# Filter *.png, *.bmp, *.gif, and *.jpg files:
imagesList = [filename for filename in filesList if str(filename).endswith(".png") or str(filename).endswith(".gif") or str(filename).endswith(".bmp") or str(filename).endswith(".jpg")]

print(imagesList)

print("Number of images found in imageFolder = " + str(len(imagesList)))

if len(imagesList) <= 0:
    sys.exit()

# Save the start time:
avgFPSStartTime = time.time()
currentFPSStartTime = avgFPSStartTime

# Play complete slideshow:
n = 0
while (n < repeatSlideshow) or (repeatSlideshow <= 0):
    n += 1

    #Play slideshow once:
    for i, filename in enumerate(imagesList):
        # Measure time between images and duration of current image:
        currentFPSStartTime = time.time()

        filepath = os.path.join(imageFolder, filename)

        # Show image on SLM. Function returns after image was loaded and displayed:
        error = slm.showDataFromFile(filepath, displayOptions)
        assert error == ErrorCode.NoError, slm.errorString(error)
        passedTime = time.time() - currentFPSStartTime

        # Wait for next image. Wait rest of desired time after slm.showDataFromFile() took some time already:
        sleep_duration = float(imageDisplayDurationMilliSec)/1000.0 - passedTime
        if (sleep_duration > 0.0):
            time.sleep(sleep_duration)

        outputStr = "n = " + str("%3d" % n)

        # Show actual current frames per seconds:
        delta_t = time.time() - currentFPSStartTime
        fps = 1.0 / delta_t
        outputStr += "   FPS = " + str("%5.2f" % fps)

        outputStr += "   file number = " + str("%5d" % (i+1))
        outputStr += "   file name: " + str(filename)

        print(outputStr)

# Show a blank screen with gray value 128 as last image:
error = slm.showBlankscreen(128)
assert error == ErrorCode.NoError, slm.errorString(error)

# Wait a few seconds after the slideshow has finished:
time.sleep(4)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()
