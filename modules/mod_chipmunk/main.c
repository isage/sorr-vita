 /*
 *  Copyright (c) 2010-2011 Prg orlando8nov@yahoo.com.mx
 *
 *  This file is part of mod_chipmunk, a Chipmunk physics binding for BennuGD.
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

/*
 *  Thanks to SplinterGU, Josebita & FreeYourMind
 */


#ifndef __libChipmunk
#define __libChipmunk
#include <stdlib.h>
#include <math.h>

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
#include "arbitros.h"
#include "Espacio.h"
#include "Constraints.h"
#include "estructuras.h"
#include "Cuerpo.h"
#include "Miscelanea.h"
#include "automGenTerr.h"
#include "agua.h"

//En cuerpo, distAng no est[a implementada ni terminada

#if CP_USE_DOUBLES!=0
#error LA LIBRERIA NO TRABAJA CON DOUBLES, DEFINASE CP_USE_DOUBLES A 0
#endif
//codigo necesario de versionest anteriores de chipmunk

//extern float * raiz;
//extern int av;
int modChipmunk_espacioHash=0;
modChipmunkStruct_nodo * modChipmunk_ListaHandlers;
cpSpace * modChipmunk_cpEspacio;
//Globales
DataPointer  modChipmunk_ListaCuerpos=NULL;
int modChipmunk_Crear=1;
//modChipmunkStruct_nodo * modChipmunk_ListaEstaticosFondo;
int modChipmunk_numLista=0;
cpBool modChipmunk_elim=0;

char __bgdexport( mod_chipmunk, globals_def )[] =
    "STRUCT gphysics\n"
    "int space;\n"
    "float gravity_X;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X)) */
    "float gravity_Y;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)) */
    "float bias_coef=0.5;\n"                                                  /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_BIAS_COEF)) */
    "float collision_slop=0.1;\n"                                             /* Access and set with *(FLOAT *)GLOADDR(mod_chipmunk,GLO_COLLISION_SLOP)) */
    "int contact_persistence=3;\n"
    "int iterations=10;\n"                                        /* Access and set with GLOADDR(mod_chipmunk,GLO_CONTACT_PERSISTENCE)) */
    "float damping=1.0;\n"
    "float idleSpeedThreshold =0;\n"
    "float sleepTimeThreshold =9999999999;\n"
    "float interval =1.0/25.0;\n"
    "int phresolution =3;\n"
    "float distPercent =0.5;\n"
    "END\n"
    "cpVect cpvzero=0,0;\n"

    ;

DLVARFIXUP __bgdexport( mod_chipmunk, globals_fixup )[] =
{
    { "gphysics.space", NULL, -1, -1 },
    { "gphysics.gravity_X", NULL, -1, -1 },
    { "gphysics.gravity_Y", NULL, -1, -1 },
    { "gphysics.bias_coef", NULL, -1, -1 },
    { "gphysics.collision_slop", NULL, -1, -1 },
    { "gphysics.contact_persistence", NULL, -1, -1 },
    { "gphysics.iterations", NULL, -1, -1 },
    { "gphysics.damping", NULL, -1, -1 },
    { "gphysics.idleSpeedThreshold", NULL, -1, -1 },
    { "gphysics.sleepTimeThreshold", NULL, -1, -1 },
    { "gphysics.interval", NULL, -1, -1 },
    { "gphysics.phresolution", NULL, -1, -1 },
    { "gphysics.distPercent", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

char __bgdexport( mod_chipmunk, locals_def )[] =
    "STRUCT lphysics\n"
    "int body=0;\n"                                                        /* Access and set with GLODWORD(mod_chipmunk,GLO_SPACE) */
    "int shape=0;\n"                                                        /* Access and set with GLODWORD(mod_chipmunk,GLO_SPACE) */
    "int incr_x=0;\n"                                    /* Access and set with GLOADDR(mod_chipmunk,GLO_CONTACT_PERSISTENCE)) */
    "int incr_y=0;\n"
    "float inertia=1.0;\n"
    "float mass=1;\n"
    "int static=0;\n"
    "float elasticity=1.0;\n"
    "float friction=1.0;\n"
    "int group=CP_NO_GROUP;\n"
    "int layers=CP_ALL_LAYERS;\n"
    "int ShapeType=TYPE_NONE;\n"
    "int * params=null;\n"
    "int collisionType=0;\n"
    "int sensor=0;\n"
    "end\n"

    ;

DLVARFIXUP __bgdexport( mod_chipmunk, locals_fixup )[] =
{
    { "lphysics.body", NULL, -1, -1 },
    { "lphysics.shape", NULL, -1, -1 },
    { "x", NULL, -1, -1 },
    { "y", NULL, -1, -1 },
    { "angle", NULL, -1, -1 },
    { "id", NULL, -1, -1 },
    { "lphysics.incr_x", NULL, -1, -1 },
    { "lphysics.incr_y", NULL, -1, -1 },
    { "lphysics.inertia", NULL, -1, -1 },
    { "lphysics.mass", NULL, -1, -1 },
    { "lphysics.static", NULL, -1, -1 },
    { "lphysics.elasticity", NULL, -1, -1 },
    { "lphysics.friction", NULL, -1, -1 },
    { "lphysics.group", NULL, -1, -1 },
    { "lphysics.layers", NULL, -1, -1 },
    { "reserved.status", NULL, -1, -1 },
    { "lphysics.ShapeType", NULL, -1, -1 },
    { "lphysics.params", NULL, -1, -1 },
    { "file", NULL, -1, -1 },
    { "graph", NULL, -1, -1 },
    { "lphysics.collisionType", NULL, -1, -1 },
    { "resolution", NULL, -1, -1 },
    { "size", NULL, -1, -1 },
    { "lphysics.sensor", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

char __bgdexport( mod_chipmunk, types_def )[] =
    "TYPE cpVect\n"
        " float x;\n"
        " float y;\n"
    "END\n"

    "TYPE cpBB\n"
        " float l, b, r ,t;\n"
    "END\n"

    "TYPE cpsegmentqueryinfo\n"
        "int shape, id;\n"
        "float x,y,t; "
        "cpVect n; \n"
    "END\n"

    "TYPE tPoints\n"
        " cpVect Point, normal;\n"
        " float dist;"
    " end \n"

    "TYPE cpContactPointSet\n"
        "int count;\n"
        "int id1, id2;\n"
        "int shape1,shape2;\n"
        "tPoints points[6]; "
    "end\n"

    "TYPE WaterS\n "
      "int*ids=0;\n"
      "int size=0;\n"
      "float kNorm=2.2104;\n"
      "float kNearNorm=3.3157;\n"
      "float kRestDensity=82;\n"
      "float kStiffness=0.08;\n"
      "float kNearStiffness=0.1;\n"
      "float kSurfaceTension=0.0004;\n"
      "float kLinearViscocity=0.5;\n"
      "float kQuadraticViscocity=1;\n"
      "float KH=120;\n"
    "end\n"
//"TYPE cpSegmentQueryInfo\n" " cpShape *shape;\n " " float t;\n " "cpVect n;\n""END\n"
    ;

DLCONSTANT __bgdexport( mod_chipmunk, constants_def )[] =
{
    { "CP_C_ITERATIONS", TYPE_INT, CP_C_ITERATIONS },
    { "CP_C_GRAVITY", TYPE_INT, CP_C_GRAVITY },
    { "CP_C_DAMPING", TYPE_INT, CP_C_DAMPING },
    { "CP_C_IDLESPEEDTHRESHOLD", TYPE_INT, CP_C_IDLESPEEDTHRESHOLD },
    { "CP_C_SLEEPTIMETHRESHOLD", TYPE_INT, CP_C_SLEEPTIMETHRESHOLD },
    { "CP_C_staticBody", TYPE_INT, CP_C_staticBody },

    { "CP_C_M", TYPE_INT, CP_C_M },
    { "CP_C_I", TYPE_INT, CP_C_I },
    { "CP_C_P", TYPE_INT, CP_C_P },
    { "CP_C_V", TYPE_INT, CP_C_V },
    { "CP_C_F", TYPE_INT, CP_C_F },
    { "CP_C_A", TYPE_INT, CP_C_A },
    { "CP_C_W", TYPE_INT, CP_C_W },
    { "CP_C_T", TYPE_INT, CP_C_T },
    { "CP_C_ROT", TYPE_INT, CP_C_ROT },
    { "CP_C_V_LIMIT", TYPE_INT, CP_C_V_LIMIT },
    { "CP_C_W_LIMIT", TYPE_INT, CP_C_W_LIMIT },

    { "CP_C_BODY", TYPE_INT, CP_C_BODY },
    { "CP_C_BB", TYPE_INT, CP_C_BB },
    { "CP_C_E", TYPE_INT, CP_C_E },
    { "CP_C_U", TYPE_INT, CP_C_U },
    { "CP_C_SENSOR", TYPE_INT, CP_C_SENSOR },
    { "CP_C_SURFACE_V", TYPE_INT, CP_C_SURFACE_V },
    { "CP_C_COLLISION_TYPE", TYPE_INT, CP_C_COLLISION_TYPE },
    { "CP_C_GROUP", TYPE_INT, CP_C_GROUP },
    { "CP_C_LAYERS", TYPE_INT, CP_C_LAYERS },
    { "CP_C_DATA", TYPE_INT, CP_C_DATA },
//    { "INFINITY", TYPE_FLOAT, 10000000.0 },
    { "NOT_GRABABLE_MASK", TYPE_INT, NOT_GRABABLE_MASK },
    { "GRABABLE_MASK_BIT", TYPE_INT, GRABABLE_MASK_BIT },
    { "CP_NO_GROUP", TYPE_INT, CP_NO_GROUP },

    { "CP_C_MAXFORCE", TYPE_INT, CP_C_MAXFORCE },
    { "CP_C_BIASCOEF", TYPE_INT, CP_C_BIASCOEF },
    { "CP_C_MAXBIAS", TYPE_INT, CP_C_MAXBIAS },
    { "CP_C_CA", TYPE_INT, CP_C_CA },
    { "CP_C_CB", TYPE_INT, CP_C_CB },

    { "CP_ALL_LAYERS", TYPE_INT, CP_ALL_LAYERS },

    { "TYPE_NONE", TYPE_INT, TYPE_NONE },
    { "TYPE_BOX", TYPE_INT, TYPE_BOX },
    { "TYPE_CIRCLE", TYPE_INT, TYPE_CIRCLE },
    { "TYPE_CONVEX_POLYGON", TYPE_INT, TYPE_CONVEX_POLYGON },
    { "TYPE_LINE", TYPE_INT, TYPE_LINE },
    { "TYPE_EMPTY", TYPE_INT, TYPE_EMPTY },

    { "CP_C_ANCHR1", TYPE_INT, CP_C_ANCHR1 },
    { "CP_C_ANCHR2", TYPE_INT, CP_C_ANCHR2 },
    { "CP_C_DIST", TYPE_INT, CP_C_DIST },
    { "CP_C_MIN", TYPE_INT, CP_C_MIN },
    { "CP_C_MAX", TYPE_INT, CP_C_MAX },
    { "CP_C_GROOVEB", TYPE_INT, CP_C_GROOVEB },
    { "CP_C_GROOVEA", TYPE_INT, CP_C_GROOVEA },
    { "CP_C_RESTLENGTH", TYPE_INT, CP_C_RESTLENGTH },
    { "CP_C_STIFFNESS", TYPE_INT, CP_C_STIFFNESS },
    { "CP_C_DAMPING", TYPE_INT, CP_C_DAMPING },
    { "CP_C_RESTANGLE", TYPE_INT, CP_C_RESTANGLE },
    { "CP_C_ANGLE", TYPE_INT, CP_C_ANGLE },
    { "CP_C_PHASE", TYPE_INT, CP_C_PHASE },
    { "CP_C_RATCHET", TYPE_INT, CP_C_RATCHET },
    { "CP_C_RATIO", TYPE_INT, CP_C_RATIO },
    { "CP_C_RATE", TYPE_INT, CP_C_RATE },

    { "CM_PI", TYPE_INT, CM_PI },

    { NULL , 0 , 0 }
} ;


void
cpBodySlew(cpBody *body, cpVect pos, cpFloat dt)
{
	cpVect delta = cpvsub(pos, body->p);
	body->v = cpvmult(delta, 1.0f/dt);
}

//globales c



//funciones matemáticas

static int modcpflerp(INSTANCE * my, int * params)
{
    float res=cpflerp( *( float * ) &params[0],  *( float * ) &params[1],  *( float * ) &params[2]);
    return *(( int * )&(res));
}

static int modcpfclamp(INSTANCE * my, int * params)
{
    float res=cpfclamp( *( float * ) &params[0],  *( float * ) &params[1],  *( float * ) &params[2]);
    return *(( int * )&(res));
}

static int modcpflerpconst(INSTANCE * my, int * params)
{
    float res=cpflerpconst(  *( float * ) &params[0],  *( float * ) &params[1],  *( float * ) &params[2]);
    return *(( int * )&(res));
}


//FUNCIONES DE LOS VECTORES

static int modcpveql(INSTANCE * my, int * params)
{
    return cpveql(*(cpVect *) params[0], *(cpVect *) params[1]);
}

static int modcpvadd(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1 = cpvadd(*(cpVect *) params[0], *(cpVect *) params[1]);
    return 0;
}

static int modcpvsub(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1 = cpvsub(*(cpVect *) params[0], *(cpVect *) params[1]);
    return 0;
}

static int modcpvneg(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1 = cpvneg(*(cpVect *) params[0]);
    return 0;
}

static int modcpvmult(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1 = cpvmult(*(cpVect *) params[0], *( float * ) &params[1]);

    return 0;
}

static int modcpvdot(INSTANCE * my, int * params)
{
    float res = cpvdot(*(cpVect *) params[0], *(cpVect *) params[2]);
    return *(( int * )&(res));
}

static int modcpvcross(INSTANCE * my, int * params)
{
    float res = cpvcross(*(cpVect *) params[0], *(cpVect *) params[2]);
    return *(( int * )&(res));
}



static int modcpvperp(INSTANCE * my, int * params)  //un vector
{
    cpVect *v1=params[1];
    *v1  = cpvperp(*(cpVect *) params[0]);
    return 0;
}


static int modcpvrperp (INSTANCE * my, int * params)  //un vector
{
    cpVect *v1=params[1];
    *v1  = cpvrperp(*(cpVect *) params[0]);
    return 0;
}

static int modcpvnormalize (INSTANCE * my, int * params)  //un vector
{
    cpVect *v1=params[1];
    *v1  = cpvnormalize(*(cpVect *) params[0]);
    return 0;
}


static int modcpvnormalize_safe (INSTANCE * my, int * params)  //un vector
{
    cpVect *v1=params[1];
    *v1  = cpvnormalize_safe(*(cpVect *) params[0]);
    return 0;
}


static int modcpvproject(INSTANCE * my, int * params) //dos vectores
{
    cpVect *v1=params[2];
    *v1 = cpvproject(*(cpVect *) params[0], *(cpVect *) params[1]);
    return 0;
}

static int modcpvrotate(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1  = cpvrotate(*(cpVect *) params[0], *(cpVect *) params[1]);
    return 0;
}


static int modcpvunrotate(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1  = cpvunrotate(*(cpVect *) params[0], *(cpVect *) params[1]);
    return 0;
}

static int modcpvlength (INSTANCE * my, int * params)  //un vector
{
    float res = cpvlength(*(cpVect *) params[0]);
    return *(( int * )&(res));
}

static int modcpvlengthsq (INSTANCE * my, int * params)  //un vector
{
    float res = cpvlengthsq(*(cpVect *) params[0]);
    return *(( int * )&(res));
}


static int modcpvtoangle (INSTANCE * my, int * params)  //un vector
{
    float res = cpvtoangle(*(cpVect *) params[0]);
    return *(( int * )&(res));
}

static int modcpvstr (INSTANCE * my, int * params)  //un vector
{
    char * res = cpvstr(*(cpVect *) params[0]);
    char * result=string_new(res);
    return result;
}

static int modcpvforangle(INSTANCE * my, int * params)
{
    cpVect *ve=params[1];
    *ve = cpvforangle(*( float * ) &params[0]);
    return 0;
}



static int modcpvclamp(INSTANCE * my, int * params)
{
    cpVect *v1=params[2];
    *v1 = cpvclamp(*(cpVect *) params[0], *( float * ) &params[1]);
    return 0;
}

static int modcpvdist(INSTANCE * my, int * params)
{
    float res = cpvdist(*(cpVect *) params[0], *(cpVect *) params[1]);
    return *(( int * )&(res));
}

static int modcpvdistsq(INSTANCE * my, int * params)
{
    float res = cpvdistsq(*(cpVect *) params[0], *(cpVect *) params[1]);
    return *(( int * )&(res));
}

static int modcpvlerp(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    cpVect *v1=params[3];
    *v1 = cpvlerp(*(cpVect *) params[0], *(cpVect *) params[1],*( float * ) &params[2]);
    return 0;
}

static int modcpvlerpconst(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    cpVect *v1=params[3];
    *v1 = cpvlerpconst(*(cpVect *) params[0], *(cpVect *) params[1],*( float * ) &params[2]);
    return 0;
}

static int modcpvslerp(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    cpVect *v1=params[2];
    *v1 =  cpvslerp(*(cpVect *) params[0], *(cpVect *) params[1],*( float * ) &params[2]);

    return 0;
}

static int modcpvslerpconst(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    cpVect *v1=params[3];
    *v1 = cpvslerpconst(*(cpVect *) params[0], *(cpVect *) params[1],*( float * ) &params[2]);
    return 0;
}


static int modcpvnear(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    return cpvnear(*(cpVect *) params[0], *(cpVect *) params[1],*( float * ) &params[2]);
}


static int modcpv(INSTANCE * my, int * params) //vec,vec,flot, ret int
{
    cpVect  *v1=(cpVect *)params[2];
    v1->x=*( float * ) &params [0];
    v1->y=*( float * ) &params[1];
    return params[2];
}


//cpBB

static int modcpBBNew(INSTANCE * my, int * params)
{
    cpBB *ve=params[4];
    *ve = cpBBNew(*( float * ) &params[0],*( float * ) &params[1],*( float * ) &params[2],*( float * ) &params[3]);
    return 0;
}



static int modcpBBNewCircle(INSTANCE * my, int * params)
{
    cpBB *ve=params[4];
//    *ve = cpBBNew(*( float * ) &params[0],*( float * ) &params[1],*( float * ) &params[2],*( float * ) &params[3]);
    *ve=cpBBNewForCircle(cpv(*( float * ) &params[0],*( float * ) &params[1]),*( float * ) &params[2]);
    return 0;
}

static int modcpBBintersects(INSTANCE * my, int * params)
{
    return cpBBIntersects(*(cpBB *)params[0],*(cpBB *)params[1]);
}



static int modcpBBcontainsBB(INSTANCE * my, int * params)
{
    return cpBBContainsBB(*(cpBB *)params[0],*(cpBB *)params[1]);
}



static int modcpBBmerge(INSTANCE * my, int * params)
{
    cpBB *ve=params[2];
    (*ve) = cpBBNew(
		cpfmin((*(cpBB *)params[0]).l, (*(cpBB *)params[1]).l),
		cpfmin((*(cpBB *)params[0]).b, (*(cpBB *)params[1]).b),
		cpfmax((*(cpBB *)params[0]).r, (*(cpBB *)params[1]).r),
		cpfmax((*(cpBB *)params[0]).t, (*(cpBB *)params[1]).t)
	);

    return 0;
}


static int modcpBBcontainsVect(INSTANCE * my, int * params)
{
    return cpBBContainsVect(*(cpBB *)params[0], *(cpVect *)params[1]);
}



static int modcpBBexpand(INSTANCE * my, int * params)
{
    cpBB *ve=params[2];
    *ve =cpBBNew(
		cpfmin((*(cpBB *)params[0]).l, (*(cpVect *)params[0]).x),
		cpfmin((*(cpBB *)params[0]).b, (*(cpVect *)params[0]).y),
		cpfmax((*(cpBB *)params[0]).r, (*(cpVect *)params[0]).x),
		cpfmax((*(cpBB *)params[0]).t, (*(cpVect *)params[0]).y)
	);
    return 0;
}



static int modcpBBClampVect(INSTANCE * my, int * params)
{
    cpVect *aux=params[2];
    *aux=cpBBClampVect(*(cpBB *)params[0], *(cpVect *)params[1]);
    return 0;
}



static int modcpBBWrapVect(INSTANCE * my, int * params)
{
    cpVect *aux=params[2];
    *aux=cpBBWrapVect(*(cpBB *)params[0], *(cpVect *)params[1]);
    return 0;
}

//cpBody



static int modcpMomentForCircle(INSTANCE * my, int * params)
{
    float  res = cpMomentForCircle(*( float * ) &params[0],*( float * ) &params [1],*( float * ) &params [2],*(cpVect  *)params[3]);
    return *(( int * )&res);

}



static int modcpMomentForSegment(INSTANCE * my, int * params)
{
    float  res = cpMomentForSegment( *( float * ) &params[0],*(cpVect *)params[1],*(cpVect *)params[2]);
    return *(( int * )&res);

}



static int modcpMomentForPoly(INSTANCE * my, int * params)
{
    float  res = cpMomentForPoly(*( float * ) &params[0],params[1],params[2],*(cpVect *)params[3]);

    return *(( int * )&res);

}

static int modcpMomentForCircle1(INSTANCE * my, int * params)
{
    float  res = cpMomentForCircle(*( float * ) &params[0],*( float * ) &params [1],*( float * ) &params [2],cpv(*( float * ) &params [3],*( float * ) &params [4]));
    return *(( int * )&res);

}

static int modcpMomentForSegment1(INSTANCE * my, int * params)
{
    float  res = cpMomentForSegment(*( float * ) &params[0],cpv(*( float * ) &params[1],*( float * ) &params[2]),cpv(*( float * ) &params[3],*( float * ) &params[4]));
    return *(( int * )&res);

}

static int modcpMomentForPoly1(INSTANCE * my, int * params)
{
    cpVect  a=cpv(*( float * ) &params[3],*( float * ) &params[4]);
    float  res = cpMomentForPoly(*( float * ) &params[0],params[1],params[2],a);
    return *(( int * )&res);
}

static int modcpMomentForBox(INSTANCE * my, int * params)
{
    float  res = cpMomentForBox(*( float * ) &params[0], *( float * ) &params [1], *( float * ) &params [2]);
    return *(( int * )&res);
}

static int modcpAreaForCircle(INSTANCE * my, int * params)
{
    float res = cpAreaForCircle(*( float * ) &params[0],*( float * ) &params [1]) ;
    return *(( int * )&res);
}

static int modcpAreaForSegment(INSTANCE * my, int * params)
{
    float  res = cpAreaForSegment(cpv(*( float * ) &params[0],*( float * ) &params[1]),cpv(*( float * ) &params[2],*( float * ) &params[3]),*( float * ) &params[4]);
    return *(( int * )&res);

}

static int modcpAreaForPoly(INSTANCE * my, int * params)
{
    float  res = cpAreaForPoly(params[0],params[1]);
    return *(( int * )&res);

}


//-----------------------------------------------------------------------------------------------------------------------------------



static int modDefcpShape(INSTANCE * my, int * params)
{
    float  flo2;// = *( float * ) &params[0];
    cpCollisionType  ct ;//= *( cpCollisionType * ) &params[2];
    cpDataPointer *dP;
    cpVect * vAux, * vAux2;
    cpVect  vec;
    cpBB *bounding;
    cpBB bound;
    cpShape * espacio = ( cpShape * ) params[0];
    int res=params[1];
    switch (res)
    {
    case CP_C_BODY:
        espacio->body=(cpBody *) params[2];
        break;
    case CP_C_BB:
        bounding=params[2];
        espacio->bb=*bounding;
        break;
    case CP_C_E:
        flo2= *( float * ) &params[2];
        espacio->e=flo2;
        break;
    case CP_C_U:
        flo2= *( float * ) &params[2];
        espacio->u=flo2;
        break;
    case CP_C_SENSOR:
        res= (int) params[2];
        espacio->sensor=res;
        break;
    case CP_C_SURFACE_V:
        vAux=params[2];
        espacio->surface_v=*vAux;
        break;
    case CP_C_COLLISION_TYPE:
        res = params[2];
        espacio->collision_type=res;
        break;
    case CP_C_GROUP:
        res= (int) params[2];
        espacio->group=res;
        break;
    case CP_C_LAYERS:
        res= (int) params[2];
        espacio->layers=res;
        break;
    case CP_C_DATA:
        dP= (cpDataPointer *) params[2];
        espacio->data=dP;
        break;



    }
    return 0;
}

static int modGetcpShape(INSTANCE * my, int * params)
{
    cpShape * espacio = ( cpShape * ) params[0];
    int res=params[1];
    cpVect * vc;
    cpBB *bb;
    float a;
    switch (res)
    {
    case CP_C_BODY:
        return (int) espacio->body;
        break;
    case CP_C_BB:
        bb=params[2];
        *bb=espacio->bb;
        return 0;
        break;
    case CP_C_E:
        a=espacio->e;
        return *(int*)&a;
        break;
    case CP_C_U:
        a=espacio->u;
        return *(int*)&a;
        break;
    case CP_C_SENSOR:
        return (int)espacio->sensor;
        break;
    case CP_C_SURFACE_V:
        vc=params[2];
        *vc=espacio->surface_v;
        break;
    case CP_C_COLLISION_TYPE:
        return(espacio->collision_type);

        break;
    case CP_C_GROUP:
        return (espacio->group);
        break;
    case CP_C_LAYERS:
        return (espacio->layers);
        break;
    case CP_C_DATA:
        return (int) espacio->data;
        break;



    }
    return 0;
}


static int modcpPolyShapeGetVert(INSTANCE * my, int * params)
{
    cpVect *a=params[2];
    *a = cpPolyShapeGetVert(( cpShape * ) params[0], params[1]);
    return (0);
}

static int modcpResetShapeIdCounter(INSTANCE * my, int * params)
{
    cpResetShapeIdCounter();
    return (0);
}


static int modcpShapeCacheBB(INSTANCE * my, int * params)
{
    cpBB *ve=params[1];
    *ve= cpShapeCacheBB(( cpShape * ) params[0]);
    return (0);
}


static int modcpPolyShapeGetNumVerts(INSTANCE * my, int * params)
{
    return cpPolyShapeGetNumVerts(( cpShape * ) params[0]);
}


static int modcpSegmentShapeGetA(INSTANCE * my, int * params)
{
    cpVect *aux=params[1];
    *aux=cpSegmentShapeGetA((cpShape *)params[0]);
    return 0;
}

static int modcpSegmentShapeGetB(INSTANCE * my, int * params)
{
    cpVect *aux=params[1];
    *aux=cpSegmentShapeGetB((cpShape *)params[0]);
    return 0;
}


static int modcpCircleShapeGetOffset(INSTANCE * my, int * params)
{
    cpVect *aux=params[1];
    *aux=cpCircleShapeGetOffset((cpShape *)params[0]);
    return 0;
}


static int modcpSegmentShapeGetNormal(INSTANCE * my, int * params)
{
    cpVect *aux=params[1];
    *aux=cpSegmentShapeGetNormal((cpShape *)params[0]);
    return 0;
}



static int modcpCircleShapeGetRadius(INSTANCE * my, int * params)
{
    float res=cpCircleShapeGetRadius((cpShape *)params[0]);
    return *((int *)&res);
}


static int modcpSegmentShapeGetRadius(INSTANCE * my, int * params)
{
    float res=cpSegmentShapeGetRadius((cpShape *)params[0]);
    return *((int *)&res);
}


static int modcpCentroidForPoly(INSTANCE * my, int * params)
{
    cpVect *aux=params[2];
    *aux=cpCentroidForPoly(params[0],params[1]);
    return 0;
}


static int modcpRecenterPoly(INSTANCE * my, int * params)
{
    cpRecenterPoly(params[0],params[1]);
    return 0;
}


//Collision points y raycast
static int modcpSpacePointQueryFirst(INSTANCE * my, int * params)
{
    cpShape *res =cpSpacePointQueryFirst(modChipmunk_cpEspacio, *(cpVect *)params[0], params[1], params[2]);
    return (int) res;
}

static int modcpShapePointQuery(INSTANCE * my, int * params)
{
    cpVect *p=params[1];
    int res = (int) cpShapePointQuery((cpShape *)params[0], *(cpVect *)params[1]);
    return  res;
}

static int modcpShapeSegmentQuery(INSTANCE * my, int * params)
{
    cpSegmentQueryInfo info;
    cpsegmentqueryinfoB *inf;
    int res = (int) cpShapeSegmentQuery((cpShape *)params[0], *(cpVect *)params[1], *(cpVect *)params[2], &info);
    inf=params[3];
    inf->x=info.n.x;
    inf->y=info.n.y;
    inf->shape=(int)info.shape;
    inf->t=info.t;
    inf->n.x=info.n.x;
    inf->n.y=info.n.y;
    if (info.shape->body->data!=NULL)
    {
        DataPointer din=(DataPointer)info.shape->body->data;
        inf->id= din->father ;
    }
    else
    {
        inf->id=0;
    }

    return  res;
}
static int modcpSegmentQueryHitPoint(INSTANCE * my, int * params)
{
    cpVect *a=params[0],*b=params[1];
    cpSegmentQueryInfo info;
    cpsegmentqueryinfoB *inf=params[2];
    info.t=inf->t;
    info.n=cpv(inf->x,inf->y);
    info.shape=(cpShape *)inf->shape;
    cpVect resAux = cpSegmentQueryHitPoint(*a, *b, info);
    cpVect *vc=params[3];
    *vc=resAux;
    return  0;
}

static int modcpSegmentQueryHitDist(INSTANCE * my, int * params)
{
    cpVect *a=params[0],*b=params[1];
    cpSegmentQueryInfo info;
    cpsegmentqueryinfoB *inf=params[2];
    info.t=inf->t;
    info.n=cpv(inf->x,inf->y);
    info.shape=(cpShape *)inf->shape;
    float res = cpSegmentQueryHitDist(*a, *b, info);

    return  *(int *) &res;
}

static int modcpSpaceSegmentQueryFirst(INSTANCE * my, int * params)
{
    cpVect *a=params[0],*b=params[1];
    cpSegmentQueryInfo info;
    cpShape *res = cpSpaceSegmentQueryFirst(modChipmunk_cpEspacio,*a,*b,params[2],params[3],&info);
    cpsegmentqueryinfoB *inf=params[4];
    inf->x=info.n.x;
    inf->y=info.n.y;
    inf->shape=(int)info.shape;
    inf->t=info.t;
    inf->n.x=info.n.x;
    inf->n.y=info.n.y;
    if (info.shape->body->data!=NULL)
    {
        DataPointer din=(DataPointer)info.shape->body->data;
        inf->id= din->father ;
    }
    else
    {
        inf->id=0;
    }

    return  (int ) res;
}


//funciones ide body

static int modSlew(INSTANCE * my, int * params)
{
    cpBodySlew(LOCDWORD(mod_chipmunk,my,LOC_BODY), cpv(*(float *)&params[0],*(float *)&params[1]), *(float *)&params[2]);
    return 0;
}

static int modUpdateVelocity(INSTANCE * my, int * params)
{
    cpBodyUpdateVelocity(LOCDWORD(mod_chipmunk,my,LOC_BODY), cpv(*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X),*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)), *(float *)GLOADDR(mod_chipmunk,GLO_DAMPING),*(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL)/(float)GLODWORD(mod_chipmunk,GLO_PHRESOLUTION));
    return 0;
}

static int modUpdateVelocityAg(INSTANCE * my, int * params)
{
    INSTANCE * i=instance_get(params[0]);
    cpBodyUpdateVelocity(LOCDWORD(mod_chipmunk,i,LOC_BODY), cpv(*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X),*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)), *(float *)GLOADDR(mod_chipmunk,GLO_DAMPING),*(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL)/(float)GLODWORD(mod_chipmunk,GLO_PHRESOLUTION));
    return 0;
}

static int modUpdateVelocity2(INSTANCE * my, int * params)
{
    cpBodyUpdateVelocity(LOCDWORD(mod_chipmunk,my,LOC_BODY), cpv(*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X),*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)), *(float *)&params[0],*(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL)/(float)GLODWORD(mod_chipmunk,GLO_PHRESOLUTION));
    return 0;
}

static int modLocal2World(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpVect vc=cpBodyLocal2World(LOCDWORD(mod_chipmunk, b,LOC_BODY), cpv(*(float *)&params[1],*(float *)&params[2]));
    int *x, *y;
    x=params[3];
    *x=vc.x;
    y=params[4];
    *y=vc.y;
    return 0;
}

static int modWorld2Local(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpVect vc=cpBodyWorld2Local(LOCDWORD(mod_chipmunk, b,LOC_BODY), cpv(*(float *)&params[1],*(float *)&params[2]));
    int *x, *y;
    x=params[3];
    *x=vc.x;
    y=params[4];
    *y=vc.y;

    return 0;
}

static int modApplyImpulse(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBodyApplyImpulse(LOCDWORD(mod_chipmunk, b,LOC_BODY), cpv(*(float *)&params[1],*(float *)&params[2]),cpv(*(float *)&params[3],*(float *)&params[4]));
    return 0;
}

static int modSpaceUseSpatialHash(INSTANCE * my, int * params){
    cpSpaceUseSpatialHash (modChipmunk_cpEspacio,*(float *)&params[1],params[1]);
    modChipmunk_espacioHash=1;
    return 0;
}

static int modResetForces(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBodyResetForces(LOCDWORD(mod_chipmunk, b,LOC_BODY));
    return 0;
}

static int modApplyForce(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBodyApplyForce(LOCDWORD(mod_chipmunk, b,LOC_BODY), cpv(*(float *)&params[1],*(float *)&params[2]),cpv(*(float *)&params[3],*(float *)&params[4]));
    return 0;
}

static int modSleep(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    if (!cpBodyIsSleeping(LOCDWORD(mod_chipmunk, b,LOC_BODY)))
        cpBodySleep(LOCDWORD(mod_chipmunk, b,LOC_BODY));
    return 0;
}

static int modActivate(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBodyActivate(LOCDWORD(mod_chipmunk, b,LOC_BODY));
    return 0;
}

static int modSleepWithGroup(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    INSTANCE * c = instance_get( params[1] ) ;
    if (!cpBodyIsSleeping(LOCDWORD(mod_chipmunk, b,LOC_BODY)))
        cpBodySleepWithGroup(LOCDWORD(mod_chipmunk, b,LOC_BODY),LOCDWORD(mod_chipmunk, c,LOC_BODY));
    return 0;
}

static int modActivateStatic(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
        cpBodyActivateStatic(LOCDWORD(mod_chipmunk, b,LOC_BODY),params[1]);
    return 0;
}


static int modIsStatic(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    return cpBodyIsStatic(LOCDWORD(mod_chipmunk, b,LOC_BODY));
}


static int modIsRogue(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    return cpBodyIsRogue(LOCDWORD(mod_chipmunk, b,LOC_BODY));
}

static int modIsSleeping(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    return cpBodyIsSleeping(LOCDWORD(mod_chipmunk, b,LOC_BODY));
}



static int modShapeCacheBB(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBB *ve=params[1];
    *ve= cpShapeCacheBB((cpShape *)LOCDWORD(mod_chipmunk, b,LOC_SHAPE));
    return (0);
}

//boundingbox ids
static int modIntersects(INSTANCE * my, int * params)
{
    INSTANCE * a = instance_get( params[0] ) ;
    INSTANCE * b = instance_get( params[1] ) ;
    return cpBBIntersects(cpShapeCacheBB(LOCDWORD(mod_chipmunk, a,LOC_SHAPE)),cpShapeCacheBB(LOCDWORD(mod_chipmunk, b,LOC_SHAPE)));
}

static int modContains(INSTANCE * my, int * params)
{
    INSTANCE * a = instance_get( params[0] ) ;
    INSTANCE * b = instance_get( params[1] ) ;
    return (int)cpBBContainsBB(cpShapeCacheBB(LOCDWORD(mod_chipmunk, a,LOC_SHAPE)),cpShapeCacheBB(LOCDWORD(mod_chipmunk, b,LOC_SHAPE)));;
}

static int modContainsVec(INSTANCE * my, int * params)
{
    INSTANCE * a = instance_get( params[0] ) ;
    return (int)cpBBContainsVect(cpShapeCacheBB(LOCDWORD(mod_chipmunk, a,LOC_SHAPE)),cpv(*(float *)&params[1],*(float *)&params[2]));
}

//locales de espacio


modChipmunkStruct_nodo * indiceConstraints=NULL;
int idConstraints=-1;

static int modgetConstraints(INSTANCE * my, int * params)
{
//    INSTANCE * a = instance_get( params[0] ) ;
//    cpBody *bod =(cpBody *)LOCDWORD( mod_chipmunk, a, LOC_BODY );
//    int z,i;
//    idConstraints=params[0];
//    if (bod)
//    {
//        if (indiceConstraints==NULL || indiceConstraints->sig== NULL || idConstraints!= params[0])
//            indiceConstraints=((DataPointer)bod->data)->Constraints;
//            else
//        indiceConstraints=indiceConstraints->sig;
//
//    }
//    if (indiceConstraints->sig==NULL)
//        return NULL;
//    return indiceConstraints->sig->elem;
return 0;
}


int indiceShapes;
DataPointer dataShapes;
static int modgetShapes(INSTANCE * my, int * params)
{
//    INSTANCE * a = instance_get( params[0] ) ;
//    cpBody *bod =(cpBody *)LOCDWORD( mod_chipmunk, a, LOC_BODY );
//    int z,i;
//    if (bod)
//    {
//        DataPointer elem= (DataPointer)bod->data;
//
//        if (elem!=dataShapes)
//            indiceShapes=0;
//        dataShapes=elem;
//        if (elem->nShapes==0)
//            return 0;
//
//        if (indiceShapes>=elem->nShapes)
//        {
//            indiceShapes=0;
//            return 0;
//        }
//
//        cpConstraint * res = elem->Shapes[indiceShapes];
//        indiceShapes++;
//        return (int) res;
//
//    }
    return 0;
}

static int modSpacePointQueryFirst(INSTANCE * my, int * params)
{

    cpShape * a=cpSpacePointQueryFirst(modChipmunk_cpEspacio, cpv(*(float*)&params[0],*(float*)&params[1]), params[2], params[3]);
    int id=0;
    if (a!=NULL)
    {
        if (a->body->data!=NULL)
        {
            DataPointer dat= (DataPointer) (a->body->data);
            id=dat->father;
        }
    }
    return id;
}

static int modSpaceSegmentQueryFirst(INSTANCE * my, int * params)
{
    cpLayers lay=params[4];
    cpGroup group = params[5];
    cpSegmentQueryInfo info;
    int id=-1;
    cpShape *a = cpSpaceSegmentQueryFirst(modChipmunk_cpEspacio,cpv(*(float*)&params[0],*(float*)&params[1]),cpv(*(float*)&params[2],*(float*)&params[3]),lay,group,&info);
    if (a!=NULL)
    {
        cpsegmentqueryinfoB *inf=params[6];
        inf->x=info.n.x;
        inf->y=info.n.y;
        inf->shape=(int)info.shape;
        inf->t=info.t;
        if (a->body->data)
        {
            DataPointer dat= (DataPointer) (a->body->data);
            id=dat->father;

        }
        else
            return 0;
    }
    return id;
}





/*void eliminaContactPointSetM(void *ptr, void *data)
{
    cpContactPointSetM * setM =(cpContactPointSetM *)ptr;
    free(setM);
}*/




static int modgetColInfo(INSTANCE * my, int * params)
{
    modChipmunkStruct_colHand* col=(modChipmunkStruct_colHand*)params[0];
    //col->colisiones=col->colisiones;
    if (modChipmunk_ArregloCantidad(col->colisiones)>0)
    {
        cpContactPointSetM * set =(cpContactPointSetM *)modChipmunk_ArregloPop(col->colisiones);
        cpContactPointSetM * setM =(cpContactPointSetM *)params[1];
        setM->id1=set->id1;
        setM->id2=set->id2;
        setM->shape1=set->shape1;
        setM->shape2=set->shape2;
        setM->count=set->count;

        int i;
        for (i=0; i<set->count; i++)
        {
            setM->points[i].Point.x=set->points[i].Point.x;
            setM->points[i].Point.y=set->points[i].Point.y;
            setM->points[i].normal.x=set->points[i].normal.x;
            setM->points[i].normal.y=set->points[i].normal.y;
            setM->points[i].dist=set->points[i].dist;
        }
        free(set);
    }
    else
        return 0;

    return 1;
}

static int modSegmentQueryHitPoint(INSTANCE * my, int * params)
{
    cpSegmentQueryInfo info;
    cpsegmentqueryinfoB *inf=params[4];
    info.t=inf->t;
    info.n=cpv(inf->x,inf->y);
    info.shape=(cpShape *)inf->shape;
    cpVect resAux = cpSegmentQueryHitPoint(cpv(params[0],params[1]),cpv(params[2],params[3]),info);
    int * x=params[5];
    int * y=params[6];
    *x=resAux.x;
    *y=resAux.y;
    return 1;
}

static int modSegmentQueryHitDist(INSTANCE * my, int * params)
{
    cpSegmentQueryInfo info;
    cpsegmentqueryinfoB *inf=params[4];
    info.t=inf->t;
    info.n=cpv(inf->x,inf->y);

    info.shape=(cpShape *)inf->shape;
    float res = cpSegmentQueryHitDist(cpv(params[0],params[1]),cpv(params[2],params[3]),info);
    return *(int *)&res;
}

static int moddeg2rad(INSTANCE * my, int * params)
{
    return ((*(float*)&params[0])/180000)*CM_PI;
}

//agregan cuerpos est[aticos al fondo

static int modaddInanimateShape(INSTANCE * my, int * params)
{
    if (params[0]<=TYPE_NONE || params[0]>=TYPE_EMPTY)
        return 0;
    cpBody * bod=modChipmunk_cpEspacio->staticBody;
    cpShape * sha;

    if (params[0]== TYPE_BOX || params[0]== TYPE_CONVEX_POLYGON)
    {
        sha= cpPolyShapeNew(bod,params[3], params[4], cpv(*(float *)&params[1],*(float *)&params[2]));
    }
    else
    {
        switch (params[0])
        {
        case TYPE_CIRCLE:
            sha= cpCircleShapeNew(bod, *(float *)&params[3], cpv(*(float *)&params[1],*(float *)&params[2]));
            break;
        case TYPE_LINE:
            sha= cpSegmentShapeNew(bod, cpv(*(float *)&params[1],*(float *)&params[2]), cpv(*(float *)&params[3],*(float *)&params[4]),*(float *)&params[5]);
            break;
        }
    }
    sha->e=1;
    sha->u=1;
    cpSpaceAddStaticShape(modChipmunk_cpEspacio, sha);
  //  LLagrega(modChipmunk_ListaEstaticosFondo, sha);
    return (int) sha;
}

void modChipmunkEliminaEstatico(void * parametro){
    cpSpaceRemoveShape(modChipmunk_cpEspacio,parametro);
    cpShapeFree(parametro);
}

int modChipmunkRemoveInanimateShape(INSTANCE * my, int * params){
    //LLelimina(modChipmunk_ListaEstaticosFondo,params[0], modChipmunkCompara, modChipmunkEliminaEstatico,1);
    cpSpaceRemoveShape(modChipmunk_cpEspacio,params[0]);
    cpShapeFree(params[0]);
    return 0;
}


static int modGetOptimalInertia(INSTANCE * my, int * params)
{
    float sha=0;
    cpShape * shape=params[1];
    if (params[0]==TYPE_CONVEX_POLYGON || params[0]==TYPE_BOX)
    {
        int cant=cpPolyShapeGetNumVerts(shape),z;
        cpVect ver[cant];
        for (z=0; z<cant; z++){
            ver[z]=cpPolyShapeGetVert(shape,z);
        }
        sha=cpMomentForPoly(shape->body->m, cant, &ver, cpv(0,0));
    }
    else
    {
        switch (params[0])
        {
        case TYPE_CIRCLE:
            sha= cpMomentForCircle(shape->body->m, 0, cpCircleShapeGetRadius(shape), cpCircleShapeGetOffset(shape));
            break;
        case TYPE_LINE:
            sha= cpMomentForSegment(shape->body->m, cpSegmentShapeGetA(shape), cpSegmentShapeGetB(shape));
            break;
        }
    }
    return *(int *)&sha;
}


static int modsetEndPointsLine(INSTANCE * my, int * params)
{
    cpSegmentShapeSetEndpoints((cpShape*)LOCDWORD( mod_chipmunk,  my, LOC_SHAPE), cpv(*(float*)&params[0],*(float*)&params[1]),cpv(*(float*)&params[2],*(float*)&params[3]));
    return 0;
}


static int modsetRadiusLine(INSTANCE * my, int * params)
{
    cpSegmentShapeSetRadius((cpShape*)LOCDWORD( mod_chipmunk,  my, LOC_SHAPE), *(float*)&params[0]);
    return 0;
}

static int modsetOffsetCircle(INSTANCE * my, int * params)
{
    cpCircleShapeSetOffset((cpShape*)LOCDWORD( mod_chipmunk,  my, LOC_SHAPE), cpv(*(float*)&params[0],*(float*)&params[1]));
    return 0;
}

static int modsetRadiusCircle(INSTANCE * my, int * params)
{
    cpCircleShapeSetRadius((cpShape*)LOCDWORD( mod_chipmunk,  my, LOC_SHAPE), *(float*)&params[0]);
    return 0;
}

static int modsetVertConvexPoligon(INSTANCE * my, int * params)
{
    cpPolyShapeSetVerts((cpShape*)LOCDWORD( mod_chipmunk,  my, LOC_SHAPE), params[2], params[3],cpv(*(float*)&params[0],*(float*)&params[1]));
    return 0;
}

static int modsetEndPointsLineI(INSTANCE * my, int * params)
{
    cpSegmentShapeSetEndpoints((cpShape*)params[0], cpv(*(float*)&params[1],*(float*)&params[2]),cpv(*(float*)&params[3],*(float*)&params[4]));
    return 0;
}


static int modsetRadiusLineI(INSTANCE * my, int * params)
{
    cpSegmentShapeSetRadius((cpShape*)params[0], *(float*)&params[1]);
    return 0;
}

static int modsetOffsetCircleI(INSTANCE * my, int * params)
{
    cpCircleShapeSetOffset((cpShape*)params[0], cpv(*(float*)&params[1],*(float*)&params[2]));
    return 0;
}

static int modsetRadiusCircleI(INSTANCE * my, int * params)
{
    cpCircleShapeSetRadius((cpShape*)params[0], *(float*)&params[1]);
    return 0;
}

static int modsetVertConvexPoligonI(INSTANCE * my, int * params)
{
    cpPolyShapeSetVerts((cpShape*)params[0], params[3], params[4],cpv(*(float*)&params[1],*(float*)&params[2]));
    return 0;
}


//static void postStepRemove(void *a, void *b, void *unused)
//{
//    int z,i;
//    if (lista!=NULL)
//    {
//        cpBool estatico;
//        for (; modChipmunk_numLista>0; modChipmunk_numLista--)
//        {
//            DataPointer elem= lista[modChipmunk_numLista];
//            if (elem->body)
//            {
//                estatico=cpBodyIsStatic(elem->body);
//                if (!estatico)
//                    cpSpaceRemoveBody(modChipmunk_cpEspacio, elem->body);
//            }
//            else
//                estatico=1;
//            for (z=0; z<elem->nShapes; z++)
//            {
//                if (estatico)
//                {
//                    cpSpaceRemoveStaticShape(modChipmunk_cpEspacio, (cpShape *)elem->Shapes[z]);
//                }
//                else
//                    cpSpaceRemoveShape(modChipmunk_cpEspacio, (cpShape *)elem->Shapes[z]);
//            }
//
//            if (elem->body)
//                cpBodyFree((cpBody *)elem->body);
//
//            for (z=0; z<elem->nShapes; z++)
//                cpShapeFree(elem->Shapes[z]);
//            if (lista[modChipmunk_numLista]->Shapes!=   NULL)
//                free(lista[modChipmunk_numLista]->Shapes);
//            free(lista[modChipmunk_numLista]);
//        }
//
//
//    }
//    if (modChipmunk_elim)
//    {
//        free(lista);
//        lista=NULL;
//    }
//
//
//}
//static modInitLista()
//{
//    lista=malloc(sizeof(DataPointer));
//    modChipmunk_elim=0;
//    modChipmunk_numLista=0;
//}
//
//static modAddLista(DataPointer a)
//{
//    if (lista!=NULL)
//    {
//        modChipmunk_numLista++;
//        lista=realloc(lista,sizeof(DataPointer)*(modChipmunk_numLista+1));
//        lista[modChipmunk_numLista]=a;
//        cpSpaceAddPostStepCallback(modChipmunk_cpEspacio, (cpPostStepFunc)postStepRemove, NULL, NULL);
//    }else{
//        modInitLista();
//        modAddLista(a);
//    }
//
//}
//
//static int modFreeLista()
//{
//    // cpSpaceAddPostStepCallback(modChipmunk_cpEspacio, (cpPostStepFunc)postStepRemove, NULL, NULL);
//    //cpSpaceStep(modChipmunk_cpEspacio, *(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL));
//    postStepRemove(NULL,NULL,NULL);
//    modChipmunk_elim=1;
//    return  0;
//}


void cpBoCoItFunc(cpBody *body, cpConstraint *c, void *data){
    cpSpaceRemoveConstraint(modChipmunk_cpEspacio, c);
    cpConstraintFree(c);
    //printf("remueve constraint %p\n",c); fflush(stdout);
}

void cpBoShaItFunc(cpBody *body, cpShape *s, void *data){
    cpSpaceRemoveShape(modChipmunk_cpEspacio, s);
    cpShapeFree(s);
   // printf("remueve shape %p\n",s); fflush(stdout);
}

static void remueveProcesoListaProcesos(DataPointer a)
{
    DataPointer aux= a->sig->sig;
    //modAddLista(a->sig);
    cpBool estatico=0;
    int z;
    DataPointer elem= a->sig;
#ifdef DEBUG
    printf("aquiA\n"); fflush(stdout);
#endif
 //   LLeliminaTodo(&elem->Constraints,eliminaConstraint,0);




    if (elem->body)
    {
        cpBodyEachConstraint(elem->body, cpBoCoItFunc, NULL);
        cpBodyEachShape(elem->body, cpBoShaItFunc, NULL);
        estatico=cpBodyIsStatic(elem->body);
        if (!estatico)
            cpSpaceRemoveBody(modChipmunk_cpEspacio, elem->body);
    }
    else
        estatico=1;
//    for (z=0; z<elem->nShapes; z++)
//    {
//            cpSpaceRemoveShape(modChipmunk_cpEspacio, (cpShape *)elem->Shapes[z]);
//    }

    if (elem->body)
        cpBodyFree((cpBody *)elem->body);

//    for (z=0; z<elem->nShapes; z++)
//        cpShapeFree(elem->Shapes[z]);
    free (a->sig);
    a->sig=aux;

}

static void inicializaListaProcesos( )
{
    modChipmunk_ListaCuerpos=malloc(sizeof(modChipmunkStruct_Data_Pointer));
    modChipmunk_ListaCuerpos->sig=NULL;
}
int existe(int id)
{
    INSTANCE * i, * ctx;

    if ( id == 0 ) return 0;

    if ( id >= FIRST_INSTANCE_ID )
    {
        i = instance_get( id ) ;
        if ( i && ( LOCDWORD( mod_chipmunk, i, LOC_RSTATUS ) & ~STATUS_WAITING_MASK ) >= STATUS_RUNNING ) return 1;
        return 0;
    }

    ctx = NULL;
    while ( ( i = instance_get_by_type( id, &ctx ) ) )
    {
        if ( ( LOCDWORD( mod_chipmunk, i, LOC_RSTATUS ) & ~STATUS_WAITING_MASK ) >= STATUS_RUNNING ) return 1;
    }

    return 0;


}
static void eliminaListaProcesos()
{
    DataPointer aux;
    for (aux= modChipmunk_ListaCuerpos; aux->sig!=NULL;)
    {
        if (existe(aux->sig->father))
        {
            INSTANCE * i = instance_get( aux->sig->father) ;
            LOCDWORD( mod_chipmunk, i, LOC_BODY )=0;
            LOCDWORD( mod_chipmunk, i, LOC_SHAPE )=0;
            LOCDWORD( mod_chipmunk, i, LOC_SHAPETYPE )=TYPE_NONE;
        }
        //modAddLista(aux->sig);
        remueveProcesoListaProcesos(aux);
    }
    free(modChipmunk_ListaCuerpos);
    modChipmunk_ListaCuerpos=NULL;
}

void ActualizaShapes(cpBody *body, cpShape *shape, void *i){
    INSTANCE * ins=i;
    shape->e=*(float*)LOCADDR( mod_chipmunk, ins, LOC_ELASTICITY );
            shape->u=*(float*)LOCADDR( mod_chipmunk, ins, LOC_FRICTION );
            shape->group=LOCDWORD( mod_chipmunk, ins, LOC_GROUP );
            shape->layers=LOCDWORD( mod_chipmunk, ins, LOC_LAYERS );
            shape->collision_type=LOCDWORD( mod_chipmunk, ins, LOC_COLLISIONTYPE );
            shape->sensor=LOCDWORD( mod_chipmunk, ins, LOC_SENSOR );
}

void actualizaProcess(DataPointer dat)
{
    int gEstatico=0;
    cpBody * cuerpo = dat->body;
    int estatico=cpBodyIsStatic(cuerpo);
    INSTANCE * ins= instance_get( dat->father ) ;
    if (!estatico && !cpBodyIsSleeping(cuerpo))
    {
        if (cuerpo->m!=*(float*)LOCADDR( mod_chipmunk, ins, LOC_MASS ))
            cpBodySetMass(cuerpo,*(float*)LOCADDR( mod_chipmunk, ins, LOC_MASS ));

        if (cuerpo->i!=*(float*)LOCADDR( mod_chipmunk, ins, LOC_INERTIA ))
            cpBodySetMoment(cuerpo,*(float*)LOCADDR( mod_chipmunk, ins, LOC_INERTIA ));
    }
    float resolucion=LOCDWORD( mod_chipmunk, ins, LOC_RESOLUTION )>0? LOCDWORD( mod_chipmunk, ins, LOC_RESOLUTION ) : 1.0f;

    if (LOCDWORD( mod_chipmunk, ins, LOC_X )!= dat->x || LOCDWORD( mod_chipmunk, ins, LOC_Y )!= dat->y)
    {
        cuerpo->p.x=LOCDWORD( mod_chipmunk, ins, LOC_X )/resolucion;
        dat->x=LOCDWORD( mod_chipmunk, ins, LOC_X );
        dat->y=LOCDWORD( mod_chipmunk, ins, LOC_Y );
        cuerpo->p.y=LOCDWORD( mod_chipmunk, ins, LOC_Y )/resolucion;
        if (estatico)
            gEstatico=1;
    }
    else
    {
        dat->x=LOCDWORD( mod_chipmunk, ins, LOC_X )=cuerpo->p.x*resolucion;
        dat->y=LOCDWORD( mod_chipmunk, ins, LOC_Y )=cuerpo->p.y*resolucion;
    }
//printf("entra\n %d",dat->angle); fflush(stdout);
    if (LOCDWORD( mod_chipmunk, ins, LOC_ANGLE )!=dat->angle)
    {
        dat->angle=LOCDWORD( mod_chipmunk, ins, LOC_ANGLE );
        cpBodySetAngle(cuerpo,-modChipmunkdeg2rad(dat->angle));
        if (estatico)
            gEstatico=1;
    }
    else
    {

//printf("%d entra No modificado\n",modChipmunkrad2deg(cuerpo->a)); fflush(stdout);
        LOCDWORD( mod_chipmunk, ins, LOC_ANGLE )=-modChipmunkrad2deg(cuerpo->a);
        dat->angle=LOCDWORD( mod_chipmunk, ins, LOC_ANGLE );

     //   printf("%d entra No modificado B\n",cuerpo->a); fflush(stdout);

    }


//    int z;
//    for (z=0; z<dat->nShapes; z++)
//    {
//        cpShape* shape=dat->Shapes[z];
//        if (shape!=NULL)
//        {
//            shape->e=*(float*)LOCADDR( mod_chipmunk, ins, LOC_ELASTICITY );
//            shape->u=*(float*)LOCADDR( mod_chipmunk, ins, LOC_FRICTION );
//            shape->group=LOCDWORD( mod_chipmunk, ins, LOC_GROUP );
//            shape->layers=LOCDWORD( mod_chipmunk, ins, LOC_LAYERS );
//            shape->collision_type=LOCDWORD( mod_chipmunk, ins, LOC_COLLISIONTYPE );
//            shape->sensor=LOCDWORD( mod_chipmunk, ins, LOC_SENSOR );
//        }
//    }
cpBodyEachShape(cuerpo, ActualizaShapes, ins);

    if (LOCDWORD( mod_chipmunk, ins, LOC_INCR_X )!=0 || LOCDWORD( mod_chipmunk, ins, LOC_INCR_Y )!=0)
    {
        cuerpo->p.x=(LOCDWORD( mod_chipmunk, ins, LOC_INCR_X )+LOCDWORD( mod_chipmunk, ins, LOC_X ))/resolucion;
        cuerpo->p.y=(LOCDWORD( mod_chipmunk, ins, LOC_INCR_Y )+LOCDWORD( mod_chipmunk, ins, LOC_Y ))/resolucion;
        if (estatico)
            gEstatico=1;
        LOCDWORD( mod_chipmunk, ins, LOC_INCR_X )=0;
        LOCDWORD( mod_chipmunk, ins, LOC_INCR_Y )=0;
    }
    if (gEstatico){
            cpSpaceReindexShapesForBody(modChipmunk_cpEspacio,(cpBody *)LOCDWORD( mod_chipmunk, ins, LOC_BODY ));
    }
}



void ActualizaLista(void *ptr, void *data) //handler de colisiones, se sustituirá por los callbacks en breve
{
    modChipmunkStruct_colHand * col=(modChipmunkStruct_colHand*) ptr;
//    cpArrayEach(col->colisiones, eliminaContactPointSetM, NULL);
    modChipmunk_destruyeArreglo(&col->colisiones,1);
    col->colisiones=col->arr;
    col->arr=modChipmunk_nuevoArreglo(10);
}



int contItUp;
DataPointer auxIterator;
static int updatePhisc()    //actualiza cada frame la física en general
{
    modChipmunk_cpEspacio->gravity.x= *(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X);
    modChipmunk_cpEspacio->gravity.y= *(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y);
    modChipmunk_cpEspacio->collisionBias=*(float *)GLOADDR(mod_chipmunk,GLO_BIAS_COEF);
    modChipmunk_cpEspacio->collisionSlop=*(float *)GLOADDR(mod_chipmunk,GLO_COLLISION_SLOP);
    modChipmunk_cpEspacio->collisionPersistence=GLODWORD(mod_chipmunk,GLO_CONTACT_PERSISTENCE);
    modChipmunk_cpEspacio->iterations= GLODWORD(mod_chipmunk,GLO_ITERATIONS);
    modChipmunk_cpEspacio->damping= *(float *)GLOADDR(mod_chipmunk,GLO_DAMPING);
    modChipmunk_cpEspacio->idleSpeedThreshold= *(float *)GLOADDR(mod_chipmunk,GLO_IDLESPEEDTHRESHOLD);
    modChipmunk_cpEspacio->sleepTimeThreshold= *(float *)GLOADDR(mod_chipmunk,GLO_SLEEPTIMETHRESHOLD);
    for (contItUp=0; contItUp<GLODWORD(mod_chipmunk,GLO_PHRESOLUTION); contItUp++)
        cpSpaceStep(modChipmunk_cpEspacio,*(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL)/(float)GLODWORD(mod_chipmunk,GLO_PHRESOLUTION) );

    modChipmunk_arregloItera(HandlerColisions,ActualizaLista,NULL);
    INSTANCE *i = first_instance ;
    while (i){
        if (LOCDWORD( mod_chipmunk, i, LOC_BODY )==0 &&  LOCDWORD( mod_chipmunk, i, LOC_SHAPETYPE )!=0){
            creaBodyAndShapeAutomat(i);
        }
        i = i->next;
    }

    for (auxIterator=modChipmunk_ListaCuerpos; auxIterator->sig!=NULL; )
    {
        if (existe(auxIterator->sig->father))
        {
            if (LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_SHAPETYPE )!=auxIterator->sig->typeShape ||
                auxIterator->sig->estado==STATUS_DEAD ||
                (LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_STATIC )!=cpBodyIsStatic(auxIterator->sig->body))
                )
            {
                if ((LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_SHAPETYPE )!=auxIterator->sig->typeShape && LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_BODY )==auxIterator->sig->body) ||
                    (LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_STATIC )!=cpBodyIsStatic(auxIterator->sig->body)))
                {
                    LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_BODY )=0;
                    LOCDWORD( mod_chipmunk, instance_get(auxIterator->sig->father), LOC_SHAPE )=0;
                }
                remueveProcesoListaProcesos(auxIterator);
            }
            else
            {
                actualizaProcess(auxIterator->sig);
                auxIterator=auxIterator->sig;
            }
        }
        else
        {
            remueveProcesoListaProcesos(auxIterator);
        }
    }

    return 0;
}

HOOK __bgdexport (mod_chipmunk , handler_hooks ) [] =
{
    { 4700 , updatePhisc},
    { 0 , NULL }
};


void __bgdexport( mod_chipmunk, module_initialize )()
{
    cpInitChipmunk();
    modChipmunk_cpEspacio= (int)cpSpaceNew();
    inicializaListaProcesos();
    HandlerColisions=modChipmunk_nuevoArreglo(10);
    LLinicializa(&modChipmunk_ListaHandlers);
//    LLinicializa(&modChipmunk_ListaEstaticosFondo);
    //int i,tam=((av*2+1)*1.4143)*((av*2+1)*1.4143);
    /*raiz=malloc(tam*sizeof(float));
    raiz[0]=0;
    raiz[1]=1;
    for (i=2;i<tam;i++)  //se calculan las raices para acelerar el proceso de calculo
        raiz[i]=sqrt(i);*/
}

static int modInfinity(INSTANCE * my, int * params)
{
    cpFloat a=INFINITY;
    return *(int*)&a;
}
/*
static void shapeFreeWrap(cpShape *ptr, void *unused){cpShapeFree(ptr);}

void
ChipmunkFreeSpaceChildren()
{
	cpArray *components = modChipmunk_cpEspacio->sleepingComponents;
	while(components->num) cpBodyActivate((cpBody *)components->arr[0]);

	cpSpatialIndexEach(modChipmunk_cpEspacio->staticShapes, (cpSpatialIndexIteratorFunc)&shapeFreeWrap, NULL);
	cpSpatialIndexEach(modChipmunk_cpEspacio->activeShapes, (cpSpatialIndexIteratorFunc)&shapeFreeWrap, NULL);
	cpArrayFreeEach(modChipmunk_cpEspacio->bodies, (void (*)(void*))cpBodyFree);
	cpArrayFreeEach(modChipmunk_cpEspacio->constraints, (void (*)(void*))cpConstraintFree);
}
*/
void eliminaHandlerLista(void *ptr, void *data)
{
    modChipmunkStruct_colHand* col=(modChipmunkStruct_colHand*) ptr;
    //cpSpaceRemoveCollisionHandler(modChipmunk_cpEspacio, col->a, col->b);
    //cpArrayDeleteObj(HandlerColisions, col);
    //cpArrayEach(col->arr, (cpArrayIter)eliminaContactPointSetM, NULL);
    modChipmunk_destruyeArreglo(&col->arr,1);
    //cpArrayEach(col->colisiones, (cpArrayIter)eliminaContactPointSetM, NULL);
    modChipmunk_destruyeArreglo(&col->colisiones,1);
    free(col);
}


static void shapeFreeWrap(cpSpace *space, cpShape *shape, void *unused){
	cpSpaceRemoveShape(space, shape);
	cpShapeFree(shape);
}

static void postShapeFree(cpShape *shape, cpSpace *space){
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)shapeFreeWrap, shape, NULL);
}

static void constraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *unused){
	cpSpaceRemoveConstraint(space, constraint);
	cpConstraintFree(constraint);
}

static void postConstraintFree(cpConstraint *constraint, cpSpace *space){
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)constraintFreeWrap, constraint, NULL);
}

static void bodyFreeWrap(cpSpace *space, cpBody *body, void *unused){
	cpSpaceRemoveBody(space, body);
	cpBodyFree(body);
}

static void postBodyFree(cpBody *body, cpSpace *space){
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)bodyFreeWrap, body, NULL);
}

// Safe and future proof way to remove and free all objects that have been added to the space.
void
ChipmunkDemoFreeSpaceChildren(cpSpace *space)
{
	// Must remove these BEFORE freeing the body or you will access dangling pointers.
	cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)postShapeFree, space);
	cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)postConstraintFree, space);

	cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)postBodyFree, space);
}

void __bgdexport( mod_chipmunk, module_finalize )()
{
    LLeliminaTodo(&modChipmunk_ListaHandlers,funcionElmHand,1);
    eliminaListaProcesos();
//    LLeliminaTodo(&modChipmunk_ListaEstaticosFondo,cpShapeFree,1);
    ChipmunkDemoFreeSpaceChildren(modChipmunk_cpEspacio);
    cpSpaceFree(modChipmunk_cpEspacio);
 //   cpArrayEach(HandlerColisions,eliminaHandlerLista,NULL);
  //  modChipmunk_destruyeArreglo(&HandlerColisions,1);
    //free(raiz);
}



static int modChipmunk_cleanSpace(INSTANCE * my, int * params)
{

    //elimina todo el mundo físico
    LLeliminaTodo(&modChipmunk_ListaHandlers,funcionElmHand,1);
    eliminaListaProcesos();
//    LLeliminaTodo(&modChipmunk_ListaEstaticosFondo,cpShapeFree,1);
    //cpSpaceFreeChildren(modChipmunk_cpEspacio);
    ChipmunkDemoFreeSpaceChildren(modChipmunk_cpEspacio);
    cpSpaceFree(modChipmunk_cpEspacio);
    modChipmunk_destruyeArreglo(&HandlerColisions,1);

    cpResetShapeIdCounter();

    //crea el nuevo espacio
    modChipmunk_cpEspacio= (int)cpSpaceNew();
    cpSpaceInit(modChipmunk_cpEspacio);
    inicializaListaProcesos();
    HandlerColisions=modChipmunk_nuevoArreglo(10);
    LLinicializa(&modChipmunk_ListaHandlers);
//    LLinicializa(&modChipmunk_ListaEstaticosFondo);
//printf("limpiado %p",modChipmunk_ListaHandlers);
    modChipmunk_numLista=0;

}

static int modForceCreateBody(INSTANCE * my, int * params)
{
    if (LOCDWORD( mod_chipmunk, my, LOC_BODY )!=0)
    {
        cpBody * bod=(cpBody *) LOCDWORD( mod_chipmunk, my, LOC_BODY );
        DataPointer est= (DataPointer) bod->data;
        est->estado=STATUS_DEAD;
    }
    if (LOCDWORD( mod_chipmunk, my, LOC_SHAPETYPE )!=TYPE_NONE)
        creaBodyAndShapeAutomat(my);

    return 1;
}

static int modForceCreateBodyI(INSTANCE * i, int * params)
{
    INSTANCE * my =params[0];
    if (LOCDWORD( mod_chipmunk, my, LOC_BODY )!=0)
    {
        cpBody * bod=(cpBody *) LOCDWORD( mod_chipmunk, my, LOC_BODY );
        DataPointer est= (DataPointer) bod->data;
        est->estado=STATUS_DEAD;
    }
    if (LOCDWORD( mod_chipmunk, my, LOC_SHAPETYPE )!=TYPE_NONE)
        creaBodyAndShapeAutomat(my);

    return 1;
}

char * __bgdexport( mod_chipmunk, modules_dependency )[] =
{
    "librender",
    "libgrbase",
    "libdraw",
    "libblit",
    NULL
};


static int modEQ(INSTANCE * my, int * params)
{
    cpFloat a=INFINITY;
    return a==*(( float * ) &params[0]);
}




static int modIntersectsBB(INSTANCE * my, int * params)
{
    INSTANCE * b = instance_get( params[0] ) ;
    cpBB v1= cpShapeCacheBB((cpShape *)LOCDWORD(mod_chipmunk, my, LOC_SHAPE));
    cpBB v2= cpShapeCacheBB((cpShape *)LOCDWORD(mod_chipmunk, b, LOC_SHAPE));

    return cpBBIntersects(v1,v2);
}

DLSYSFUNCS __bgdexport( mod_chipmunk, functions_exports) [] =
{

    {"INFINITYF" , "",   TYPE_FLOAT, modInfinity},

    {"modChipmunkdeg2rad" , "F",   TYPE_FLOAT, moddeg2rad},
    {"SEGMENTQUERYHITDIST" , "IIIIP",   TYPE_FLOAT, modSegmentQueryHitDist},
    {"SEGMENTQUERYHITPOINT" , "IIIIPPP",   TYPE_INT, modSegmentQueryHitPoint},
    {"FORCECREATEBODY" , "",   TYPE_INT, modForceCreateBody},
    {"FORCECREATEBODY" , "I",   TYPE_INT, modForceCreateBodyI},

    //destrucción general
    {"CLEANSPACE" , "",   TYPE_INT, modChipmunk_cleanSpace},

    //Funciones matemáticas
    {"CPFLERP" , "FFF",   TYPE_FLOAT, modcpflerp},
    {"CPFCLAMP" , "FFF",   TYPE_FLOAT, modcpfclamp},
    {"CPFLERPCONST" , "FFF",   TYPE_FLOAT, modcpflerpconst},

    //OPERACIONES CON VECTORES
    {"CPVEQL" , "PP",   TYPE_INT, modcpveql},
    {"CPVADD" , "PPP",   TYPE_INT, modcpvadd},
    {"CPVSUB" , "PPP",   TYPE_INT, modcpvsub},
    {"CPVNEG" , "PP",   TYPE_INT, modcpvneg},
    {"CPVMULT" , "PFP",   TYPE_INT, modcpvmult},
    {"CPVPERP" , "PP",   TYPE_INT, modcpvperp},
    {"CPVRPERP" , "PP",   TYPE_INT, modcpvrperp},
    {"CPVNORMALIZE" , "PP",   TYPE_INT, modcpvnormalize},
    {"CPVNORMALIZE_SAFE" , "PP",   TYPE_INT, modcpvnormalize_safe},
    {"CPVPROJECT" , "PPP",   TYPE_INT, modcpvproject},
    {"CPVROTATE" , "PPP",   TYPE_INT, modcpvrotate},
    {"CPVUNROTATE" , "PPP",   TYPE_INT, modcpvunrotate},
    {"CPVFORANGLE" , "FP",   TYPE_INT, modcpvforangle},
    {"CPVCLAMP" , "PFP",   TYPE_INT, modcpvclamp},
    {"CPVLERP" , "PPFP",   TYPE_INT, modcpvlerp},
    {"CPVLERPCONST" , "PPFP",   TYPE_INT, modcpvlerpconst},
    {"CPVSLERP" , "PPFP",   TYPE_INT, modcpvslerp},
    {"CPVSLERPCONST" , "PPFP",   TYPE_INT, modcpvslerpconst},
    {"CPVNEAR" , "PPF",   TYPE_INT, modcpvnear},

    {"CPVDOT" , "PP",   TYPE_FLOAT, modcpvdot},
    {"CPVCROSS" , "PP",   TYPE_FLOAT, modcpvcross},
    {"CPVLENGTH" , "P",   TYPE_FLOAT, modcpvlength},
    {"CPVLENGTHSQ" , "P",   TYPE_FLOAT, modcpvlengthsq},
    {"CPVTOANGLE" , "P",   TYPE_FLOAT, modcpvtoangle},
    {"CPVDISTSQ" , "PP",   TYPE_FLOAT, modcpvdistsq},
    {"CPVDIST" , "PP",   TYPE_FLOAT, modcpvdist},

    {"CPVSTR" , "P",   TYPE_STRING, modcpvstr},
    {"CPV" , "FFP",    TYPE_POINTER, modcpv},


    //cpBB
    {"CPBBWRAPVECT" , "PPP",   TYPE_INT, modcpBBWrapVect},
    {"CPBBCLAMPVECT" , "PPP",   TYPE_INT, modcpBBClampVect},
    {"CPBBEXPAND" , "PPP",   TYPE_INT, modcpBBexpand},
    {"CPBBCONTAINSVECT" , "PP",   TYPE_INT, modcpBBcontainsVect},
    {"CPBBMERGE" , "PPP",   TYPE_INT, modcpBBmerge},
    {"CPBBCONTAINSBB" , "PP",   TYPE_INT, modcpBBcontainsBB},
    {"CPBBINTERSECTS" , "PP",   TYPE_INT, modcpBBintersects},
    {"CPBBNEW" , "FFFFP",   TYPE_INT, modcpBBNew},



    //cpBody
    {"DEFBODYP" , "IIP",   TYPE_INT, modDefcpBody},
    {"DEFBODYF" , "IIF",   TYPE_INT, modDefcpBody},
    {"DEFBODYI" , "III",   TYPE_INT, modDefcpBody},

    {"GETBODY" , "",   TYPE_INT, modgetBody},
    {"CPBODYLOCAL2WORLD" , "IPP",   TYPE_INT, modcpBodyLocal2World},
    {"GETBODY" , "II",   TYPE_FLOAT, modGetEcpBody},
    {"GETBODY" , "IIP",   TYPE_INT, modGetEcpBody},

    {"CPMOMENTFORCIRCLE" , "FFFP",   TYPE_FLOAT, modcpMomentForCircle},
    {"CPMOMENTFORSEGMENT" , "FPP",   TYPE_FLOAT, modcpMomentForSegment},
    {"CPMOMENTFORPOLY" , "FIPP",   TYPE_FLOAT, modcpMomentForPoly},
    {"CPMOMENTFORBOX" , "FFF",   TYPE_FLOAT, modcpMomentForBox},
    {"CPMOMENTFORCIRCLE" , "FFFFF",   TYPE_FLOAT, modcpMomentForCircle1},
    {"CPMOMENTFORSEGMENT" , "FFFFF",   TYPE_FLOAT, modcpMomentForSegment1},
    {"CPMOMENTFORPOLY" , "FIPFF",   TYPE_FLOAT, modcpMomentForPoly1},
    {"CPAREAFORCIRCLE" , "FF",   TYPE_FLOAT, modcpAreaForCircle},
    {"CPAREAFORSEGMENT" , "FFFFF",   TYPE_FLOAT, modcpAreaForSegment},
    {"CPAREAFORPOLY" , "IP",   TYPE_FLOAT, modcpAreaForPoly},
    {"CPBODYSETMASS" , "IF",   TYPE_INT, modcpBodySetMass},
    {"CPBODYSETMOMENT" , "IF",   TYPE_INT, modcpBodySetMoment},
    {"CPBODYSETANGLE" , "IF",   TYPE_INT, modcpBodySetAngle},
    {"CPBODYUPDATEPOSITION" , "IF",   TYPE_INT, modcpBodyUpdatePosition},
    {"CPBODYRESETFORCES" , "I",   TYPE_INT, modcpBodyResetForces},
    {"CPBODYISSLEEPING" , "I",   TYPE_INT, modcpBodyIsSleeping},
    {"CPBODYSLEEP" , "I",   TYPE_INT, modcpBodySleep},
    {"CPBODYACTIVATE" , "I",   TYPE_INT, modcpBodyActivate},
    {"CPBODYISSTATIC" , "I",   TYPE_INT, modcpBodyIsStatic},
    {"CPBODYISROGUE" , "I",   TYPE_INT, modcpBodyIsRogue},
    {"CPBODYSLEEPWITHGROUP" , "II",   TYPE_INT, modcpBodySleepWithGroup},
    {"CPBODYAPPLYFORCE" , "IPP",   TYPE_INT, modcpBodyApplyForce},
    {"CPBODYAPPLYIMPULSE" , "IPP",   TYPE_INT, modcpBodyApplyImpulse},
    {"CPBODYWORLD2LOCAL" , "IPP",   TYPE_INT, modcpBodyWorld2Local},
    {"CPBODYSLEW" , "IPF",   TYPE_INT, modcpBodySlew},
    {"CPBODYUPDATEVELOCITY" , "IPFF",   TYPE_INT, modcpBodyUpdateVelocity},


    //cpShape
    {"DEFSHAPEI" , "III",   TYPE_INT, modDefcpShape},
    {"DEFSHAPEF" , "IIF",   TYPE_INT, modDefcpShape},
    {"DEFSHAPEP" , "IIP",   TYPE_INT, modDefcpShape},
    {"CPPOLYSHAPEGETVERT" , "IIP",   TYPE_INT, modcpPolyShapeGetVert},

    {"CPRECENTERPOLY" , "IP",   TYPE_INT, modcpRecenterPoly},
    {"CPCENTROIDFORPOLY" , "IPP",   TYPE_INT, modcpCentroidForPoly},
    {"CPSEGMENTSHAPEGETRADIUS" , "I",   TYPE_FLOAT, modcpSegmentShapeGetRadius},
    {"CPCIRCLESHAPEGETRADIUS" , "I",   TYPE_FLOAT, modcpCircleShapeGetRadius},
    {"CPSEGMENTSHAPEGETNORMAL" , "IP",   TYPE_INT, modcpSegmentShapeGetNormal},
    {"CPCIRCLESHAPEGETOFFSET" , "IP",   TYPE_INT, modcpCircleShapeGetOffset},
    {"CPSEGMENTSHAPEGETA" , "IP",   TYPE_INT, modcpSegmentShapeGetA},
    {"CPSEGMENTSHAPEGETB" , "IP",   TYPE_INT, modcpSegmentShapeGetB},

    {"CPPOLYSHAPEGETNUMVERTS" , "I",   TYPE_INT, modcpPolyShapeGetNumVerts},
    {"CPSHAPECACHEBB" , "IP",   TYPE_INT, modcpShapeCacheBB},
    {"CPRESETSHAPEIDCOUNTER" , "",   TYPE_INT, modcpResetShapeIdCounter},

    {"GETCPSHAPEI" , "II",   TYPE_INT, modGetcpShape},
    {"GETCPSHAPEF" , "II",   TYPE_FLOAT, modGetcpShape},
    {"GETCPSHAPEP" , "IIP",   TYPE_INT, modGetcpShape},



    //colisiones
    {"CPSPACEPOINTQUERYFIRST" , "PII",   TYPE_INT, modcpSpacePointQueryFirst},

    {"CPSHAPEPOINTQUERY" , "IP",   TYPE_INT, modcpShapePointQuery},
    {"CPSHAPESEGMENTQUERY" , "IPPP",   TYPE_INT, modcpShapeSegmentQuery},
    {"CPSEGMENTQUERYHITPOINT" , "PPPP",   TYPE_INT, modcpSegmentQueryHitPoint},
    {"CPSEGMENTQUERYHITDIST" , "PPP",   TYPE_FLOAT, modcpSegmentQueryHitDist},
    {"CPSPACESEGMENTQUERYFIRST" , "PPIIP",   TYPE_INT, modcpSpaceSegmentQueryFirst},


    //funciones de bodies con ids
    {"SLEW" , "FFF",   TYPE_INT, modSlew},
    {"UPDATEVELOCITY" , "",   TYPE_INT, modUpdateVelocity},
    {"UPDATEVELOCITY" , "F",   TYPE_INT, modUpdateVelocity2},
    {"UPDATEVELOCITYAG" , "I",   TYPE_INT, modUpdateVelocityAg},
    {"LOCAL2WORLD" , "IFFPP",   TYPE_INT, modLocal2World},
    {"WORLD2LOCAL" , "IFFPP",   TYPE_INT, modWorld2Local},
    {"APPLYIMPULSE" , "IFFFF",   TYPE_INT, modApplyImpulse},
    {"RESETFORCES" , "I",   TYPE_INT, modResetForces},
    {"APPLYFORCE" , "IFFFF",   TYPE_INT, modApplyForce},
    {"SLEEP" , "I",   TYPE_INT, modSleep},
    {"ACTIVATE" , "I",   TYPE_INT, modActivate},
    {"ISSTATIC" , "I",   TYPE_INT, modIsStatic},
    {"ISROGUE" , "I",   TYPE_INT, modIsRogue},
    {"ISSLEEPING" , "I",   TYPE_INT, modIsSleeping},
    {"CALCULATECONVEXBODY" , "IIP",   TYPE_INT, modcpCalculaConvexHull},


    //funciones de bb con ids
    {"INTERSECTS" , "II",   TYPE_INT, modIntersects},
    {"CONTAINS" , "II",   TYPE_INT, modContains},
    {"CONTAINSVEC" , "IFF",   TYPE_INT, modContainsVec},

    //funciones de space con ids
    {"ACTIVATEPROCESSTOUCHINGME" , "",   TYPE_INT, modActivateProcessTouchingIt},

    {"ADDSEGMENTSHAPETO" , "FFFFFI",   TYPE_INT, modaddSegmentShapeTo},

    //FUNCIONS DE SHAPE CON IDS
    {"ADDCIRCLESHAPE" , "FFF",   TYPE_INT, modaddCircleShape},
    {"ADDSEGMENTSHAPE" , "FFFFF",   TYPE_INT, modaddSegmentShape},
    {"ADDPOLYSHAPE" , "FFIP",   TYPE_INT, modaddPolyShape},

    {"SPACEPOINTQUERYFIRST" , "FFII",   TYPE_INT, modSpacePointQueryFirst},
    {"SPACESEGMENTQUERYFIRST" , "FFFFIIP",   TYPE_INT, modSpaceSegmentQueryFirst},

    {"COLLISIONHANDLERNEW" , "II",   TYPE_INT, modcrearHandler},
    {"REMOVECOLLISIONHANDLER" , "I",   TYPE_INT, modremoveHandler},
    {"GETCOLLISIONINFO" , "IP",   TYPE_INT, modgetColInfo},

    {"SETPINJOINTPROPERTIES" , "IIFF",   TYPE_INT, modsetPinJointProperties},
    {"SETPINJOINTPROPERTIES" , "IIF",   TYPE_INT, modsetPinJointProperties},
    {"SETSLIDEJOINTPROPERTIES" , "IIFF",   TYPE_INT, modsetSlideJointProperties},
    {"SETSLIDEJOINTPROPERTIES" , "IIF",   TYPE_INT, modsetSlideJointProperties},
    {"SETPIVOTJOINTPROPERTIES" , "IIFF",   TYPE_INT, modsetPivotJointProperties},
    {"SETGROOVEJOINTPROPERTIES" , "IIFF",   TYPE_INT, modsetGrooveJointProperties},
    {"SETDAMPEDSPRINGPROPERTIES" , "IIFF",   TYPE_INT, modsetDampedSpringProperties},
    {"SETDAMPEDSPRINGPROPERTIES" , "IIF",   TYPE_INT, modsetDampedSpringProperties},
    {"SETDAMPEDROTARYSPRINGPROPERTIES" , "IIF",   TYPE_INT, modsetDampedRotarySpringProperties},
    {"SETROTARYLIMITJOINTPROPERTIES" , "IIF",   TYPE_INT, modsetRotaryLimitJointProperties},
    {"SETRATCHETJOINTPROPERTIES" , "IIF",   TYPE_INT, modsetRatchetJointProperties},
    {"SETGEARJOINTPROPERTIES" , "IIF",   TYPE_INT, modsetGearJointProperties},
    {"SETSIMPLEMOTORPROPERTIES" , "IIF",   TYPE_INT, modsetSimpleMotorProperties},

    {"GETPINJOINTPROPERTIES" , "IIPP",   TYPE_INT, modgetPinJointProperties},
    {"GETPINJOINTPROPERTIES" , "II",   TYPE_FLOAT, modgetPinJointProperties},
    {"GETSLIDEJOINTPROPERTIES" , "IIPP",   TYPE_INT, modgetSlideJointProperties},
    {"GETSLIDEJOINTPROPERTIES" , "II",   TYPE_FLOAT, modgetSlideJointProperties},
    {"GETGROOVEJOINTPROPERTIES" , "IIPP",   TYPE_INT, modgetGrooveJointProperties},
    {"GETDAMPEDSPRINGPROPERTIES" , "IIPP",   TYPE_INT, modgetDampedSpringProperties},
    {"GETDAMPEDSPRINGPROPERTIES" , "II",   TYPE_FLOAT, modgetDampedSpringProperties},
    {"GETDAMPEDROTARYSPRINGPROPERTIES" , "II",   TYPE_FLOAT, modgetDampedRotarySpringProperties},
    {"GETROTARYLIMITJOINTPROPERTIES" , "II",   TYPE_FLOAT, modgetRotaryLimitJointProperties},
    {"GETRATCHETJOINTPROPERTIES" , "II",   TYPE_FLOAT, modgetRatchetJointProperties},
    {"GETGEARJOINTPROPERTIES" , "II",   TYPE_FLOAT, modgetGearJointProperties},
    {"GETSIMPLEMOTORPROPERTIES" , "II",   TYPE_FLOAT, modgetSimpleMotorProperties},
    {"GETPIVOTJOINTPROPERTIES" , "IIPP",   TYPE_INT, modgetPivotJointProperties},

    {"GETCONSTRAINTS" , "I",   TYPE_INT, modgetConstraints},
    {"GETSHAPES" , "I",   TYPE_INT, modgetShapes},

    {"ADDINANIMATESHAPE" , "IFFF",   TYPE_INT, modaddInanimateShape},
    {"ADDINANIMATESHAPE" , "IFFFFF",   TYPE_INT, modaddInanimateShape},
    {"ADDINANIMATESHAPE" , "IFFIP",   TYPE_INT, modaddInanimateShape},

    {"SETENDPOINTSLINE" , "FFFF",   TYPE_INT, modsetEndPointsLine},
    {"SETRADIUSLINE" , "F",   TYPE_INT, modsetRadiusLine},
    {"SETOFFSETCIRCLE" , "FF",   TYPE_INT, modsetOffsetCircle},
    {"SETRADIUSCIRCLE" , "F",   TYPE_INT, modsetRadiusCircle},
    {"SETVERTCONVEXPOLIGON" , "FFIP",   TYPE_INT, modsetVertConvexPoligon},

    {"SETENDPOINTSLINE" , "IFFFF",   TYPE_INT, modsetEndPointsLineI},
    {"SETRADIUSLINE" , "IF",   TYPE_INT, modsetRadiusLineI},
    {"SETOFFSETCIRCLE" , "IFF",   TYPE_INT, modsetOffsetCircleI},
    {"SETRADIUSCIRCLE" , "IF",   TYPE_INT, modsetRadiusCircleI},
    {"SETVERTCONVEXPOLIGON" , "IFFIP",   TYPE_INT, modsetVertConvexPoligonI},

    {"SHAPECACHEBB" , "IP",   TYPE_INT, modShapeCacheBB},
    {"GETOPTIMALINERTIA" , "II",   TYPE_FLOAT, modGetOptimalInertia},
    {"GETOPTIMALINERTIA" , "IIFF",   TYPE_FLOAT, modGetOptimalInertia},

    {"SPACEUSESPATIALHASH","FI",TYPE_INT,modSpaceUseSpatialHash},

    {"SPACEADDCOLLISIONHANDLER", "IISSSSPPPP" ,TYPE_INT ,modcpSpaceAddCollisionHandler},
    {"SPACEREMOVECOLLISIONHANDLER", "II", TYPE_INT, modcpSpaceRemoveCollisionHandler},
    {"INTERSECTSBB", "I", TYPE_INT, modIntersectsBB},

    {"GETARBITERCANTCONTACTPOINTS", "I", TYPE_INT, modArbiterGetNumContactPoints},
    {"ARBITERISFIRSTCONTACT", "I", TYPE_INT, modArbiterIsFirstContact},
    {"GETARBITERNORMAL", "IIPP", TYPE_INT, modArbiterGetNormal},
    {"GETARBITERPOINT", "IIPP", TYPE_INT, modArbiterGetPoint},
    {"GETARBITERDEPTH", "II", TYPE_FLOAT, modArbiterGetDepth},
    {"GETARBITERTOTALIMPULSEWITHFRICTION", "IPP", TYPE_INT, modArbiterTotalImpulseWithFriction},
    {"GETARBITERTOTALIMPULSE", "IPP", TYPE_INT, modArbiterTotalImpulse},

    {"SETARBITERSURFACEVELOCITY", "IFF" , TYPE_INT, modArbiterSetSurfaceVelocity},
    {"GETARBITERSURFACEVELOCITY", "IPP" , TYPE_INT, modArbiterGetSurfaceVelocity},
    {"SETARBITERFRICTION", "IF" , TYPE_INT, modArbiterSetFriction},
    {"GETARBITERFRICTION", "I" , TYPE_FLOAT, modArbiterGetFriction},
    {"SETARBITERELASTICITY", "IF" , TYPE_INT, modArbiterSetElasticity},
    {"GETARBITERELASTICITY", "I" , TYPE_FLOAT, modArbiterGetElasticity},
    {"MODARBITERPROCESSA", "I" , TYPE_INT, modArbiterProcessA},
    {"MODARBITERPROCESSB", "I" , TYPE_INT, modArbiterProcessB},

//cpSpace

    {"CPSPACEACTIVATESHAPESTOUCHINGSHAPE" , "I",   TYPE_INT, modcpSpaceActivateShapesTouchingShape},
    {"SPACERESIZESTATICHASH" , "FI",   TYPE_INT, modcpSpaceResizeStaticHash},
    {"SPACERESIZEACTIVEHASH" , "FI",   TYPE_INT, modcpSpaceResizeActiveHash},

    {"MODGETSTATICBODY", "" , TYPE_INT, modGetStaticBody},
    {"SPACEUSESPATIALHASH", "FI" , TYPE_INT, modcpSpaceUseSpatialHash},
    {"SPACEGETCURRENTTIMESTEP", "" , TYPE_FLOAT, modcpSpaceGetCurrentTimeStep},

    //CONSTRAITNTS

    {"ADDDAMPEDSPRING" , "IIFFFFFFF",   TYPE_INT, modaddDampedSpring},
    {"ADDPIVOTJOINT" , "IIFF",   TYPE_INT, modaddPivotJoint},
    {"ADDDAMPEDROTARYSPRING" , "IIFFF",   TYPE_INT, modaddDampedRotarySpring},
    {"ADDROTARYLIMITJOINT" , "IIFF",   TYPE_INT, modaddRotaryLimitJoint},
    {"ADDRATCHETJOINT" , "IIFF",   TYPE_INT, modaddRatchetJoint},
    {"ADDSIMPLEMOTOR" , "IIF",   TYPE_INT, modaddSimpleMotor},
    {"ADDGROOVEJOINT" , "IIFFFFFF",   TYPE_INT, modaddGrooveJoint},
    {"ADDSLIDEJOINT" , "IIFFFFFF",   TYPE_INT, modaddSlideJoint},
    {"ADDPIVOTJOINT2" , "IIFFFF",   TYPE_INT, modaddPivotJoint2},
    {"ADDPINJOINT" , "IIFFFF",   TYPE_INT, modaddPinJoint},
    {"ADDGEARJOINT" , "IIFF",   TYPE_INT, modaddGearJoint},
    {"REMOVECONSTRAINT" , "I",   TYPE_INT, modremoveConstraint},

    {"CONSTRAINTGETIMPULSE" , "I", TYPE_FLOAT, modcpConstraintGetImpulse},

    {"DEFCONSTRAINTF" , "IIF",   TYPE_INT, modDefcpConstraint},
    {"DEFCONSTRAINTI" , "III",   TYPE_INT, modDefcpConstraint},
    {"GETCONSTRAINTF" , "II",   TYPE_FLOAT, modGetcpConstraint},
    {"GETCONSTRAINTI" , "II",   TYPE_INT, modGetcpConstraint},
    {"CONSTRAINTSETPOSTSOLVEFUNC" , "IS",   TYPE_INT, modcpConstraintSetPostSolveFunc},
    {"CONSTRAINTSETPRESOLVEFUNC" , "IS",   TYPE_INT, modcpConstraintSetPreSolveFunc},

    {"GETCONSTRAINTFATHERA" , "I",   TYPE_INT, modGetFathetA},
    {"GETCONSTRAINTFATHERB" , "I",   TYPE_INT, modGetFathetB},
    {"GENERATEAPROXPOLY" , "IIF",   TYPE_INT, modChipmunkGeneraTerreno},
    {"DELETEAPROXPOLY" , "I",   TYPE_INT, modChipmunkEliminaTerreno},

    {"GETELEMENT" , "II",   TYPE_POINTER, modChipmunkGetElement},
    {"CANTELEMENT" , "I",   TYPE_INT, modChipmunkCantElement},

//    {"CANT_LINES" , "I",   TYPE_INT, modChipmunkCantLineas},
 //   {"CANT_POINTS" , "II",   TYPE_INT, modChipmunkCantLPuntos},
  //  {"TPOINT" , "III",   TYPE_POINTER, modChipmunkPuntoT},
   // {"ELIM_POINTS" , "I",   TYPE_INT, modChipmunkElimTerr},
    //{"CHANGE_AV" , "I",   TYPE_INT, modChipmunkCambiaTamano},

    {"METABALL" , "IIIPIF",   TYPE_INT, modChipmunkMetaball},
    {"DRAW_WATER" , "IIIPIFFIII",   TYPE_INT, modChipmunkPintaAgua}, //FILE,GRAPH,COLOR,LISTA IDS, TAM,tam particulas,umbral
    {"SET_EFECTOR" , "II",   TYPE_INT, modChipmunkSetEfector},
    {"EMULATE_WATER" , "P",   TYPE_INT, modChipmunkEmulateAgua},

    {"BBFORCIRCLE" , "FFF",   TYPE_INT, modcpBBNewCircle},

    {"ACTIVATESTATIC" , "II",   TYPE_INT, modActivateStatic},
    {"SLEEPWITHGROUP" , "II",   TYPE_INT, modSleepWithGroup },


    {0, 0, 0, 0}//TYPE_POINTER
};

#endif

