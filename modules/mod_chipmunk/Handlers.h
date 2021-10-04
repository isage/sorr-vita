#ifndef HANDLERS_H
#define HANDLERS_H

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


int beginCall(cpArbiter *arb, struct cpSpace *space, void *data);
int modcpSpaceAddCollisionHandler(INSTANCE * my, int * params);
int modcpSpaceRemoveCollisionHandler(INSTANCE * my, int * params);
int modremoveHandler(INSTANCE * my, int * params);
int modcrearHandler(INSTANCE * my, int * params);
void recogeColisionHandler(cpArbiter *arb, cpSpace *space, void *dat);
void buscaABCol(void *ptr, void *data);
typedef struct {
    PROCDEF* funciones[4];
    void * parametros[4];
    int a,b;
} Hands;
void funcionElmHand(void * v);

#endif
