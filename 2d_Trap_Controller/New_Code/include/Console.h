#ifndef CONSOLE_H
#define CONSOLE_H

// Created by H.Levine with Atom Array team, Lukin group (2016).

#include "AWGController.h"
#include "TrapController.h"
#include "TrapControllerHandler.h"
#include "CameraServer.h"
#include "Rearrange2d.h"

#include <malloc.h>
#include <vector>
#include <string>
#include <math.h>
#include <csignal>


void run1DConsole(TrapController &trapController, AWGController &AWGController);
void run2DConsole(TrapControllerHandler trapControllerHandler, AWGController &awgController);


#endif
