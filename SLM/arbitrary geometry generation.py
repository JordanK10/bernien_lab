from scipy.optimize import curve_fit
import PIL
import numpy as np
import matplotlib.pyplot as plt
from skimage import util
import scipy
import cv2
import UC480
import math
import os, time
import UC480
import pandas as pd
# Import the SLM Display SDK:
import detect_heds_module_path
from holoeye import slmdisplaysdk
import random
from PIL import Image
from collections import OrderedDict

# The code from SLM SDK
# Loads a grayscale image file and shows it on the SLM.
# activate the SLM

# Import the SLM Display SDK:
import detect_heds_module_path
from holoeye import slmdisplaysdk

#initializing the UC480 platform and Detect SLMs and open a window on the selected SLM:
try:
    cam = UC480.uc480()
    cam.connect_to_library()
    cam.connect()
except UC480.uc480Error:

    time.sleep(1)

    cam = UC480.uc480()
    cam.connect_to_library()
    cam.connect()

ErrorCode = slmdisplaysdk.SLMDisplay.ErrorCode
ShowFlags = slmdisplaysdk.SLMDisplay.ShowFlags
slm = slmdisplaysdk.SLMDisplay()

def arbitrary():
    (X, Y) = (1080, 1080)  # slm dimensions
    array_image = np.zeros((X, Y))
    d = -30

    for i in range(40, 80, 8):
        for j in range(0, 20, 4):
            array_image[j][i] = 1
            array_image[-j][i] = 1

    return array_image

def arbitraryj():
    (X, Y) = (1080, 1080)  # slm dimensions
    array_image = np.zeros((X, Y))
    d = 60
    a = 4



    array_image[0][0+d] = 1
    array_image[0][-1*a+d] = 1
    array_image[0][-2*a+d] = 1
    array_image[0][-3*a+d] = 1
    array_image[0][-4*a+d] = 1
    array_image[0][-5*a+d] = 1
    array_image[0*a][-6*a+d] = 1
    array_image[0*a][-7*a+d] = 1
    array_image[-1*a][-9*a+d] = 1
    array_image[-2*a][-10*a+d] = 1
    array_image[-3*a][-11*a+d] = 1
    array_image[1*a][-9*a+d] = 1
    array_image[2*a][-10*a+d] = 1
    array_image[3*a][-11*a+d] = 1
    array_image[-1*a][-3*a+d] = 1
    array_image[-2*a][-4*a+d] = 1
    array_image[-3*a][-3*a+d] = 1
    array_image[-4*a][-3*a+d] = 1
    array_image[1*a][-3*a+d] = 1
    array_image[2*a][-4*a+d] = 1
    array_image[3*a][-3*a+d] = 1
    array_image[4*a][-3*a+d] = 1
    array_image[1*a][-1*a+d] = 1
    array_image[2*a][0*a+d] = 1
    array_image[1*a][1*a+d] = 1
    array_image[0*a][2*a+d] = 1
    array_image[-1*a][-1*a+d] = 1
    array_image[-2*a][0*a+d] = 1
    array_image[-1*a][1*a+d] = 1


    return array_image

def arbitraryq():
    (X, Y) = (1080, 1080)  # slm dimensions
    array_image = np.zeros((X, Y))
    s = 4
    d = 20
    # B
    array_image[0][40- d] = 1
    array_image[s][40- d] = 1
    array_image[2 * s][40- d] = 1
    array_image[-s][40- d] = 1
    array_image[-2 * s][40- d] = 1
    array_image[0][44- d] = 1
    array_image[2 * s][44- d] = 1
    array_image[-2 * s][44- d] = 1
    array_image[7][48- d] = 1
    array_image[-7][48- d] = 1
    array_image[4][49- d] = 1
    array_image[-4][49- d] = 1
    array_image[-2][47- d] = 1
    array_image[2][47- d] = 1

    # E
    array_image[0][55- d] = 1
    array_image[4][55- d] = 1
    array_image[8][55- d] = 1
    array_image[-4][55- d] = 1
    array_image[-8][55- d] = 1
    array_image[0][59- d] = 1
    array_image[8][59- d] = 1
    array_image[-8][59- d] = 1
    array_image[-8][63- d] = 1
    array_image[8][63- d] = 1
    array_image[0][63- d] = 1
    array_image[8][67- d] = 1
    array_image[-8][67- d] = 1

    # R
    array_image[0][73- d] = 1
    array_image[4][73- d] = 1
    array_image[8][73- d] = 1
    array_image[-4][73- d] = 1
    array_image[-8][73- d] = 1
    array_image[-8][77- d] = 1
    array_image[4][79- d] = 1
    array_image[6][81- d] = 1
    array_image[8][83- d] = 1
    array_image[0][77- d] = 1
    array_image[-2][81- d] = 1
    array_image[-8][81- d] = 1
    array_image[-5][83- d] = 1



    # N
    array_image[0][89- d] = 1
    array_image[4][89- d] = 1
    array_image[8][89- d] = 1
    array_image[-4][89- d] = 1
    array_image[-8][89- d] = 1
    array_image[0][103- d] = 1
    array_image[4][103- d] = 1
    array_image[8][103- d] = 1
    array_image[-4][103- d] = 1
    array_image[-8][103- d] = 1

    array_image[-6][92- d] = 1
    array_image[-2][95- d] = 1
    array_image[2][98- d] = 1
    array_image[6][100- d] = 1

    # i
    array_image[0][115 - 1- d] = 1
    array_image[4][115 - 1- d] = 1
    array_image[8][115 - 1- d] = 1
    array_image[-8][115 - 1- d] = 1
    array_image[-4][115 - 1- d] = 1
    array_image[-8][112 - 1- d] = 1
    array_image[-8][109 - 1- d] = 1
    array_image[-8][118 - 1- d] = 1
    array_image[-8][121 - 1- d] = 1
    array_image[8][112 - 1- d] = 1
    array_image[8][109 - 1- d] = 1
    array_image[8][118 - 1- d] = 1
    array_image[8][121 - 1- d] = 1

    # E
    array_image[0][125- d] = 1
    array_image[4][125- d] = 1
    array_image[8][125- d] = 1
    array_image[-4][125- d] = 1
    array_image[-8][125- d] = 1
    array_image[0][129- d] = 1
    array_image[8][129- d] = 1
    array_image[-8][129- d] = 1
    array_image[-8][129- d] = 1
    array_image[8][129- d] = 1
    array_image[0][133- d] = 1
    array_image[8][133- d] = 1
    array_image[-8][133- d] = 1
    array_image[-8][136+1- d] = 1
    array_image[8][136+1- d] = 1

    # N
    array_image[0][139+1+2- d] = 1
    array_image[4][139+1+2- d] = 1
    array_image[8][139+1+2- d] = 1
    array_image[-4][139+1+2- d] = 1
    array_image[-8][139+1+2- d] = 1
    array_image[0][154+1+2- d] = 1
    array_image[4][154+1+2- d] = 1
    array_image[8][154+1+2- d] = 1
    array_image[-4][154+1+2- d] = 1
    array_image[-8][154+1+2- d] = 1

    array_image[-6][142+1+1+1- d] = 1
    array_image[-2][145+1+1+1- d] = 1
    array_image[2][148+1+1+1- d] = 1
    array_image[6][151+1+1+1- d] = 1

    # L

    array_image[0+d][167- d-127] = 1
    array_image[4+d][167- d-127] = 1
    array_image[-4+d][167- d-127] = 1
    array_image[8+d][167- d-127] = 1
    array_image[-8+d][167- d-127] = 1
    array_image[8+d][171- d-1-127] = 1
    array_image[8+d][175- d-1-1-127] = 1
    array_image[8+d][179- d-1-1-1-127] = 1

    # A

    array_image[8+d][185- d-1-1-1-127] = 1
    array_image[4+d][187- d-1-1-1-127] = 1
    array_image[0+d][189- d-1-1-1-127] = 1
    array_image[-4+d][191- d-1-1-1-127] = 1
    array_image[-8+d][193- d-1-1-1-127] = 1
    array_image[0+d][193- d-1-1-1-127] = 1
    array_image[-4+d][195- d-1-1-1-127] = 1
    array_image[0+d][197- d-1-1-1-127] = 1
    array_image[4+d][199- d-1-1-1-127] = 1
    array_image[8+d][201- d-1-1-1-127] = 1

    # A

    array_image[-8-d][185- d-1-1-1-127 + 20] = 1
    array_image[-4-d][187- d-1-1-1-127+ 20] = 1
    array_image[0-d][189- d-1-1-1-127+ 20] = 1
    array_image[4-d][191- d-1-1-1-127+ 20] = 1
    array_image[8-d][193- d-1-1-1-127+ 20] = 1
    array_image[0-d][193- d-1-1-1-127+ 20] = 1
    array_image[4-d][195- d-1-1-1-127+ 20] = 1
    array_image[0-d][197- d-1-1-1-127+ 20] = 1
    array_image[4-d][199- d-1-1-1-127+ 20] = 1
    array_image[-8-d][201- d-1-1-1-127+ 20] = 1

    # A

    array_image[-8-d][185- d-1-1-1-127 + 40] = 1
    array_image[-4-d][187- d-1-1-1-127+ 40] = 1
    array_image[0-d][189- d-1-1-1-127+ 40] = 1
    array_image[4-d][191- d-1-1-1-127+ 40] = 1
    array_image[8-d][193- d-1-1-1-127+ 40] = 1
    array_image[0-d][193- d-1-1-1-127+ 40] = 1
    array_image[4-d][195- d-1-1-1-127+ 40] = 1
    array_image[0-d][197- d-1-1-1-127+ 40] = 1
    array_image[4-d][199- d-1-1-1-127+ 40] = 1
    array_image[-8-d][201- d-1-1-1-127+ 40] = 1

    # # B
    # array_image[0+d][207- d-1-1-1-127] = 1
    # array_image[s+d][207- d-1-1-1-127] = 1
    # array_image[2 * s+d][207- d-1-1-1-127] = 1
    # array_image[-s+d][207- d-1-1-1-127] = 1
    # array_image[-2 * s+d][207- d-1-1-1-127] = 1
    # array_image[0+d][211- d-1-1-1-127] = 1
    # array_image[2 * s+d][211- d-1-1-1-127] = 1
    # array_image[-2 * s+d][211- d-1-1-1-127] = 1
    # array_image[7+d][215- d-1-1-1-127] = 1
    # array_image[-7+d][215- d-1-1-1-127] = 1
    # array_image[4+d][216- d-1-1-1-127] = 1
    # array_image[-4+d][216- d-1-1-1-127] = 1
    # array_image[-2+d][214- d-1-1-1-127] = 1
    # array_image[2+d][214- d-1-1-1-127] = 1

    # # B
    # array_image[0+d][207- d-1-1-1-127+20] = 1
    # array_image[s+d][207- d-1-1-1-127+20] = 1
    # array_image[2 * s+d][207- d-1-1-1-127+20] = 1
    # array_image[-s+d][207- d-1-1-1-127+20] = 1
    # array_image[-2 * s+d][207- d-1-1-1-127+20] = 1
    # array_image[0+d][211- d-1-1-1-127+20] = 1
    # array_image[2 * s+d][211- d-1-1-1-127+20] = 1
    # array_image[-2 * s+d][211- d-1-1-1-127+20] = 1
    # array_image[7+d][215- d-1-1-1-127+20] = 1
    # array_image[-7+d][215- d-1-1-1-127+20] = 1
    # array_image[4+d][216- d-1-1-1-127+20] = 1
    # array_image[-4+d][216- d-1-1-1-127+20] = 1
    # array_image[-2+d][214- d-1-1-1-127+20] = 1
    # array_image[2+d][214- d-1-1-1-127+20] = 1

    # B
    array_image[0+d][207- d-1-1-1-127+70] = 1
    array_image[s+d][207- d-1-1-1-127+70] = 1
    array_image[2 * s+d][207- d-1-1-1-127+70] = 1
    array_image[-s+d][207- d-1-1-1-127+70] = 1
    array_image[-2 * s+d][207- d-1-1-1-127+70] = 1
    array_image[0+d][211- d-1-1-1-127+70] = 1
    array_image[2 * s+d][211- d-1-1-1-127+70] = 1
    array_image[-2 * s+d][211- d-1-1-1-127+70] = 1
    array_image[7+d][215- d-1-1-1-127+70] = 1
    array_image[-7+d][215- d-1-1-1-127+70] = 1
    array_image[4+d][216- d-1-1-1-127+70] = 1
    array_image[-4+d][216- d-1-1-1-127+70] = 1
    array_image[-2+d][214- d-1-1-1-127+70] = 1
    array_image[2+d][214- d-1-1-1-127+70] = 1

    return array_image

def arbitrarye():
    (X, Y) = (1080, 1080)  # slm dimensions
    array_image = np.zeros((X, Y))
    d = 12
    u = -0
    a = 4
    o = 3

    array_image[0][40] = 1
    array_image[0][48] = 1
    array_image[4][42] = 1
    array_image[-4][42] = 1
    array_image[-4][46] = 1
    array_image[4][46] = 1

    array_image[0+u][40+d] = 1
    array_image[0+u][48+d] = 1
    array_image[4+u][42+d] = 1
    array_image[-4+u][42+d] = 1
    array_image[-4+u][46+d] = 1
    array_image[4+u][46+d] = 1

    array_image[0 ][40+2*d] = 1
    array_image[0][48+2*d] = 1
    array_image[4 ][42+2*d] = 1
    array_image[-4 ][42+2*d] = 1
    array_image[-4 ][46+2*d] = 1
    array_image[4][46+2*d] = 1

    array_image[0+u][40+3*d] = 1
    array_image[0+u][48+3*d] = 1
    array_image[4+u][42+3*d] = 1
    array_image[-4+u][42+3*d] = 1
    array_image[-4+u][46+3*d] = 1
    array_image[4+u][46+3*d] = 1

    array_image[0 ][40+4*d] = 1
    array_image[0 ][48+4*d] = 1
    array_image[4 ][42+4*d] = 1
    array_image[-4 ][42+4*d] = 1
    array_image[-4 ][46+4*d] = 1
    array_image[4 ][46+4*d] = 1

    # array_image[0 ][40+5*d] = 1
    # array_image[0 ][48+5*d] = 1
    # array_image[4 ][42+5*d] = 1
    # array_image[-4 ][42+5*d] = 1
    # array_image[-4 ][46+5*d] = 1
    # array_image[4 ][46+5*d] = 1
    #
    # array_image[0 ][40+6*d] = 1
    # array_image[0 ][48+6*d] = 1
    # array_image[4 ][42+6*d] = 1
    # array_image[-4 ][42+6*d] = 1
    # array_image[-4 ][46+6*d] = 1
    # array_image[4 ][46+6*d] = 1

    # second
    array_image[0+d-a][40] = 1
    array_image[0+d-a][48] = 1
    array_image[4+d-a][42] = 1
    array_image[-4+d -a][42] = 1
    array_image[-4+d-a][46] = 1
    array_image[4+d-a][46] = 1

    array_image[0+u+d-a][40+d] = 1
    array_image[0+u+d-a][48+d] = 1
    array_image[4+u+d-a][42+d] = 1
    array_image[-4+u+d-a][42+d] = 1
    array_image[-4+u+d-a][46+d] = 1
    array_image[4+u+d-a][46+d] = 1

    array_image[0 +d-a][40+2*d] = 1
    array_image[0+d-a][48+2*d] = 1
    array_image[4 +d-a][42+2*d] = 1
    array_image[-4 +d-a][42+2*d] = 1
    array_image[-4 +d-a][46+2*d] = 1
    array_image[4+d-a][46+2*d] = 1

    array_image[0+u+d-a][40+3*d] = 1
    array_image[0+u+d-a][48+3*d] = 1
    array_image[4+u+d-a][42+3*d] = 1
    array_image[-4+u+d-a][42+3*d] = 1
    array_image[-4+u+d-a][46+3*d] = 1
    array_image[4+u+d-a][46+3*d] = 1

    array_image[0+d-a ][40+4*d] = 1
    array_image[0+d -a][48+4*d] = 1
    array_image[4 +d-a][42+4*d] = 1
    array_image[-4 +d-a][42+4*d] = 1
    array_image[-4+d -a][46+4*d] = 1
    array_image[4 +d-a][46+4*d] = 1

    # array_image[0+d-a ][40+5*d] = 1
    # array_image[0+d -a][48+5*d] = 1
    # array_image[4 +d-a][42+5*d] = 1
    # array_image[-4 +d-a][42+5*d] = 1
    # array_image[-4+d -a][46+5*d] = 1
    # array_image[4 +d-a][46+5*d] = 1
    #
    # array_image[0+d-a ][40+6*d] = 1
    # array_image[0+d -a][48+6*d] = 1
    # array_image[4 +d-a][42+6*d] = 1
    # array_image[-4 +d-a][42+6*d] = 1
    # array_image[-4+d -a][46+6*d] = 1
    # array_image[4 +d-a][46+6*d] = 1


    #third

    array_image[0-d + a][40] = 1
    array_image[0-d+ a][48] = 1
    array_image[4-d+ a][42] = 1
    array_image[-4-d+ a][42] = 1
    array_image[-4-d+ a][46] = 1
    array_image[4-d+ a][46] = 1

    array_image[0+u-d+ a][40+d] = 1
    array_image[0+u-d+ a][48+d] = 1
    array_image[4+u-d+ a][42+d] = 1
    array_image[-4+u-d+ a][42+d] = 1
    array_image[-4+u-d+ a][46+d] = 1
    array_image[4+u-d+ a][46+d] = 1

    array_image[0 -d+ a][40+2*d] = 1
    array_image[0-d+ a][48+2*d] = 1
    array_image[4 -d+ a][42+2*d] = 1
    array_image[-4-d + a][42+2*d] = 1
    array_image[-4 -d+ a][46+2*d] = 1
    array_image[4-d+ a][46+2*d] = 1

    array_image[0+u-d+ a][40+3*d] = 1
    array_image[0+u-d+ a][48+3*d] = 1
    array_image[4+u-d+ a][42+3*d] = 1
    array_image[-4+u-d+ a][42+3*d] = 1
    array_image[-4+u-d+ a][46+3*d] = 1
    array_image[4+u-d+ a][46+3*d] = 1

    array_image[0-d+ a ][40+4*d] = 1
    array_image[0 -d+ a][48+4*d] = 1
    array_image[4 -d+ a][42+4*d] = 1
    array_image[-4 -d+ a][42+4*d] = 1
    array_image[-4 -d+ a][46+4*d] = 1
    array_image[4-d + a][46+4*d] = 1

    # array_image[0-d+ a ][40+5*d] = 1
    # array_image[0 -d+ a][48+5*d] = 1
    # array_image[4 -d+ a][42+5*d] = 1
    # array_image[-4 -d+ a][42+5*d] = 1
    # array_image[-4 -d+ a][46+5*d] = 1
    # array_image[4-d + a][46+5*d] = 1
    #
    # array_image[0-d+ a ][40+6*d] = 1
    # array_image[0 -d+ a][48+6*d] = 1
    # array_image[4 -d+ a][42+6*d] = 1
    # array_image[-4 -d+ a][42+6*d] = 1
    # array_image[-4 -d+ a][46+6*d] = 1
    # array_image[4-d + a][46+6*d] = 1

    #forth

    array_image[0 + 2*d -a-a][40] = 1
    array_image[0+ 2*d-a-a][48] = 1
    array_image[4+ 2*d-a-a][42] = 1
    array_image[-4+ 2*d-a-a][42] = 1
    array_image[-4+ 2*d-a-a][46] = 1
    array_image[4+ 2*d-a-a][46] = 1

    array_image[0+u+ 2*d-a-a][40+d] = 1
    array_image[0+u+ 2*d-a-a][48+d] = 1
    array_image[4+u+ 2*d-a-a][42+d] = 1
    array_image[-4+u+ 2*d-a-a][42+d] = 1
    array_image[-4+u+ 2*d-a-a][46+d] = 1
    array_image[4+u+ 2*d-a-a][46+d] = 1

    array_image[0 + 2*d-a-a][40+2*d] = 1
    array_image[0+ 2*d-a-a][48+2*d] = 1
    array_image[4 + 2*d-a-a][42+2*d] = 1
    array_image[-4+ 2*d -a-a][42+2*d] = 1
    array_image[-4 + 2*d-a-a][46+2*d] = 1
    array_image[4+ 2*d-a-a][46+2*d] = 1

    array_image[0+u+ 2*d-a-a][40+3*d] = 1
    array_image[0+u+ 2*d-a-a][48+3*d] = 1
    array_image[4+u+ 2*d-a-a][42+3*d] = 1
    array_image[-4+u+ 2*d-a-a][42+3*d] = 1
    array_image[-4+u+ 2*d-a-a][46+3*d] = 1
    array_image[4+u+ 2*d-a-a][46+3*d] = 1

    array_image[0+ 2*d-a-a ][40+4*d] = 1
    array_image[0+ 2*d -a-a][48+4*d] = 1
    array_image[4 + 2*d-a-a][42+4*d] = 1
    array_image[-4+ 2*d -a-a][42+4*d] = 1
    array_image[-4+ 2*d-a-a ][46+4*d] = 1
    array_image[4 + 2*d-a-a][46+4*d] = 1

    # array_image[0+ 2*d-a-a ][40+5*d] = 1
    # array_image[0+ 2*d -a-a][48+5*d] = 1
    # array_image[4 + 2*d-a-a][42+5*d] = 1
    # array_image[-4+ 2*d -a-a][42+5*d] = 1
    # array_image[-4+ 2*d-a-a ][46+5*d] = 1
    # array_image[4 + 2*d-a-a][46+5*d] = 1
    #
    # array_image[0+ 2*d-a-a ][40+6*d] = 1
    # array_image[0+ 2*d -a-a][48+6*d] = 1
    # array_image[4 + 2*d-a-a][42+6*d] = 1
    # array_image[-4+ 2*d -a-a][42+6*d] = 1
    # array_image[-4+ 2*d-a-a ][46+6*d] = 1
    # array_image[4 + 2*d-a-a][46+6*d] = 1

    #fith
    array_image[0 - 2 * d+a+a][40] = 1
    array_image[0 - 2 * d+a+a][48] = 1
    array_image[4 - 2 * d+a+a][42] = 1
    array_image[-4 - 2 * d+a+a][42] = 1
    array_image[-4 - 2 * d+a+a][46] = 1
    array_image[4 - 2 * d+a+a][46] = 1

    array_image[0 + u - 2 * d+a+a][40 + d] = 1
    array_image[0 + u - 2 * d+a+a][48 + d] = 1
    array_image[4 + u - 2 * d+a+a][42 + d] = 1
    array_image[-4 + u - 2 * d+a+a][42 + d] = 1
    array_image[-4 + u - 2 * d+a+a][46 + d] = 1
    array_image[4 + u - 2 * d+a+a][46 + d] = 1

    array_image[0 - 2 * d+a+a][40 + 2 * d] = 1
    array_image[0 - 2 * d+a+a][48 + 2 * d] = 1
    array_image[4 - 2 * d+a+a][42 + 2 * d] = 1
    array_image[-4 - 2 * d+a+a][42 + 2 * d] = 1
    array_image[-4 - 2 * d+a+a][46 + 2 * d] = 1
    array_image[4 - 2 * d+a+a][46 + 2 * d] = 1

    array_image[0 + u - 2 * d+a+a][40 + 3 * d] = 1
    array_image[0 + u - 2 * d+a+a][48 + 3 * d] = 1
    array_image[4 + u - 2 * d+a+a][42 + 3 * d] = 1
    array_image[-4 + u - 2 * d+a+a][42 + 3 * d] = 1
    array_image[-4 + u - 2 * d+a+a][46 + 3 * d] = 1
    array_image[4 + u - 2 * d+a+a][46 + 3 * d] = 1

    array_image[0 - 2 * d+a+a][40 + 4 * d] = 1
    array_image[0 - 2 * d+a+a][48 + 4 * d] = 1
    array_image[4 - 2 * d+a+a][42 + 4 * d] = 1
    array_image[-4 - 2 * d+a+a][42 + 4 * d] = 1
    array_image[-4 - 2 * d+a+a][46 + 4 * d] = 1
    array_image[4 - 2 * d+a+a][46 + 4 * d] = 1

    # array_image[0 - 2 * d+a+a][40 + 5 * d] = 1
    # array_image[0 - 2 * d+a+a][48 + 5 * d] = 1
    # array_image[4 - 2 * d+a+a][42 + 5 * d] = 1
    # array_image[-4 - 2 * d+a+a][42 + 5 * d] = 1
    # array_image[-4 - 2 * d+a+a][46 + 5 * d] = 1
    # array_image[4 - 2 * d+a+a][46 + 5 * d] = 1
    #
    # array_image[0 - 2 * d+a+a][40 + 6 * d] = 1
    # array_image[0 - 2 * d+a+a][48 + 6 * d] = 1
    # array_image[4 - 2 * d+a+a][42 + 6 * d] = 1
    # array_image[-4 - 2 * d+a+a][42 + 6 * d] = 1
    # array_image[-4 - 2 * d+a+a][46 + 6 * d] = 1
    # array_image[4 - 2 * d+a+a][46 + 6 * d] = 1

    #sixth

    array_image[0 - 3 * d+3*a][40] = 1
    array_image[0 - 3 * d+3*a][48] = 1
    array_image[4 - 3 * d+3*a][42] = 1
    array_image[-4 - 3 * d+3*a][42] = 1
    array_image[-4 - 3 * d+3*a][46] = 1
    array_image[4 - 3 * d+3*a][46] = 1

    array_image[0 + u - 3 * d+3*a][40 + d] = 1
    array_image[0 + u - 3 * d+3*a][48 + d] = 1
    array_image[4 + u - 3 * d+3*a][42 + d] = 1
    array_image[-4 + u - 3 * d+3*a][42 + d] = 1
    array_image[-4 + u - 3 * d+3*a][46 + d] = 1
    array_image[4 + u - 3 * d+3*a][46 + d] = 1

    array_image[0 - 3 * d+3*a][40 + 2 * d] = 1
    array_image[0 - 3 * d+3*a][48 + 2 * d] = 1
    array_image[4 - 3 * d+3*a][42 + 2 * d] = 1
    array_image[-4 - 3 * d+3*a][42 + 2 * d] = 1
    array_image[-4 - 3 * d+3*a][46 + 2 * d] = 1
    array_image[4 - 3 * d+3*a][46 + 2 * d] = 1

    array_image[0 + u -3 * d+3*a][40 + 3 * d] = 1
    array_image[0 + u - 3 * d+3*a][48 + 3 * d] = 1
    array_image[4 + u - 3 * d+3*a][42 + 3 * d] = 1
    array_image[-4 + u - 3 * d+3*a][42 + 3 * d] = 1
    array_image[-4 + u - 3 * d+3*a][46 + 3 * d] = 1
    array_image[4 + u - 3 * d+3*a][46 + 3 * d] = 1

    array_image[0 - 3* d+3*a][40 + 4 * d] = 1
    array_image[0 - 3* d+3*a][48 + 4 * d] = 1
    array_image[4 - 3* d+3*a][42 + 4 * d] = 1
    array_image[-4 - 3 * d+3*a][42 + 4 * d] = 1
    array_image[-4 - 3 * d+3*a][46 + 4 * d] = 1
    array_image[4 - 3 * d+3*a][46 + 4 * d] = 1

    # array_image[0 - 3* d+3*a][40 + 5 * d] = 1
    # array_image[0 - 3* d+3*a][48 + 5 * d] = 1
    # array_image[4 - 3* d+3*a][42 + 5 * d] = 1
    # array_image[-4 - 3 * d+3*a][42 + 5 * d] = 1
    # array_image[-4 - 3 * d+3*a][46 + 5 * d] = 1
    # array_image[4 - 3 * d+3*a][46 + 5 * d] = 1
    #
    # array_image[0 - 3* d+3*a][40 +  6* d] = 1
    # array_image[0 - 3* d+3*a][48 + 6 * d] = 1
    # array_image[4 - 3* d+3*a][42 + 6 * d] = 1
    # array_image[-4 - 3 * d+3*a][42 + 6 * d] = 1
    # array_image[-4 - 3 * d+3*a][46 + 6 * d] = 1
    # array_image[4 - 3 * d+3*a][46 + 6 * d] = 1

    #seventh
    array_image[0 + 3 * d-3*a][40] = 1
    array_image[0 + 3 * d-3*a][48] = 1
    array_image[4 + 3 * d-3*a][42] = 1
    array_image[-4 + 3 * d-3*a][42] = 1
    array_image[-4+ 3 * d-3*a][46] = 1
    array_image[4 + 3 * d-3*a][46] = 1

    array_image[0 + 3 * d-3*a][40+d] = 1
    array_image[0 + 3 * d-3*a][48+d] = 1
    array_image[4 + 3 * d-3*a][42+d] = 1
    array_image[-4 + 3 * d-3*a][42+d] = 1
    array_image[-4+ 3 * d-3*a][46+d] = 1
    array_image[4 + 3 * d-3*a][46+d] = 1

    array_image[0 + 3 * d-3*a][40 + 2 * d] = 1
    array_image[0 + 3 * d-3*a][48 + 2 * d] = 1
    array_image[4 + 3 * d-3*a][42 + 2 * d] = 1
    array_image[-4 + 3 * d-3*a][42 + 2 * d] = 1
    array_image[-4+ 3 * d-3*a][46 + 2 * d] = 1
    array_image[4 + 3 * d-3*a][46 + 2 * d] = 1

    array_image[0 + 3 * d-3*a][40+3*d] = 1
    array_image[0 + 3 * d-3*a][48+3*d] = 1
    array_image[4 + 3 * d-3*a][42+3*d] = 1
    array_image[-4 + 3 * d-3*a][42+3*d] = 1
    array_image[-4+ 3 * d-3*a][46+3*d] = 1
    array_image[4 + 3 * d-3*a][46+3*d] = 1

    array_image[0 + 3* d-3*a][40 + 4 * d] = 1
    array_image[0 + 3* d-3*a][48 + 4 * d] = 1
    array_image[4 + 3* d-3*a][42 + 4 * d] = 1
    array_image[-4 + 3 * d-3*a][42 + 4 * d] = 1
    array_image[-4 + 3 * d-3*a][46 + 4 * d] = 1
    array_image[4 + 3 * d-3*a][46 + 4 * d] = 1

    # array_image[0 + 3* d-3*a][40 + 5 * d] = 1
    # array_image[0 + 3* d-3*a][48 + 5 * d] = 1
    # array_image[4 + 3* d-3*a][42 + 5 * d] = 1
    # array_image[-4 + 3 * d-3*a][42 + 5 * d] = 1
    # array_image[-4 + 3 * d-3*a][46 + 5 * d] = 1
    # array_image[4 + 3 * d-3*a][46 + 5 * d] = 1
    #
    # array_image[0 + 3* d-3*a][40 + 6 * d] = 1
    # array_image[0 + 3* d-3*a][48 + 6 * d] = 1
    # array_image[4 + 3* d-3*a][42 + 6 * d] = 1
    # array_image[-4 + 3 * d-3*a][42 + 6 * d] = 1
    # array_image[-4 + 3 * d-3*a][46 + 6 * d] = 1
    # array_image[4 + 3 * d-3*a][46 + 6 * d] = 1
    return array_image

def CMOS_data_import():
    CMOS_img = cam.acquire()
    CMOS_img = cv2.flip(CMOS_img,-1)  # flip the camera image, which can be canceled by changign the orientation of camera
    CMOS_img = rescale(CMOS_img, magnifying_factor)
    return CMOS_img

def unwrap(zernike):

    zernike = np.arctan2(np.sin(zernike), np.cos(zernike))
    zernike = np.angle(np.exp(1j * zernike))
    zernike = np.add(zernike, np.pi)

    return zernike

def Zernike_phs(order, sub_order, variable):
    X, Y = 1080, 1080
    x,y =  np.meshgrid(np.linspace(-1,1,X), np.linspace(-1,1,Y))
    d = np.sqrt(x*x + y*y)
    theta = np.arctan(np.divide(y,x))

    if int(order) == 2:
        if int(sub_order) == 0:
            zernike_2_0 = float(variable) *np.sqrt(3) * (2 * (d) ** 2 - 1)
            zernike_2_0 = unwrap(zernike_2_0)
            return zernike_2_0

        if int(sub_order) == 2:
            zernike_2_2 = float(variable)*np.sqrt(6) * ((d) ** 2)*np.cos(2*theta)
            zernike_2_2 = unwrap(zernike_2_2)
            return zernike_2_2

        if int(sub_order) == -2:
            zernike_2_n2 = float(variable)*np.sqrt(6) * ((d) ** 2)*np.sin(2*theta)
            zernike_2_n2 = unwrap(zernike_2_n2)
            return zernike_2_n2


# return the location of chopped image in the original image
def call_out_chopping_boundary(array_image):
    location_tuple = np.where(array_image != 0)
    location_array = np.asarray(location_tuple)
    row_max = np.amax(location_array[0, :])
    col_max = np.amax(location_array[1, :])
    row_min = np.amin(location_array[0, :])
    col_min = np.amin(location_array[1, :])
    boundary = [row_min, row_max + 1, col_min, col_max + 1]
    return boundary

# call out the total number of traps, which is general for different delta function distribution
def total_nums_traps(array_image):
    occurrence = 0
    (Y, X) = array_image.shape
    for index in range(0, Y):
        for sub_index in range(0, X):
            if array_image[index][sub_index] != 0:
                occurrence = occurrence + 1
    return occurrence

# cut the image which has smaller data size and just contain the region where has traps
def image_chopping(array_image):
    location_tuple = np.where(array_image != 0)
    location_array = np.asarray(location_tuple)
    row_max = np.amax(location_array[0, :])
    col_max = np.amax(location_array[1, :])
    row_min = np.amin(location_array[0, :])
    col_min = np.amin(location_array[1, :])
    array_image_chopped = array_image[row_min: row_max + 1, col_min: col_max + 1]
    return array_image_chopped

# normalize the matrix which adds up to to 1
def normalize_traps(array_image):
    occurrence = 0
    (Y, X) = array_image.shape
    for index in range(0, Y):
        for sub_index in range(0, X):
            if array_image[index][sub_index] != 0:
                occurrence = occurrence + 1
    return np.divide(array_image, occurrence)

# which is to generate a triangular shape array, user can define a different array geometry, here trap number != the total trap number
def gen_triangle_array(trap_number, center_spacing, trap_spacing_x, trap_spacing_y, number_of_row, number_of_col):
    j = 1
    list = []

    (X, Y) = (number_of_row, number_of_col)  # slm dimensions
    array_image = np.zeros((X, Y))

    for i in range(0, trap_number):
        list.append(center_spacing)
        center_spacing = center_spacing + trap_spacing_x

        for k in range(0, j, trap_spacing_y):
            array_image[k][list[i]] = 1
            array_image[-k][list[i]] = 1
        j = j + trap_spacing_y
    return array_image

def line(trap_number):
    (X, Y) = (1080, 1080)  # slm dimensions
    array_image = np.zeros((X, Y))
    for k in range(0,int(trap_number)):
        array_image[80][k+5] = 1

    return array_image

# paste the chopped image which contain updated information to the whole image for fft and ifft
def paste_chopped_image(image_chopped, array_image, image):
    location_tuple = np.where(array_image != 0)
    location_array = np.asarray(location_tuple)
    row_max = np.amax(location_array[0, :])
    col_max = np.amax(location_array[1, :])
    row_min = np.amin(location_array[0, :])
    col_min = np.amin(location_array[1, :])
    image[row_min: row_max + 1, col_min: col_max + 1] = image_chopped

    return image

# to rescale the phase pattern
def rescale(image, percentage):
    new_dim_x = int(image.shape[1] * (float(percentage)))
    new_dim_y = int(image.shape[0] * (float(percentage)))
    new_dim = (new_dim_x, new_dim_y)
    image = cv2.resize(image, new_dim)
    return image

start_time = time.time()

# The code from CGH
# Generation of target array arrangement - starting initialization
trap_number, center_spacing, trap_spacing_x, trap_spacing_y, number_of_row, number_of_col =10 , 80, 3, 3, 1080, 1080
(X, Y) = (1080, 1080)  # slm dimensions

iteration = 50  # iterations for IFTA in total
iteration_for_GSA = 20  # iterations for IFTA without feedback
color = ["r", "g", "b", "y", "c", "k", "m"]
color_map = ["Blues", "jet", "inferno", "magma", "viridis", "afmhot", "gist_heat"]
color_index = 1 # for choosing color in the color map
magnifying_factor = 4 # to scale up the resolution of camera image in order to reduce the error accumulation
boolean = True # defined for the single run code
boolean_1 = True
boolean_2 = True
boolean_coor = True
trap_coor = []


array_image = arbitrary()   # array_image that will be generated. Replace arbitrary by other new target image.

print("the size of image matrix:", array_image.shape)
array_image_chopped = image_chopping(np.fft.fftshift(array_image)) # image that use to calculation
array_image = normalize_traps(array_image)

array_image_chopped = normalize_traps(array_image_chopped)
chopping_boundary = call_out_chopping_boundary(np.fft.fftshift(array_image))
print("row_min: row_max + 1, col_min: col_max + 1",chopping_boundary)
g_factor_array = np.fft.fftshift(np.ones(array_image.shape)) # array that use to store the updated g values
g_factor_array_chopped = np.ones(array_image_chopped.shape)
array_image_chopped_cp = array_image_chopped    # serves as a reference on trap coordinate in the ideal image plane in case the trap fades out during calculation
array_image_rl = array_image
array_image_chopped_rl = array_image_chopped

# plt.imshow(array_image, cmap=color_map[color_index])
# plt.title("target array matrix before fftshift")
# plt.show()
#
# plt.imshow(np.fft.fftshift(array_image), cmap=color_map[color_index])
# plt.title("target array matrix after fftshift")
# plt.show()

plt.imshow(array_image_chopped, cmap=color_map[color_index])
plt.title("target array matrix")
plt.show()

"# slm laser intensity"
# position of incidence beam for Holography generation
X_axis, Y_axis = 0, 0  # position of incidence beam for simulating the SLM pattern generator phase
sigma2, mu2 = 800, 0.0  # gaussian parameter for simulation, sigma2 control the beam size

x, y = np.meshgrid(np.linspace(-X, X, X), np.linspace(-Y, Y, Y))
x, y = (x - X_axis), (y - Y_axis)  # moving the beam on the slm
d = np.sqrt((x * x + y * y))
slm_laser_profile = np.exp(-((d - mu2) ** 2 / (2.0 * sigma2 ** 2)))

"# initial phase generation, random phase can also be applied"
# Configure the lens properties:
innerRadius = X / 3
centerX = 0
centerY = 0
# Calculate the phase values of a lens in a pixel-wise matrix:
# pre-calc. helper variables adapted from slm file:
phaseModulation = 2 * math.pi
dataWidth = X
dataHeight = Y

x = np.linspace(1, dataWidth, dataWidth, dtype=np.float32) - np.float32(dataWidth / 2) - np.float32(centerX)
y = np.linspace(1, dataHeight, dataHeight, dtype=np.float32) - np.float32(dataHeight / 2) - np.float32(centerY)
x2 = np.matrix(x * x)  # what is the purpose of this 2 lines
y2 = np.matrix(y * y).transpose()
phaseData = np.float32(phaseModulation) * np.array(
    (np.dot(np.ones([dataHeight, 1], np.float32), x2) + np.dot(y2, np.ones([1, dataWidth], np.float32))),
    dtype=np.float32) / np.float32(innerRadius * innerRadius)
initial_phase_profile = phaseData
slm_plane_data = np.multiply(slm_laser_profile, np.exp(np.multiply(initial_phase_profile, 1j)))

# Generation of target array arrangement - ending initialization

"run the GSA , generate the coarse array first then start the feedback to increase the uniformity"

for iteration_s in range(0, iteration):

    image_plane_data = np.fft.fft2(slm_plane_data)
    mag_spec = (np.abs(image_plane_data))  # magnitude on the image plane, which corresponding to the image that being formed on the camera
    mag_spec = np.divide(mag_spec, np.amax(mag_spec))  # normalize the magnitude spectrum
    mag_spec_chopped = np.fft.fftshift(mag_spec)[ chopping_boundary[0] : chopping_boundary[1] , chopping_boundary[2]: chopping_boundary[3] ] # this is for initial array preparation only


    if iteration_s <= iteration_for_GSA:

        img_phs_spec = np.angle(image_plane_data)  # phase on the image plane

        # Here comes in the WGSA

        total_intensity_0 = 0  # calculation of the average intensity
        total_amp = 0   # initialize the total trap amp
        z_phase = np.zeros((1080,1080)) # to make the code works, but that's not the right phase

        # run over all the non-zero pixels in the target array image
        for index in range(0, array_image_chopped.shape[0]):  # for rows in the target array image
            for sub_index in range(0, array_image_chopped.shape[1]):  # for columns in the target array image

                if array_image_chopped_cp[index][sub_index] != 0:   # if there is a trap

                    total_intensity_0 = total_intensity_0 + mag_spec_chopped[index][sub_index]  # add its intentisy

                    total_amp = total_amp + np.sqrt(mag_spec_chopped[index][sub_index])     # add its amp

                else:
                    continue

        average_intensity = total_intensity_0 / (total_nums_traps(array_image_chopped_cp))  # define the average trap intensity
        average_amp = total_amp / (total_nums_traps(array_image_chopped_cp))

        # phase fixing WGSA

        for index in range(0, array_image_chopped.shape[0]):    # for rows in the target array image
            for sub_index in  range(0, array_image_chopped.shape[1]):   # for columns in the target array image

                if array_image_chopped_cp[index][sub_index] != 0:

                    g_factor_array_chopped[index][sub_index] = (average_amp) / (np.sqrt(mag_spec_chopped[index][sub_index]))  # calculating the g factor


                    array_image_chopped[index][sub_index] = g_factor_array_chopped[index][sub_index] * array_image_chopped[index][sub_index]  # updating the target array

                    # print(g_factor_array_chopped[index][sub_index])
                else:

                    continue

        array_image = np.fft.ifftshift(paste_chopped_image(array_image_chopped,np.fft.fftshift(array_image), np.fft.fftshift(array_image)))      # update the target image

            # problem remains for tmr, paste the chopped mag_spec back to the right place

        print("iteratons:", str(iteration_s))
        print("the average intensity is: ", str(average_intensity)) # the average intensity should increase monocrhomatically if the feedback is working

            # Here ends the WGSA

        replaced_img_data = np.multiply(array_image, np.exp(np.multiply(img_phs_spec, 1j)))
        slm_plane_data = np.fft.ifft2(replaced_img_data)
        slm_plane_mag = np.abs(slm_plane_data)  # to be discarded
        slm_plane_data_phs = np.angle(slm_plane_data)
        slm_plane_data = np.multiply(slm_laser_profile, np.exp(np.multiply(slm_plane_data_phs, 1j)))

        phaseData = np.add(slm_plane_data_phs, math.pi)  # the slm transform data from 0-2Pi to 8 bit
        # add the phase Data to the Zernike Phase
        error = slm.showPhasevalues(phaseData, ShowFlags.PresentFitWithBars)  # upload phase to the slm
        assert error == ErrorCode.NoError, slm.errorString(error)

        print("the scale of phase data", np.amax(phaseData), np.amin(phaseData))


        "Aberration fixing"
        if iteration_s == iteration_for_GSA:
            increment = float(0.1)
            ab_boundary = 30
            order = [2]  # the order of zernike polynomial to be added in
            sub_order = [-2, 2]
            variabl_buffer = [] # variable buffer store the variable corresponding to the sub-order z phase

            while boolean_2:
                try:
                    CMOS_img_1 = CMOS_data_import()
                    plt.imshow(CMOS_img_1, cmap=color_map[color_index])
                    plt.title("choose the coordinate of the fartest trap")
                    plt.show()
                    ax, ay = input("Choose the fartest trap coordinate:").split()

                except ValueError or SyntaxError:
                    continue

                else:
                    boolean_2 = False


            for index in order:
                print("Aberration fixing in progress")
                for sub_index in sub_order:

                    temperary = []  # up to here, store the variable and peak value downs there

                    for variable in np.arange(0, 1.5, increment):
                        z_phase = Zernike_phs(index, sub_index, float(variable))  # creat the new zernike phase
                        phaseDatanew = np.add(phaseData, z_phase)  # add the array phase and the zernike phase tgt
                        error = slm.showPhasevalues(phaseDatanew, ShowFlags.TransposeData)  # upload phase to the slm
                        assert error == ErrorCode.NoError, slm.errorString(error)
                        print("fixing aberration")
                        CMOS_img_1 = CMOS_data_import()
                        reference_trap = CMOS_img_1[int(ay) - int(ab_boundary / 2): int(ay) + int(ab_boundary / 2),
                                         int(ax) - int(ab_boundary / 2): int(ax) + int(ab_boundary / 2)]
                        ref_peak = np.amax(
                            reference_trap)  # which is the maximum trap intensity before aberration fixing
                        phaseDatanew = phaseData  # renew the phase data, this phase doen's involve calculation of array phase
                        temperary.append([variable, ref_peak])

                        # print("ref peak, updated peak", np.amax(reference_trap), np.amax(updated_trap))
                        print("variable", str(variable))
                        print("ref peak, updated peak", np.amax(reference_trap))

                    print("look for the maximum", temperary)
                    beta_tuple = np.where(
                        temperary == np.amax(temperary))  # the first index is variable, second is the ref peak
                    beta_array = np.asarray(beta_tuple)
                    print("This is the corresponding variable", temperary[beta_array[0][0]][
                        0])  # this number gives the corresponding variable to Zernike polynomial
                    variabl_buffer.append(temperary[beta_array[0][0]][0])
            z_phase = Zernike_phs(2, -2, variabl_buffer[0])

        phaseData = np.add(phaseData,z_phase)
        #error = slm.showPhasevalues(phaseData, ShowFlags.PresentFitWithBars)  # upload phase to the slm
        error = slm.showPhasevalues(phaseData, ShowFlags.TransposeData)  # upload phase to the slm

        # now start the camera feedback
        # import camera image

    #"Start running the camera feedback"
    else:
        min_trap_dist = 7          # setting the minimum trap distance, which depends on the magnifying factor, user has to measure it.
        CMOS_img = CMOS_data_import()       # capture camera image
        y_corr, x_corr = 0, 0  # initialize the correction steps = 0


        "Check if the traps are observable, input 1=0 and adjust the laser power, input 1 when you want to go on"
        while boolean:

            CMOS_img = CMOS_data_import()  # capture camera image


            plt.imshow(CMOS_img, cmap=color_map[color_index])
            plt.title("camera image ")
            plt.show()

            try:
                response = input("is the trap intensity satisfying no/yes = 0/1 :")

                if int(response) == 0:
                    continue

                if int(response) == 1:
                    break

            except ValueError:
                print("ValueError ! ")
                continue

        boolean = False

        # dy, dx = min_trap_dist * (trap_spacing_y), min_trap_dist * (trap_spacing_x)
        dy, dx = min_trap_dist, min_trap_dist
        # dy, dx = 30, 36
        intensity_buffer = []
        image_buffer = []
        m, n = 0, 0
        boundary = 16

        "cutting the region of interest for the feedback, 3 sets of data is needed in total. Optical aberration " \
        "will also be reduced in this part "

        if boolean_1 :
            CMOS_img = CMOS_data_import()

            plt.imshow(CMOS_img, cmap=color_map[color_index])  # manual input data
            plt.title("camera image ")
            plt.show()


            try:
                x, y = input("Coordinate of upper left corner: ").split()
                X, Y = input("Coordinate of bottom right corner: ").split()
                CMOS_img = CMOS_img[int(y): int(Y), int(x): int(X)]


                plt.imshow(CMOS_img, cmap=color_map[color_index])
                plt.title("image that use for feedback ")
                plt.show()

                s_x, s_y = input("Enter the starting point: ").split()
                cx, cy = s_x, s_y

            except ValueError or SyntaxError:
                continue

            boolean_1 = False

        else:
            CMOS_img = CMOS_img[int(y): int(Y), int(x): int(X)]
            cx, cy = s_x, s_y    # the cx and cy will be renewed every time

        CMOS_img = np.divide(CMOS_img,np.amax(CMOS_img))    # normalize the trap intensity

        if iteration_s < 40:

            img_phs_spec = np.angle(image_plane_data)  # phase on the image plane

        else:
            img_phs_spec = img_phs_spec  # phase fixing

        while boolean_coor :

            if len(intensity_buffer)< total_nums_traps(array_image_chopped_cp) :    # if the number of trap image is not enough, the code keeps runnning

                try:    # searching the traps' location
                    CMOS_traps = CMOS_img[int(cy)+int(y_corr) + int(dy * n) - int(boundary/2)  : int(cy) +int(y_corr)+ int(dy * n) + int(boundary/2), int(cx)+int(x_corr) + int(dx * m) - int(boundary/2) : int(cx)+int(x_corr) + int(dx * m) + int(boundary/2)]
                    t_max = np.amax(CMOS_traps)
                    up_value = np.amax(CMOS_traps[0,:])
                    left_value = np.amax(CMOS_traps[:,0])
                    down_value = np.amax(CMOS_traps[CMOS_traps.shape[0]-1, :])
                    right_value = np.amax(CMOS_traps[:,CMOS_traps.shape[1]-1])
                    central_value = CMOS_traps[int(CMOS_traps.shape[0]/2),int(CMOS_traps.shape[1]/2)]
                    boundary_value = [up_value,down_value,left_value,right_value]
                    trap_y, trap_x = int(
                        int(cy) + int(y_corr) + int(dy * n) - int(boundary / 2) + int(cy) + int(y_corr) + int(
                            dy * n) + int(
                            boundary / 2)) / 2, int(
                        int(cx) + int(x_corr) + int(dx * m) - int(boundary / 2) + int(cx) + int(x_corr) + int(
                            dx * m) + int(
                            boundary / 2)) / 2

                    # print("n and m :", str(n), str(m))

                    "fit_trap which is a function that return the average trap intensity "

                    "Here sets the traps identification, for traps have relative intensity(ranging from 0 - 1) >= 0.3 we count it as a trap"
                    if np.amax(t_max) >= 0.3 and central_value > np.amax(boundary_value):

                        while np.abs(right_value - left_value) > 0.15or np.abs(up_value - down_value) >0.15: # which means the trap off centered
                            print("error correction working!")
                            while right_value - left_value > 0.15: # fix the horrizontal line

                                x_corr = x_corr + 1
                                Modified_CMOS_image = CMOS_img[int(cy) + int(dy * n) - int(boundary / 2) + int(y_corr): int(
                                    cy) + int(dy * n) + int(boundary / 2) + int(y_corr),
                                                      int(cx) + int(dx * m) - int(boundary / 2) + int(x_corr): int(
                                                          cx) + int(dx * m) + int(boundary / 2) + int(x_corr)]

                                trap_y, trap_x = int(
                                    int(cy) + int(y_corr) + int(dy * n) - int(boundary / 2) + int(cy) + int(
                                        y_corr) + int(dy * n) + int(
                                        boundary / 2)) / 2, int(
                                    int(cx) + int(x_corr) + int(dx * m) - int(boundary / 2) + int(cx) + int(
                                        x_corr) + int(dx * m) + int(
                                        boundary / 2)) / 2

                                up_value = np.amax(Modified_CMOS_image[0, :])
                                left_value = np.amax(Modified_CMOS_image[:, 0])
                                down_value = np.amax(Modified_CMOS_image[Modified_CMOS_image.shape[0] - 1, :])
                                right_value = np.amax(Modified_CMOS_image[:, Modified_CMOS_image.shape[1] - 1])
                                print("Too right ! the LR difference:", right_value - left_value)

                                # renew the left right value

                            while right_value - left_value < -0.15:  # fix the horrizontal line
                                x_corr = x_corr - 1
                                Modified_CMOS_image = CMOS_img[int(cy) + int(dy * n) - int(boundary / 2) + int(y_corr): int(
                                    cy) + int(dy * n) + int(boundary / 2) + int(y_corr),
                                                      int(cx) + int(dx * m) - int(boundary / 2) + int(x_corr): int(
                                                          cx) + int(dx * m) + int(boundary / 2) + int(x_corr)]

                                trap_y, trap_x = int(
                                    int(cy) + int(y_corr) + int(dy * n) - int(boundary / 2) + int(cy) + int(
                                        y_corr) + int(dy * n) + int(
                                        boundary / 2)) / 2, int(
                                    int(cx) + int(x_corr) + int(dx * m) - int(boundary / 2) + int(cx) + int(
                                        x_corr) + int(dx * m) + int(
                                        boundary / 2)) / 2

                                up_value = np.amax(Modified_CMOS_image[0, :])
                                left_value = np.amax(Modified_CMOS_image[:, 0])
                                down_value = np.amax(Modified_CMOS_image[Modified_CMOS_image.shape[0] - 1, :])
                                right_value = np.amax(Modified_CMOS_image[:, Modified_CMOS_image.shape[1] - 1])

                                print("Too left! the LR difference:", right_value - left_value)

                            while up_value - down_value > 0.15:
                                y_corr = y_corr -1
                                Modified_CMOS_image = CMOS_img[int(cy) + int(dy * n) - int(boundary / 2) + int(y_corr): int(
                                    cy) + int(dy * n) + int(boundary / 2) + int(y_corr),
                                                      int(cx) + int(dx * m) - int(boundary / 2) + int(x_corr): int(
                                                          cx) + int(dx * m) + int(boundary / 2) + int(x_corr)]

                                trap_y, trap_x = int(
                                    int(cy) + int(y_corr) + int(dy * n) - int(boundary / 2) + int(cy) + int(
                                        y_corr) + int(dy * n) + int(
                                        boundary / 2)) / 2, int(
                                    int(cx) + int(x_corr) + int(dx * m) - int(boundary / 2) + int(cx) + int(
                                        x_corr) + int(dx * m) + int(
                                        boundary / 2)) / 2

                                up_value = np.amax(Modified_CMOS_image[0, :])
                                left_value = np.amax(Modified_CMOS_image[:, 0])
                                down_value = np.amax(Modified_CMOS_image[Modified_CMOS_image.shape[0] - 1, :])
                                right_value = np.amax(Modified_CMOS_image[:, Modified_CMOS_image.shape[1] - 1])
                                print("Too high ! the UD difference:", up_value - down_value)

                            while up_value - down_value < -0.15:
                                y_corr = y_corr + 1
                                Modified_CMOS_image = CMOS_img[int(cy) + int(dy * n) - int(boundary / 2) + int(y_corr): int(
                                    cy) + int(dy * n) + int(boundary / 2) + int(y_corr),
                                                      int(cx) + int(dx * m) - int(boundary / 2) + int(x_corr): int(
                                                          cx) + int(dx * m) + int(boundary / 2) + int(x_corr)]

                                trap_y, trap_x = int(
                                    int(cy) + int(y_corr) + int(dy * n) - int(boundary / 2) + int(cy) + int(
                                        y_corr) + int(dy * n) + int(
                                        boundary / 2)) / 2, int(
                                    int(cx) + int(x_corr) + int(dx * m) - int(boundary / 2) + int(cx) + int(
                                        x_corr) + int(dx * m) + int(
                                        boundary / 2)) / 2

                                up_value = np.amax(Modified_CMOS_image[0, :])
                                left_value = np.amax(Modified_CMOS_image[:, 0])
                                down_value = np.amax(Modified_CMOS_image[Modified_CMOS_image.shape[0] - 1, :])
                                right_value = np.amax(Modified_CMOS_image[:, Modified_CMOS_image.shape[1] - 1])
                                print("Too low ! the UD difference:", up_value - down_value)

                                # print("error reduced")
                                "Testing the error correction scheme"
                        trap_coor.append([trap_y, trap_x])  # store the trap coordinate (row, column)
                        intensity_buffer.append(t_max)  # store the trap's peak intensity in to a list and call out chonologically while calculating the g matrix
                        print("traps have been capture ,", len(intensity_buffer) ,"intensity: ", t_max)

                    m = m + 1


                except ValueError:  # when the scanning hits the boundary, then move to a new row

                    n = n + 1
                    m = 0 # back to the 0 column
                    y_corr = 0
                    x_corr = 0

            else:
                intensity_buffer = [] # empty the captured intensity
                boolean_coor = False # no longer search traps over the trapping plane, the while loop wont run in the next iteration


                # except ValueError:
                #     n = 0       # start over
                #     m = 0
                #     x_corr = 0
                #     y_corr = 0
                #     CMOS_img = CMOS_data_import()       # using new image
                #     CMOS_img = CMOS_img[int(y): int(Y), int(x): int(X)]
                #     CMOS_img = np.divide(CMOS_img,np.amax(CMOS_img))
                #     plt.imshow(CMOS_img, cmap="Blues")
                #     plt.title("reloaded traps")
                #     plt.show()
                #     cx, cy = input("reinput cx and cy ").split()
                #     intensity_buffer = []

        for coord in trap_coor:
            CMOS_traps = CMOS_img[int(coord[0])  - int(boundary / 2): int(coord[0])  + int(boundary / 2),int(coord[1])  - int(boundary / 2): int(coord[1]) + int(boundary / 2)]
            t_max = np.amax(CMOS_traps)
            intensity_buffer.append(t_max)
            print("trap intensity:", np.amax(CMOS_traps), "number of traps being captured:", len(intensity_buffer))



        intensity_buffer = np.divide(intensity_buffer, np.amax(intensity_buffer)) # normalize the intensity of the traps

        print("Number of traps", len(intensity_buffer)) # to see if the trap number are enough

        # up to here, we are able to obtain information from camera and ready for feedback

        #calculate the g matrix

        total_real_intensity = 0
        total_real_amp = 0

        for index in range(0,len(intensity_buffer)):
            total_real_intensity = total_real_intensity + intensity_buffer[index]
            total_real_amp = total_real_amp + np.sqrt(intensity_buffer[index])

        average_intensity = total_real_intensity / (total_nums_traps(array_image_chopped_cp))  # define the average trap intensity
        average_amp = total_real_amp / (total_nums_traps(array_image_chopped_cp))

        i = 0

        for index in range(0, array_image_chopped_cp.shape[0]):
            for sub_index in  range(0, array_image_chopped_cp.shape[1]):

                if array_image_chopped_cp[index][sub_index] != 0:

                    g_factor_array_chopped[index][sub_index] = (average_amp) / (np.sqrt(intensity_buffer[i]))  # calculating the g factor


                    array_image_chopped_rl[index][sub_index] = g_factor_array_chopped[index][sub_index] * array_image_chopped_rl[index][sub_index]  # updating the target array

                    i = i + 1

        array_image_rl = np.fft.ifftshift(paste_chopped_image(array_image_chopped_rl, np.fft.fftshift(array_image_rl), np.fft.fftshift(array_image_rl)))  # paste the chopped image back

        print("iteratons:", str(iteration_s))
        print("the average intensity is: ", str(average_intensity))

            # Here ends the WGSA

        replaced_img_data = np.multiply(array_image_rl, np.exp(np.multiply(img_phs_spec, 1j)))
        slm_plane_data = np.fft.ifft2(replaced_img_data)
        slm_plane_mag = np.abs(slm_plane_data)  # to be discarded
        slm_plane_data_phs = np.angle(slm_plane_data)
        slm_plane_data = np.multiply(slm_laser_profile, np.exp(np.multiply(slm_plane_data_phs, 1j)))

        phaseData = np.add(slm_plane_data_phs, math.pi)  # the slm transform data from 0-2Pi to 8 bit
        phaseData = np.add(phaseData, z_phase)
        error = slm.showPhasevalues(phaseData, ShowFlags.TransposeData)  # upload phase to the slm
        assert error == ErrorCode.NoError, slm.errorString(error)

        print("Iteration:", str(iteration_s))

"see the final image"
plt.imshow(CMOS_img, cmap=color_map[color_index])
plt.title("camera image before cutting")
plt.show()

"statistic of the array"
histogram_list = np.divide(intensity_buffer, np.average(intensity_buffer))
print(len(histogram_list))
plt.hist(histogram_list,bins = np.arange(0.2,1.8,0.01) , color = "b")
plt.title("triangle with adaptive feedback")
plt.show()

# "release the camera"
cam.disconnect()

# # " the phase that creat the array, you may store the phase in a gallary and call out as a flash show"
# plt.imshow((slm_plane_data_phs), cmap="gray")
# plt.show()

slm.utilsSLMPreviewShow() # this line will show the phase pattern on slm display
assert error == ErrorCode.NoError, slm.errorString(error)

time.sleep(50000)

# Unloading the SDK may or may not be required depending on your IDE:
slm.release()

print("---Duty cycle: %s seconds ---" % (time.time() - start_time))