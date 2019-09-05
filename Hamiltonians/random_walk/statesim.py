
import matplotlib.pyplot as plt
import numpy as np
from qutip import *
from sympy import *
import arc
import time
from mpl_toolkits.mplot3d import Axes3D

create1 = tensor(create(2),destroy(2))
create2 = tensor(destroy(2),create(2))
H0 = create1*create2
state1=tensor(basis(2,1),basis(2,0))

times = [1,2,3,4,5]
qobj = qeye(2)

tp1 = 0

def H1_coeff(t, args):
 	return  1

def H4_coeff(t, args):
	return 0

H1 = qeye(2)
H4 = qeye(2)

H = [H0,[H1,H1_coeff]]#,[

sesolve(hopper,state1,times,qobj)

print(output.expect[0])
