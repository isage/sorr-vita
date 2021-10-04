#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

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

//int addConstr(cpBody *bod1, cpBody *bod2,cpConstraint * cons);
int modaddDampedSpring(INSTANCE * my, int * params);
int modaddPinJoint(INSTANCE * my, int * params);
int modaddPivotJoint2(INSTANCE * my, int * params);
int modaddSlideJoint(INSTANCE * my, int * params);
int modaddGrooveJoint(INSTANCE * my, int * params);
int modaddSimpleMotor(INSTANCE * my, int * params);
int modaddRatchetJoint(INSTANCE * my, int * params);
int modaddRotaryLimitJoint(INSTANCE * my, int * params);
int modaddGearJoint(INSTANCE * my, int * params);
int modaddDampedRotarySpring(INSTANCE * my, int * params);
int modaddPivotJoint(INSTANCE * my, int * params);

int modsetPinJointProperties(INSTANCE * my, int * params);
int modsetSlideJointProperties(INSTANCE * my, int * params);
int modsetPivotJointProperties(INSTANCE * my, int * params);
int modsetGrooveJointProperties(INSTANCE * my, int * params);
int modsetDampedSpringProperties(INSTANCE * my, int * params);
int modsetDampedRotarySpringProperties(INSTANCE * my, int * params);
int modsetRotaryLimitJointProperties(INSTANCE * my, int * params);
int modsetRatchetJointProperties(INSTANCE * my, int * params);
int modsetGearJointProperties(INSTANCE * my, int * params);
int modsetSimpleMotorProperties(INSTANCE * my, int * params);
int modgetPinJointProperties(INSTANCE * my, int * params);
int modgetSlideJointProperties(INSTANCE * my, int * params);
int modgetPivotJointProperties(INSTANCE * my, int * params);
int modgetGrooveJointProperties(INSTANCE * my, int * params);
int modgetDampedSpringProperties(INSTANCE * my, int * params);
int modgetDampedRotarySpringProperties(INSTANCE * my, int * params);
int modgetRotaryLimitJointProperties(INSTANCE * my, int * params);
int modgetRatchetJointProperties(INSTANCE * my, int * params);
int modgetGearJointProperties(INSTANCE * my, int * params);
int modgetSimpleMotorProperties(INSTANCE * my, int * params);


int modcpConstraintGetImpulse(INSTANCE * my, int * params);
int modDefcpConstraint(INSTANCE * my, int * params);
int modGetcpConstraint(INSTANCE * my, int * params);
int modcpConstraintSetPostSolveFunc(INSTANCE * my, int * params);
int modcpConstraintSetPreSolveFunc(INSTANCE * my, int * params);

int modGetFathetA(INSTANCE * my, int * params);
int modGetFathetB(INSTANCE * my, int * params);

int modremoveConstraint(INSTANCE * my, int * params);
void eliminaConstraint(void * par);

#endif
