#ifndef CONSTANTES_H
#define CONSTANTES_H


//CONSTANTES
#define CP_C_ITERATIONS 0
#define CP_C_GRAVITY 1
#define CP_C_DAMPING 2
#define CP_C_IDLESPEEDTHRESHOLD 3
#define CP_C_SLEEPTIMETHRESHOLD 4
#define CP_C_staticBody 5

#define CP_C_M  0
#define CP_C_I  1
#define CP_C_P  2
#define CP_C_V  3
#define CP_C_F  4
#define CP_C_A  5
#define CP_C_W  6
#define CP_C_T  7
#define CP_C_ROT  8
#define CP_C_V_LIMIT  9
#define CP_C_W_LIMIT  10

#define CP_C_BODY   0
#define CP_C_BB   1
#define CP_C_E   2
#define CP_C_U   3
#define CP_C_SENSOR   4
#define CP_C_SURFACE_V   5
#define CP_C_COLLISION_TYPE   6
#define CP_C_GROUP   7
#define CP_C_LAYERS   8
#define CP_C_DATA   20

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)


#define CP_C_MAXFORCE 0
#define CP_C_BIASCOEF 1
#define CP_C_MAXBIAS  2
#define CP_C_CA  3
#define CP_C_CB  4


#define CM_PI 3.14159265

#define TYPE_NONE           0
#define TYPE_BOX            1
#define TYPE_CIRCLE         2
#define TYPE_CONVEX_POLYGON 3
#define TYPE_LINE           4
#define TYPE_EMPTY          5

#define     CP_C_ANCHR1     1
#define     CP_C_ANCHR2     10
#define     CP_C_DIST       3
#define     CP_C_MIN        4
#define     CP_C_MAX        5
#define     CP_C_GROOVEB    6
#define     CP_C_GROOVEA    7
#define     CP_C_RESTLENGTH 8
#define     CP_C_STIFFNESS  9
//#define     CP_C_DAMPING    2
#define     CP_C_RESTANGLE  11
#define     CP_C_ANGLE      12
#define     CP_C_PHASE      13
#define     CP_C_RATCHET    14
#define     CP_C_RATIO      15
#define     CP_C_RATE       16

//constantes bennu

#endif
