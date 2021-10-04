#include "Espacio.h"

extern cpSpace * modChipmunk_cpEspacio;

int modcpSpaceGetCurrentTimeStep(INSTANCE * my, int * params){
    float r=cpSpaceGetCurrentTimeStep(modChipmunk_cpEspacio);
    return *(int*)&r;
}



int modGetStaticBody(INSTANCE * my, int * params)
{
    return modChipmunk_cpEspacio->staticBody;
}

 int modcpSpaceActivateShapesTouchingShape(INSTANCE * my, int * params)
{
    cpSpaceActivateShapesTouchingShape(modChipmunk_cpEspacio,( cpShape * ) params[0]);
    return 0;
}
 int modcpSpaceResizeStaticHash(INSTANCE * my, int * params)
{
    //cpHashResizeHash(modChipmunk_cpEspacio,*(( float * ) &params[0]),params[1]);
    printf("Función en desuso\n");
    return 0;
}

 int modcpSpaceResizeActiveHash(INSTANCE * my, int * params)
{
   // cpSpaceResizeActiveHash(modChipmunk_cpEspacio,*(( float * ) &params[0]),params[1]);
    printf("Función en desuso\n");
    return 0;
}

int modcpSpaceUseSpatialHash(INSTANCE * my, int * params)
{
    cpSpaceUseSpatialHash(modChipmunk_cpEspacio, *(float*)&params[0], params[1]);
    return 1;
}

