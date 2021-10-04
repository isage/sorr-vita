#ifndef GLOBALES_H
#define GLOBALES_H
//Globales c/bennu
#define GLO_SPACE                   0
#define GLO_GRAVITY_X               1
#define GLO_GRAVITY_Y               2
#define GLO_BIAS_COEF               3
#define GLO_COLLISION_SLOP          4
#define GLO_CONTACT_PERSISTENCE     5
#define GLO_ITERATIONS              6
#define GLO_DAMPING                 7
#define GLO_IDLESPEEDTHRESHOLD      8
#define GLO_SLEEPTIMETHRESHOLD      9
#define GLO_INTERVAL                10
#define GLO_PHRESOLUTION            11
#define GLO_DISTPERCENT             12

//globales del programa
extern int modChipmunk_espacioHash;
extern DLVARFIXUP __bgdexport( mod_chipmunk, globals_fixup )[];

#endif
