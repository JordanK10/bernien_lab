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


# Shows a 2d matrix of phase values with data type float (single) on the SLM.
# The phase values have a range from 0 to 2pi, non-fitting values will be wrapped automatically on the GPU.

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

# Calculate e.g. a vertical blazed grating:
blazePeriod = 77

phaseModulation = 2.0 * math.pi  # radian
dataWidth = slm.width_px
dataHeight = slm.height_px
phaseData = slmdisplaysdk.createFieldSingle(dataWidth, dataHeight)

# Calculate phase data. The values are calculated in unit radian without any wrapping:
for y in range(dataHeight):
    row = phaseData[y]

    for x in range(dataWidth):

        row[x] = float(phaseModulation * x / blazePeriod)
        print(row[x])
# Show phase data on SLM:
error = slm.showPhasevalues(phaseData)

assert error == ErrorCode.NoError, slm.errorString(error)

# If your IDE terminates the python interpreter process after the script is finished, the SLM content
# will be lost as soon as the script finishes.

# You may insert further code here.

# Wait a few seconds:
time.sleep(4)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()
