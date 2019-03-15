// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "AWGController.h"
#include "TrapController.h"
#include "TrapControllerHandler.h"
#include "StaticController.h"
#include "CameraServer.h"

/* Console documentation.
 * ----------------------
 * traps [trap command] [arguments]
 * camera [camera command] [arguments]
 * sdr [sdr command] [arguments]
 *
 *
 * trap commands:
 * list 															-		lists traps.
 * sort																-		sort traps by frequency
 * add [freq in MHz] [amplitude] 									-		adds a trap at a given frequency, amplitude, and phase.
 * delete [trap_index]												-		delete a trap at a given index in the list
 * change [trap_index] [property: freq, amp, phase] [new value]		-		change trap to have a new value for the specified property
 * load_default	[default name]										-		load a default configuration of traps
 * set_phases [phase pattern description]							-		set phases of array to adopt a pattern
 *
 *
 *
 * camera commands:
 * connect							-		reconnect to the camera server
 *
 *
 * sdr commands:
 * gain [new_val]					-		print out current gain (if no arg specified), otherwise set gain value.
 *											requires disconnecting / reconnecting from the SDR.
 * timeout [new_val]				-		print out current timeout value (if no arg specified), otherwise set timeout value.
*/
void run1DConsole(TrapController &trapController, AWGController &AWGController);
void run2DConsole(StaticController &staticController, TrapControllerHandler trapControllerHandler, AWGController &awgController);
