#ifndef LOCALES_H
#define LOCALES_H

//Locales c/bennu
#define LOC_BODY                     0
#define LOC_SHAPE                    1
#define LOC_X                        2
#define LOC_Y                        3
#define LOC_ANGLE                    4
#define LOC_ID                       5
#define LOC_INCR_X                   6
#define LOC_INCR_Y                   7
#define LOC_INERTIA                  8
#define LOC_MASS                     9
#define LOC_STATIC                  10
#define LOC_ELASTICITY              11
#define LOC_FRICTION                12
#define LOC_GROUP                   13
#define LOC_LAYERS                  14
#define LOC_RSTATUS                 15
#define LOC_SHAPETYPE               16
#define LOC_PARAMS                  17
#define LOC_FILE                    18
#define LOC_GRAPH                   19
#define LOC_COLLISIONTYPE           20
#define LOC_RESOLUTION              21
#define LOC_SIZE                    22
#define LOC_SENSOR                  23

extern char __bgdexport( mod_chipmunk, locals_def )[];
extern DLVARFIXUP __bgdexport( mod_chipmunk, locals_fixup )[];

#endif
