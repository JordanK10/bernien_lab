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


# Calculates an axicon using numpy and show it on the SLM.

import sys, time, math

# Import the SLM Display SDK:
import detect_heds_module_path
from holoeye import slmdisplaysdk

if not slmdisplaysdk.supportNumPy:
    print("Please install numpy to make this example work on your system.")
    sys.exit()

# Import numpy for matrix multiplication:
import numpy as np

# Make some enumerations available locally to avoid too much code:
ErrorCode = slmdisplaysdk.SLMDisplay.ErrorCode
ShowFlags = slmdisplaysdk.SLMDisplay.ShowFlags

# Detect SLMs and open a window on the selected SLM:
slm = slmdisplaysdk.SLMDisplay()

# Open the SLM preview window (might have an impact on performance):
slm.utilsSLMPreviewShow()

# Configure the axicon properties:
innerRadius = slm.height_px / 3
centerX = 0
centerY = 0

# Calculate the phase values of an axicon in a pixel-wise matrix:

# pre-calc. helper variables:
phaseModulation = 2*math.pi
dataWidth =  slm.width_px
dataHeight = slm.height_px

x = np.linspace(1, dataWidth, dataWidth, dtype=np.float32) - np.float32(dataWidth/2) - np.float32(centerX)
y = np.linspace(1, dataHeight, dataHeight, dtype=np.float32) - np.float32(dataHeight/2) - np.float32(centerY)

x2 = np.matrix(x*x)
y2 = np.matrix(y*y).transpose()

phaseData = np.float32(phaseModulation) * np.sqrt(np.array((np.dot(np.ones([dataHeight, 1], np.float32), x2) + np.dot(y2, np.ones([1, dataWidth], np.float32))), dtype=np.float32), dtype=np.float32) / np.float32(innerRadius)

# Show data on the SLM:
error = slm.showPhasevalues(phaseData)

assert error == ErrorCode.NoError, slm.errorString(error)

# If your IDE terminates the python interpreter process after the script is finished, the SLM content
# will be lost as soon as the script finishes.

# You may insert further code here.

# Wait a few seconds:
time.sleep(4)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()