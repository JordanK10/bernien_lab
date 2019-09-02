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


# Shows a 1d vector of phase values with data type float (single) on the SLM.
# The phase values have a range from 0 to 2pi, non-fitting values will be wrapped automatically on the GPU.
# We use the show-flags to replicate (tile) the 1d vector to the full SLM size.

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

# Calculate e.g. a horizontal blazed grating column:
blazePeriod = 77

phaseModulation = 2.0 * math.pi  # radian
dataWidth = blazePeriod
dataHeight = 77
phaseData = slmdisplaysdk.createFieldSingle(dataWidth, dataHeight)    # this is to write phase data onto the slm

# Calculate phase data. The values are calculated in unit radian without any wrapping:
dataRow = phaseData[0]
for (x) in (range(dataWidth)):
    dataRow[x] = float((phaseModulation * x / blazePeriod)*math.sin(x))

# Show phase data on the SLM:
error = slm.showPhasevalues(phaseData, ShowFlags.PresentTiledCentered)

assert error == ErrorCode.NoError, slm.errorString(error)

# If your IDE terminates the python interpreter process after the script is finished, the SLM content
# will be lost as soon as the script finishes.

# You may insert further code here.

# Wait a few seconds:
time.sleep(10)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()
