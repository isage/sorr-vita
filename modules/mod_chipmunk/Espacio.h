#ifndef ESPACIO_H
#define ESPACIO_H

#include <stdlib.h>
#include <math.h>

#include "chipmunk.h"
#include "bgddl.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include "dlvaracc.h"
#include "libdraw.h"
#include "librender.h"
#include "mod_map.h"

int modcpSpaceGetCurrentTimeStep(INSTANCE * my, int * params);
int modGetStaticBody(INSTANCE * my, int * params);
int modcpSpaceActivateShapesTouchingShape(INSTANCE * my, int * params);
int modcpSpaceResizeStaticHash(INSTANCE * my, int * params);
int modcpSpaceResizeActiveHash(INSTANCE * my, int * params);
int modcpSpaceUseSpatialHash(INSTANCE * my, int * params);

#endif
