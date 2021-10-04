#ifndef ARBITROS_H
#define ARBITROS_H

#include "chipmunk.h"
#include "bgddl.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include <math.h>
#include "dlvaracc.h"
#include "libdraw.h"
#include "librender.h"
#include "mod_map.h"
#include "globales.h"
#include "locales.h"
#include "constantes.h"
#include "Arreglos.h"
#include "LL.h"
#include "Handlers.h"

int modArbiterGetNumContactPoints(INSTANCE * my, int * params);
int modArbiterIsFirstContact(INSTANCE * my, int * params);
int modArbiterGetNormal(INSTANCE * my, int * params);
int modArbiterGetPoint(INSTANCE * my, int * params);
int modArbiterGetDepth(INSTANCE * my, int * params);
int modArbiterTotalImpulseWithFriction(INSTANCE * my, int * params);
int modArbiterTotalImpulse(INSTANCE * my, int * params);
int modArbiterProcessA(INSTANCE * my, int * params);
int modArbiterProcessB(INSTANCE * my, int * params);
int modArbiterGetElasticity(INSTANCE * my, int * params);
int modArbiterSetElasticity(INSTANCE * my, int * params);
int modArbiterGetFriction(INSTANCE * my, int * params);
int modArbiterSetFriction(INSTANCE * my, int * params);
int modArbiterGetSurfaceVelocity(INSTANCE * my, int * params);
int modArbiterSetSurfaceVelocity(INSTANCE * my, int * params);

#endif
