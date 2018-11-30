"""
laserCalc.py
Author: Jordan Kemp
Date: 11/11/18
For the purpose of determining wavelength for optical tweezer for Bernien Lab
"""
import numpy as np
import matplotlib.pyplot as plt


"""
Note: Intensity = (Amplitude)^2
    This force is attractive with red detuning, repulsive with blue
Potential energy of the dipole

Definition 1
    U=(hΩ^2/(2π4δ)
     =(hbar)(rabiFreq)^2/(4*delta)

    Where rabi frequency is defined as the element of the perturbing matrix

        Ω=|<1|d*ε|2>|/(h/2π)
         =(Expectation value of causing transition)/(hbar)

Definition 2
    U=hΓ^2/16δπ*Ι/Ιsat
     =(hbar)(transLtime)^2(Intensity)/(8)(detune)(Saturation Intensity)
"""
hbar = 1
satIntensity = 0
intensity = 0
rabiFreq = 0

rabiFreq()

"""
Scattering rate of a laser
Rscatt=(gamma)(rabi)^2/4(detune^2+rabi^2/2+transLtime^2/4)

"""

"""
equations taken from the "Numerical Aperture" wikipedia
    Note: The width of a beam is where the intensity drops to 1/e^2 its on-axis
          value
Waist of a laser
NUMERICAL APERTURE = NA ~ λ/πω0 = (wavelength/(π)(waist radius))
where NUMERICAL APERTURE (NA) is defined as
NA = n*sin[θ] =(index of refraction)(sin[beam divergence])
θ

Other important parameter: Rayleigh Range
    Where the radius is sqrt(2) that of the waist - given by

zR=π(ω0)^2/λ
(rayleigh range)=(π)(waist)^2/(wavelength)
"""
