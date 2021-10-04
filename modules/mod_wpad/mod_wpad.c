/*
 *  Copyright © 2011 Joseba García Etxebarria <joseba.gar@gmail.com>
 *
 *  mod_wpad is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  mod_wpad is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"
#include "libvideo.h"
#ifdef TARGET_WII
#include "wiiuse/wpad.h"
#endif

#include "mod_wpad.h"


// Checks wether a given wpad channel corresponds to a Wii Balance Board
int is_bb(int i)
{
    u32 type;

    WPAD_Probe(i, &type);
    if (type==WPAD_EXP_WIIBOARD)
        return 1;
    else
        return 0;
}

// Checks wether a given wpad channel has a nunchuk attached to it
int has_nunchuk(int i)
{
    u32 type;
	
    WPAD_Probe(i, &type);
    if (type==WPAD_EXP_NUNCHUK)
        return 1;
    else
        return 0;
}

// Checks wether a given wpad channel has a Wii MotionPlus attached to it
int has_motionplus(int i)
{
    return 0;
}

// Checks wether a given wpad channel has a classic controller attached to it
int has_classic(int i)
{
    u32 type;
	
    WPAD_Probe(i, &type);
    if (type==WPAD_EXP_CLASSIC)
        return 1;
    else
        return 0;
}

// Checks wether a given wpad channel has a guitar hero 3
// controller attached to it
int has_guitar(int i)
{
    u32 type;
	
    WPAD_Probe(i, &type);
    if (type==WPAD_EXP_GUITARHERO3)
        return 1;
    else
        return 0;
}

/* Check the status of each Wiimote */
int modwpad_is_ready( INSTANCE * my, int * params )
{
    int res=0;
    u32 type;

    res = WPAD_Probe(params[0], &type);
    if(res == 0)
        return 1;   // So it's true in BennuGD code
    else
        return res;
}

// Get info from generic controller
int modwpad_info( INSTANCE * my, int * params )
{
    u32 type;
    WPADData *wd;

    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], &type) != 0 )
        return 0;

    // Return the info the user asked for
    wd = WPAD_Data(params[0]);
    switch(params[1]) {
        case WPAD_BATT:     // Battery level (0<level<256)
            return (int)WPAD_BatteryLevel(params[0]);
        case WPAD_X:     // X position
            return wd->ir.x;
        case WPAD_Y:     // Y position
            return wd->ir.y;
        case WPAD_Z:     // Z position (distance from screen in m)
            return wd->ir.z;
        case WPAD_ANGLE:     // Angle, BennuGD likes 1/1000th of degrees
            return -(int)(wd->ir.angle*1000.);
        case WPAD_PITCH:     // Pitch angle, BennuGD likes 1/1000th of degrees
            return (int)(wd->orient.pitch*1000.);
        case WPAD_ROLL:     // Roll angle,  BennuGD likes 1/1000th of degrees
            return (int)(wd->orient.roll*1000.);    // Uses accelerometer
        case WPAD_ACCELX:     // Acceleration in (local) x axis
            return wd->accel.x;
        case WPAD_ACCELY:     // Acceleration in (local) y axis
            return wd->accel.y;
        case WPAD_ACCELZ:     // Acceleration in (local) z axis
            return wd->accel.z;
		case WPAD_GX:         // Gravity in the (local) X axis 
			return wd->gforce.x;
		case WPAD_GY:         // Gravity in the (local) Y axis 
			return wd->gforce.y;
		case WPAD_GZ:         // Gravity in the (local) Z axis 
			return wd->gforce.z;
        case WPAD_IS_BB:      // Check wether controller is a balance board
            return is_bb(params[0]);
		case WPAD_HAS_NUNCHUK: // Nunchuk attached to this controller?
			return has_nunchuk(params[0]);
		case WPAD_HAS_CLASSIC: // Classic attached to this controller?
			return has_classic(params[0]);
		case WPAD_HAS_GUITAR: // Guitar Hero 3 attached to this controller?
			return has_guitar(params[0]);
    }

    return 0;
}

// Get info from nunchuk
int modwpad_info_nunchuk( INSTANCE * my, int * params )
{
    u32 type;
    struct expansion_t exp;
	
    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], &type) != 0 )
        return 0;

	// Make sure the device has a nunchuk attached to it
	if( type != WPAD_EXP_NUNCHUK )
		return 0;
	
    // Return the info the user asked for
    WPAD_Expansion(params[0], &exp);
    switch(params[1]) {
        case WPAD_BATT:     // Battery level (0<level<256)
            return (int)WPAD_BatteryLevel(params[0]);
        case WPAD_ANGLE:     // Angle, BennuGD likes 1/1000th of degrees
            return -(int)(exp.nunchuk.orient.yaw*1000.); // Cehck this is correct
        case WPAD_PITCH:     // Pitch angle, BennuGD likes 1/1000th of degrees
            return (int)(exp.nunchuk.orient.pitch*1000.);
        case WPAD_ROLL:     // Roll angle,  BennuGD likes 1/1000th of degrees
            return (int)(exp.nunchuk.orient.roll*1000.);    // Uses accelerometer
        case WPAD_ACCELX:     // Acceleration in x axis
            return exp.nunchuk.accel.x;
        case WPAD_ACCELY:     // Acceleration in y axis
            return exp.nunchuk.accel.y;
        case WPAD_ACCELZ:     // Acceleration in z axis
            return exp.nunchuk.accel.z;
		case WPAD_GX:         // Gravity in the (local) X axis 
			return exp.nunchuk.gforce.x;
		case WPAD_GY:         // Gravity in the (local) Y axis 
			return exp.nunchuk.gforce.y;
		case WPAD_GZ:         // Gravity in the (local) Z axis 
			return exp.nunchuk.gforce.z;
    }
	
    return 0;
}

// Get data from the Wii Balance Board
int modwpad_info_bb( INSTANCE * my, int * params )
{
    struct expansion_t exp;
    u32 type;

    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], &type) != 0 )
        return 0;
	
	// Make sure the device is a BB
	if( type != WPAD_EXP_WIIBOARD )
		return 0;

    // Return the info the user asked for
    WPAD_Expansion(params[0], &exp);
    switch(params[1]) {
        case WPAD_BATT:     // Battery level (0<level<256)
            return (int)WPAD_BatteryLevel(params[0]);
        case WPAD_X:     // X position
            return (int)exp.wb.x;
        case WPAD_Y:     // Y position
            return (int)exp.wb.y;
        case WPAD_WTL:     // Weight measured on the TOP-LEFT base (Balance Board)
            return (int)exp.wb.tl;
        case WPAD_WTR:     // Weight in TOP-RIGHT
            return (int)exp.wb.tr;
        case WPAD_WBL:     // Weight in BOTTOM-LEFT
            return (int)exp.wb.bl;
        case WPAD_WBR:     // Weight in BOTTOM-RIGHT
            return (int)exp.wb.br;
    }

    return 0;
}

// Get info from classic controller
int modwpad_info_classic( INSTANCE * my, int * params )
{
    u32 type;
    struct expansion_t exp;
	
    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], &type) != 0 )
        return 0;

	// Make sure the device has a classic controller attached to it
	if( type != WPAD_EXP_CLASSIC )
		return 0;
	
    // Return the info the user asked for
    WPAD_Expansion(params[0], &exp);
    switch(params[1]) {
        case WPAD_BATT:     // Battery level (0<level<256)
            return (int)WPAD_BatteryLevel(params[0]);
    }
	
    return 0;
}

// Get info from guitar controller
int modwpad_info_guitar( INSTANCE * my, int * params )
{
    u32 type;
    struct expansion_t exp;
	
    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], &type) != 0 )
        return 0;

	// Make sure the device has a guitar attached to it
	if( type != WPAD_EXP_GUITARHERO3 )
		return 0;
	
    // Return the info the user asked for
    WPAD_Expansion(params[0], &exp);
    switch(params[1]) {
        case WPAD_BATT:     // Battery level (0<level<256)
            return (int)WPAD_BatteryLevel(params[0]);
    }
	
    return 0;
}

// Make a controller rumble (or stop rumbling)
int modwpad_rumble( INSTANCE * my, int * params)
{
    // Ensure it's been correctly initialized
    if( WPAD_Probe(params[0], NULL) != 0 )
        return 0;

    WPAD_Rumble(params[0], params[1]);
    return 0;
}

/* ----------------------------------------------------------------- */
/* Function declarations                                             */
DLSYSFUNCS  __bgdexport( mod_wpad, functions_exports )[] =
{
    { "WPAD_IS_READY"          , "I" , TYPE_INT      , modwpad_is_ready     },
    { "WPAD_INFO"              , "II", TYPE_INT      , modwpad_info         },
    { "WPAD_INFO_NUNCHUK"      , "II", TYPE_INT      , modwpad_info_nunchuk },
    { "WPAD_INFO_CLASSIC"      , "II", TYPE_INT      , modwpad_info_classic },
    { "WPAD_INFO_GUITAR"       , "II", TYPE_INT      , modwpad_info_guitar  },
    { "WPAD_INFO_BB"           , "II", TYPE_INT      , modwpad_info_bb      },
    { "WPAD_RUMBLE"            , "II", TYPE_INT      , modwpad_rumble       },
    { 0                        , 0   , 0             , 0                    }
};
