#ifndef CUERPO_H
#define CUERPO_H

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
#include "Arreglos.h"
#include "LL.h"

int modgetBody(INSTANCE * my, int * params);
int modcpBodyLocal2World(INSTANCE * my, int * params);
int modDefcpBody(INSTANCE * my, int * params);
int modGetEcpBody(INSTANCE * my, int * params);
int modcpBodySetMass(INSTANCE * my, int * params);
int modcpBodySetMoment(INSTANCE * my, int * params);
int modcpBodySetAngle(INSTANCE * my, int * params);
int modcpBodyUpdatePosition(INSTANCE * my, int * params);
int modcpBodyResetForces(INSTANCE * my, int * params);
int modcpBodyIsSleeping(INSTANCE * my, int * params);
int modcpBodySleep(INSTANCE * my, int * params);
int modcpBodyActivate(INSTANCE * my, int * params);
int modcpBodyIsStatic(INSTANCE * my, int * params);
int modcpBodyIsRogue(INSTANCE * my, int * params);
int modcpBodySleepWithGroup(INSTANCE * my, int * params);
int modcpBodyApplyForce(INSTANCE * my, int * params);
int modcpBodyApplyImpulse(INSTANCE * my, int * params);
int modcpBodyWorld2Local(INSTANCE * my, int * params);
int modcpBodySlew(INSTANCE * my, int * params);
int modcpBodyUpdateVelocity(INSTANCE * my, int * params);
int modActivateProcessTouchingIt(INSTANCE * my, int * params);
void addListaProcesos(INSTANCE * ins);
int modaddCircleShape(INSTANCE * my, int * params);
int modaddSegmentShape(INSTANCE * my, int * params);
int modaddPolyShape(INSTANCE * my, int * params);
int creaPoly(int * params,cpBody* bod);
int modaddSegmentShapeTo(INSTANCE * my, int * params);
#define ajustaTam LOCDWORD( mod_chipmunk, my, LOC_SIZE )/100.0
int creaBodyAndShapeAutomat(INSTANCE *my);
int modcpCalculaConvexHull(INSTANCE * my, int * params);
#endif
