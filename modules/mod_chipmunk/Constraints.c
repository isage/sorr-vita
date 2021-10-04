#include "Constraints.h"
#include "locales.h"
#include "constantes.h"
#include "Miscelanea.h"
#include "estructuras.h"

extern cpSpace * modChipmunk_cpEspacio;

//int addConstr(cpBody *bod1, cpBody *bod2,cpConstraint * cons)
//{
//    int z;
//    if (bod1!=modChipmunk_cpEspacio->staticBody)
//    {
//        DataPointer elem= (DataPointer)bod1->data;
//        LLagrega(elem->Constraints,cons);
//        //printf(" creado  %d  \n",elem->Constraints);
//    }
//    if (bod2!=modChipmunk_cpEspacio->staticBody)
//    {
//        DataPointer elem= (DataPointer)bod2->data;
//        LLagrega(elem->Constraints,cons);
//        //printf(" creado  %d  \n",elem->Constraints);
//    }
//    return 0;
//}

#define agregaData z->data=malloc(sizeof(DatosConstraint));\
    ((DatosConstraint*)z->data)->fatherA=params[0];\
    ((DatosConstraint*)z->data)->fatherB=params[1];
 int modaddDampedSpring(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpDampedSpringNew(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]),cpv(*(float *)&params[4],*(float *)&params[5]),*(float *)&params[6],*(float *)&params[7],*(float *)&params[8]);
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
    //addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddPinJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=(cpBody *)LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=(cpBody *)LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpPinJointNew(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]),cpv(*(float *)&params[4],*(float *)&params[5]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddPivotJoint2(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpPivotJointNew2(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]),cpv(*(float *)&params[4],*(float *)&params[5]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddSlideJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpSlideJointNew(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]),cpv(*(float *)&params[4],*(float *)&params[5]),*(float *)&params[6],*(float *)&params[7]);
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddGrooveJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpGrooveJointNew(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]),cpv(*(float *)&params[4],*(float *)&params[5]),cpv(*(float *)&params[6],*(float *)&params[7]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddSimpleMotor(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpSimpleMotorNew(b1,b2,modChipmunkdeg2rad(*(float *)&params[2]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddRatchetJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpRatchetJointNew(b1,b2,modChipmunkdeg2rad(*(float *)&params[2]),modChipmunkdeg2rad(*(float *)&params[3]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}
 int modaddRotaryLimitJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpRotaryLimitJointNew(b1,b2,modChipmunkdeg2rad(*(float *)&params[2]),modChipmunkdeg2rad(*(float *)&params[3]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddGearJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;
    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpGearJointNew(b1,b2,modChipmunkdeg2rad(*(float *)&params[2]),modChipmunkdeg2rad(*(float *)&params[3]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddDampedRotarySpring(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpDampedRotarySpringNew(b1,b2,modChipmunkdeg2rad(*(float *)&params[2]),*(float *)&params[3],*(float *)&params[4]);
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}

 int modaddPivotJoint(INSTANCE * my, int * params)
{
    cpBody *	b1, *b2;
    cpConstraint * z;

    INSTANCE * b;
    if (params[0]!=0)
    {
        b = instance_get( params[0] ) ;
        b1=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b1 = modChipmunk_cpEspacio->staticBody;
    }

    if (params[1]!=0)
    {
        b = instance_get( params[1] ) ;
        b2=LOCDWORD(mod_chipmunk,b,LOC_BODY);
    }
    else
    {
        b2 = modChipmunk_cpEspacio->staticBody;
    }
    z=cpPivotJointNew(b1,b2,cpv(*(float *)&params[2],*(float *)&params[3]));
    cpSpaceAddConstraint(modChipmunk_cpEspacio, z);
//    addConstr(b1,b2,z);
    agregaData
    return (int)z;
}


 int modsetPinJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_ANCHR1:
        cpPinJointSetAnchr1(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_ANCHR2:
        cpPinJointSetAnchr2(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_DIST:
        cpPinJointSetDist(constr,*(float *)&params[2]);
        break;
    }

    return 1;
}



 int modsetSlideJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_ANCHR1:
        cpSlideJointSetAnchr1(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_ANCHR2:
        cpSlideJointSetAnchr2(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_MIN:
        cpSlideJointSetMin(constr,*(float *)&params[2]);
        break;
    case CP_C_MAX:
        cpSlideJointSetMax(constr,*(float *)&params[2]);
        break;
    }

    return 1;
}



 int modsetPivotJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_ANCHR1:
        cpPivotJointSetAnchr1(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_ANCHR2:
        cpPivotJointSetAnchr2(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    }

    return 1;
}


 int modsetGrooveJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_GROOVEA:
        cpGrooveJointSetGrooveA(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_GROOVEB:
        cpGrooveJointSetGrooveB(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_ANCHR2:
        cpGrooveJointSetAnchr2(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    }

    return 1;
}



 int modsetDampedSpringProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_ANCHR1:
        cpDampedSpringSetAnchr1(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_ANCHR2:
        cpDampedSpringSetAnchr2(constr,cpv(*(float *)&params[2],*(float *)&params[3]));
        break;
    case CP_C_RESTLENGTH:
        cpDampedSpringSetRestLength(constr,*(float *)&params[2]);
        break;
    case CP_C_STIFFNESS:
        cpDampedSpringSetStiffness(constr,*(float *)&params[2]);
        break;
    case CP_C_DAMPING:
        cpDampedSpringSetDamping(constr,*(float *)&params[2]);
        break;
    }

    return 1;
}


 int modsetDampedRotarySpringProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_RESTANGLE:
        cpDampedRotarySpringSetRestAngle(constr,modChipmunkdeg2rad(*(float *)&params[2]));
        break;
    case CP_C_STIFFNESS:
        cpDampedRotarySpringSetStiffness(constr,*(float *)&params[2]);
        break;
    case CP_C_DAMPING:
        cpDampedRotarySpringSetDamping(constr,*(float *)&params[2]);
        break;
    }

    return 1;
}



 int modsetRotaryLimitJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_MIN:
        cpRotaryLimitJointSetMin(constr, modChipmunkdeg2rad(*(float *)&params[2]) );
        break;
    case CP_C_MAX:
        cpRotaryLimitJointSetMax(constr,modChipmunkdeg2rad(*(float *)&params[2]) );
        break;
    }

    return 1;
}




 int modsetRatchetJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_ANGLE:
        cpRatchetJointSetAngle(constr,modChipmunkdeg2rad(*(float *)&params[2]) );
        break;
    case CP_C_PHASE:
        cpRatchetJointSetPhase(constr,modChipmunkdeg2rad(*(float *)&params[2]));
        break;
    case CP_C_RATCHET:
        cpRatchetJointSetRatchet(constr,modChipmunkdeg2rad(*(float *)&params[2]));
        break;
    }

    return 1;
}



 int modsetGearJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_RATIO:
        cpGearJointSetRatio(constr,modChipmunkdeg2rad(*(float *)&params[2]));
        break;
    case CP_C_PHASE:
        cpGearJointSetPhase(constr,modChipmunkdeg2rad(*(float *)&params[2]));
        break;
    }

    return 1;
}



 int modsetSimpleMotorProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    switch (params[1])
    {
    case CP_C_RATIO:

        cpSimpleMotorSetRate(constr,modChipmunkdeg2rad(*(float *)&params[2]) );
        break;
    }

    return 1;
}

 int modgetPinJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    cpVect ar;
    float *a,*b,res;
    switch (params[1])
    {
    case CP_C_ANCHR1:
        ar=cpPinJointGetAnchr1(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;

        break;
    case CP_C_ANCHR2:
        ar=cpPinJointGetAnchr2(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_DIST:
        res=cpPinJointGetDist(constr);
        return *(int *)&res;
        break;
    }

    return 1;
}



 int modgetSlideJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    cpVect ar;
    float *a,*b,res;
    switch (params[1])
    {
    case CP_C_ANCHR1:
        ar=cpSlideJointGetAnchr1(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_ANCHR2:
        ar=cpSlideJointGetAnchr2(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_MIN:
        res=cpSlideJointGetMin(constr);
        return *(int *)&res;
        break;
    case CP_C_MAX:
        res=cpSlideJointGetMax(constr);
        return *(int *)&res;
        break;
    }

    return 1;
}



 int modgetPivotJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    cpVect ar;
    float *a,*b,res;
    switch (params[1])
    {
    case CP_C_ANCHR1:
        ar=cpPivotJointGetAnchr1(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_ANCHR2:
        ar=cpPivotJointGetAnchr2(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    }

    return 1;
}


 int modgetGrooveJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    cpVect ar;
    float *a,*b,res;
    switch (params[1])
    {
    case CP_C_GROOVEA:
        ar=cpGrooveJointGetGrooveA(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_GROOVEB:
        ar=cpGrooveJointGetGrooveB(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_ANCHR2:
        cpGrooveJointGetAnchr2(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    }

    return 1;
}



 int modgetDampedSpringProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    cpVect ar;
    float *a,*b,res;
    switch (params[1])
    {
    case CP_C_ANCHR1:
        ar=cpDampedSpringGetAnchr1(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_ANCHR2:
        ar=cpDampedSpringGetAnchr2(constr);
        a=params[2];
        b=params[3];
        *a=ar.x;
        *b=ar.y;
        break;
    case CP_C_RESTLENGTH:
        res=cpDampedSpringGetRestLength(constr);
        return *(int *)&res;
        break;
    case CP_C_STIFFNESS:
        res=cpDampedSpringGetStiffness(constr);
        return *(int *)&res;
        break;
    case CP_C_DAMPING:
        res=cpDampedSpringGetDamping(constr);
        return *(int *)&res;
        break;
    }

    return 1;
}


 int modgetDampedRotarySpringProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    float res;
    switch (params[1])
    {
    case CP_C_RESTANGLE:
        res=cpDampedRotarySpringGetRestAngle(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    case CP_C_STIFFNESS:
        res=cpDampedRotarySpringGetStiffness(constr);
        return *(int *)&res;
        break;
    case CP_C_DAMPING:
        res=cpDampedRotarySpringGetDamping(constr);
        return *(int *)&res;
        break;
    }

    return 1;
}



 int modgetRotaryLimitJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    float res;
    switch (params[1])
    {
    case CP_C_MIN:
        res=cpRotaryLimitJointGetMin(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    case CP_C_MAX:
        res=cpRotaryLimitJointGetMax(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    }

    return 1;
}




 int modgetRatchetJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    float res;
    switch (params[1])
    {
    case CP_C_ANGLE:
        res=cpRatchetJointGetAngle(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    case CP_C_PHASE:
        res=cpRatchetJointGetPhase(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    case CP_C_RATCHET:
        res=cpRatchetJointGetRatchet(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    }

    return 1;
}



 int modgetGearJointProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    float res;
    switch (params[1])
    {
    case CP_C_RATIO:
        res=cpGearJointGetRatio(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    case CP_C_PHASE:
        res=cpGearJointGetPhase(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    }

    return 1;
}



 int modgetSimpleMotorProperties(INSTANCE * my, int * params)
{
    cpConstraint * constr=(cpConstraint *)params[0];
    float res;
    switch (params[1])
    {
    case CP_C_RATIO:
        res=cpSimpleMotorGetRate(constr);
        res=modChipmunkrad2deg(res);
        return *(int *)&res;
        break;
    }

    return 1;
}


 int modcpConstraintGetImpulse(INSTANCE * my, int * params)
{
    float a = cpConstraintGetImpulse(( cpConstraint * ) params[0]);
    return *((int * )&a);
}


 int modDefcpConstraint(INSTANCE * my, int * params)
{
    cpConstraint * espacio = ( cpConstraint * ) params[0];
    int res=params[1];

    switch (res)
    {
    case CP_C_MAXFORCE:
        espacio->maxForce=*( float*) &params[2];
        break;
    case CP_C_BIASCOEF:
        espacio->errorBias=*( float*) &params[2];
        break;
    case CP_C_MAXBIAS:
        espacio->maxBias=*( float*) &params[2];
        break;
    case CP_C_CA:
        espacio->a=( cpBody*) params[2];
        break;
    case CP_C_CB:
        espacio->b=( cpBody*) params[2];
        break;
    }
    return 0;
}

 int modGetcpConstraint(INSTANCE * my, int * params)
{
    cpConstraint * espacio = ( cpConstraint * ) params[0];
    switch (params[1])
    {
    case CP_C_MAXFORCE:
        return(*(int *)&espacio->maxForce);
        break;
    case CP_C_BIASCOEF:
        return(*(int *)&espacio->errorBias);
        break;
    case CP_C_MAXBIAS:
        return(*(int *)&espacio->maxBias);
        break;
    case CP_C_CA:
        return((int)espacio->a);
        break;
    case CP_C_CB:
        return((int)espacio->b);
        break;
    }
    return 0;
}

void modChipmunkJointPostSolve(cpConstraint *constraint, cpSpace *space){
   INSTANCE * r ;
   r = instance_new (((DatosConstraint *)constraint->data)->funcion, 0) ; // Create Function
   PRIDWORD(r, 0) = (int)constraint ;    // Argument 1
   instance_go(r) ; // Call Function
}

int modcpConstraintSetPostSolveFunc(INSTANCE * my, int * params){
    cpConstraint *constraint=params[0];
    char * postSolv = string_get(params[1]);
    modChipmunk_mayusStr(postSolv);
    PROCDEF * proc;
    if (strlen(postSolv)!=0){
        proc = procdef_get_by_name (postSolv); // Get definition function (name must be in uppercase)
        if (proc)
        {
            ((DatosConstraint *)constraint->data)->funcion=proc;
            string_discard(params[1]);
            cpConstraintSetPostSolveFunc(constraint, modChipmunkJointPostSolve);
            return 1;
        }
    }
    printf ("Función %s no encontrada\n", postSolv) ;
    string_discard(params[1]);
    return -1;
}

void modChipmunkJointPreSolve(cpConstraint *constraint, cpSpace *space){
   INSTANCE * r ;
   r = instance_new (((DatosConstraint *)constraint->data)->funcionPre, 0) ; // Create Function
   PRIDWORD(r, 0) = (int)constraint ;    // Argument 1
   instance_go(r) ; // Call Function
}

int modcpConstraintSetPreSolveFunc(INSTANCE * my, int * params){
    cpConstraint *constraint=params[0];
    char * preSolv = string_get(params[1]);
    modChipmunk_mayusStr(preSolv);
    PROCDEF * proc;
    if (strlen(preSolv)!=0){
        proc = procdef_get_by_name (preSolv); // Get definition function (name must be in uppercase)
        if (proc)
        {
            ((DatosConstraint *)constraint->data)->funcionPre=proc;
            string_discard(params[1]);
            cpConstraintSetPreSolveFunc(constraint, modChipmunkJointPostSolve);
            return 1;
        }
    }
    printf ("Función %s no encontrada\n", preSolv) ;
    string_discard(params[1]);
    return -1;
}

int modGetFathetA(INSTANCE * my, int * params){
    return((DatosConstraint *)((cpConstraint*)params[0])->data)->fatherA;
}

int modGetFathetB(INSTANCE * my, int * params){
    return((DatosConstraint *)((cpConstraint*)params[0])->data)->fatherB;
}

static void
rompeJoint(cpSpace *space, cpConstraint *joint, void *unused)
{
	cpSpaceRemoveConstraint(space, joint);
	cpConstraintFree(joint);
}


int modremoveConstraint(INSTANCE * my, int * params)
{
    cpConstraint * c=params[0];
//    INSTANCE * a = instance_get( ((DatosConstraint *)((cpConstraint *)params[1])->data)->fatherA ) ;
//    cpBody *bod =(cpBody *)LOCDWORD( mod_chipmunk, a, LOC_BODY );
//    int z,i;
//#ifdef DEBUG
//    printf("removido %d \n",params[1]);
//#endif
//    if (bod){
//    void * cons=LLbusca(((DataPointer)bod->data)->Constraints,params[1],modChipmunkCompara);
//        if (cons!=NULL){
//            eliminaConstraint(cons);
//        }else{
//            printf("No se encontro %d\n",params[1]);
//        }
//        }
        if (cpSpaceIsLocked(modChipmunk_cpEspacio))
            cpSpaceAddPostStepCallback(modChipmunk_cpEspacio, (cpPostStepFunc)rompeJoint, c, NULL);
        else{
            cpSpaceRemoveConstraint(modChipmunk_cpEspacio, c);
            cpConstraintFree(c);
        }
      return 0;
}




//void eliminaConstraint(void * par){
//
//    cpConstraint * c=par;
//    modChipmunkStruct_nodo * n;
////    printf("inseer %d\n",c); fflush(stdout);
//        if (cpConstraintGetA(c)!=modChipmunk_cpEspacio->staticBody){    //el cuerpo estatico del fondo no almacena los constraints con los que interactúa, así que si el cuerpo del constraint es el fondo, que no intente borrar de él su registro
//            n=((DataPointer)(cpConstraintGetA(c))->data)->Constraints;
//            for(;n->sig!=NULL;n=n->sig){
//                if (c==n->sig->elem){
//#ifdef DEBUG
//                        printf("eliminado en A %d\n",c); fflush(stdout);
//#endif
//                        free(n->sig);
//                        n->sig=n->sig->sig;
//                        break;
//                }
//            }
//        }
//if (cpConstraintGetB(c)!=modChipmunk_cpEspacio->staticBody){    //el cuerpo estatico del fondo no almacena los constraints con los que interactúa, así que si el cuerpo del constraint es el fondo, que no intente borrar de él su registro
//        n=((DataPointer)(cpConstraintGetB(c))->data)->Constraints;
//        for(;n->sig!=NULL;n=n->sig){
//            if (c==n->sig->elem){
//#ifdef DEBUG
//                printf("eliminado en B %d\n",c); fflush(stdout);
//#endif
//                    free(n->sig);
//                    n->sig=n->sig->sig;
//                    break;
//            }
//        }
//}
//    cpSpaceRemoveConstraint(modChipmunk_cpEspacio, c);
//    cpConstraintFree(c);
//}
