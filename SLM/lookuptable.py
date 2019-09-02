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


# Shows a 2d matrix of grayscale data with data type float (single) on the SLM.
# The gray values have a range from 0 to 1, non-fitting values will be cropped.
# In this example, a lookup table is applied to the data before it is actually
# shown on the SLM.

import time

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
blazePeriod = 55
lookupTableSize = 256

# Reserve memory for the data:
dataWidth = slm.width_px
dataHeight = slm.height_px
data = slmdisplaysdk.createFieldSingle(dataWidth, dataHeight)

# Calculate the data:
for y in range(dataHeight):
    row = data[y]

    for x in range(dataWidth):
        row[x] = float(x % blazePeriod) / blazePeriod

# Load the data onto the SLM
error, dataHandle = slm.loadData(data)

assert error == ErrorCode.NoError, slm.errorString(error)

# Generate the lookup tables
lookupTable = slmdisplaysdk.createFieldUChar(lookupTableSize, 1)
lookupTableInv = slmdisplaysdk.createFieldUChar(lookupTableSize, 1)

lookupTableRow = lookupTable[0]
lookupTableInvRow = lookupTableInv[0]
for x in range(lookupTableSize):
    lookupTableRow[x] = x
    lookupTableInvRow[x] = lookupTableSize - x

# Load the lookup tables onto the SLM
error2, lookupTableHandle = slm.loadData(lookupTable)
error3, lookupTableInvHandle = slm.loadData(lookupTableInv)

assert error2 == ErrorCode.NoError, slm.errorString(error2)
assert error3 == ErrorCode.NoError, slm.errorString(error3)

# Show data on SLM:
error = slm.showDatahandle(dataHandle)

assert error == ErrorCode.NoError, slm.errorString(error)

# Assign the lookup table
for x in range(10):
    time.sleep(1)
  
    if (x % 2) == 1:
        error = slm.datahandleSetLookupTable(dataHandle, lookupTableHandle)
    else:
        error = slm.datahandleSetLookupTable(dataHandle, lookupTableInvHandle)

    assert error == ErrorCode.NoError, slm.errorString(error)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()
