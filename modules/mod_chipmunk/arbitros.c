#include "arbitros.h"
typedef
struct modChipmunkStruct_Data_Pointer
{
    int father;
    int estado;
    struct modChipmunkStruct_Data_Pointer * sig;
    int x,y;
    cpBody * body;
    int angle;
    int nShapes;
    cpShape ** Shapes;
    int nConstraints;
    cpConstraint ** Constraints;
    cpBody ** constraintsBody;
    int typeShape;
} *DataPointer;

int modArbiterGetNumContactPoints(INSTANCE * my, int * params){
    return cpArbiterGetCount((cpArbiter *)params[0]);
}

int modArbiterIsFirstContact(INSTANCE * my, int * params){
    return cpArbiterIsFirstContact((cpArbiter *)params[0]);
}

int modArbiterGetNormal(INSTANCE * my, int * params){
    cpVect v=cpArbiterGetNormal((cpArbiter *)params[0],params[1]);
    *(float*)params[2]=v.x;
    *(float*)params[3]=v.y;
    return 1;
}

int modArbiterGetPoint(INSTANCE * my, int * params){
    cpVect v=cpArbiterGetPoint((cpArbiter *)params[0],params[1]);
    *(float*)params[2]=v.x;
    *(float*)params[3]=v.y;
    return 1;
}

int modArbiterGetDepth(INSTANCE * my, int * params){
    float f=cpArbiterGetDepth((cpArbiter *)params[0],params[1]);
    return *(int*)&f;
}

int modArbiterTotalImpulseWithFriction(INSTANCE * my, int * params){
    cpVect v=cpArbiterTotalImpulseWithFriction((cpArbiter *)params[0]);
    *(float*)params[1]=v.x;
    *(float*)params[2]=v.y;
    return 1;
}

int modArbiterTotalImpulse(INSTANCE * my, int * params){
    cpVect v=cpArbiterTotalImpulse((cpArbiter *)params[0]);
    *(float*)params[1]=v.x;
    *(float*)params[2]=v.y;
    return 1;
}

int modArbiterProcessA(INSTANCE * my, int * params){
    cpBody * b,*c;
    cpArbiterGetBodies((cpArbiter *)params[0], &b,&c);
    return ((DataPointer)b->data)->father;
}

int modArbiterProcessB(INSTANCE * my, int * params){
    cpBody * b,*c;
    cpArbiterGetBodies((cpArbiter *)params[0], &b,&c);
    return ((DataPointer)c->data)->father;
}

int modArbiterGetElasticity(INSTANCE * my, int * params){
    float f=cpArbiterGetElasticity((cpArbiter *)params[0]);
    return *(int*)&f;
}

int modArbiterSetElasticity(INSTANCE * my, int * params){
    cpArbiterSetElasticity((cpArbiter *)params[0],*(float*)&params[1]);
    return 1;
}

int modArbiterGetFriction(INSTANCE * my, int * params){
    float f=cpArbiterGetFriction((cpArbiter *)params[0]);
    return *(int*)&f;
}

int modArbiterSetFriction(INSTANCE * my, int * params){
    cpArbiterSetFriction((cpArbiter *)params[0],*(float*)&params[1]);
    return 1;
}

int modArbiterGetSurfaceVelocity(INSTANCE * my, int * params){
    cpVect v=cpArbiterGetSurfaceVelocity((cpArbiter *)params[0]);
    *(float*)params[1]=v.x;
    *(float*)params[2]=v.y;
    return 1;
}

int modArbiterSetSurfaceVelocity(INSTANCE * my, int * params){
    cpArbiterSetSurfaceVelocity((cpArbiter *)params[0],cpv(*(float*)&params[1],*(float*)&params[2]));
    return 1;
}
