#ifndef AUTOGENTERR_H
#define AUTOGENTERR_H
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

int modChipmunkGeneraTerreno(INSTANCE * my, int * params);
GRAPH * modChipmunkObtenTodosBordes(GRAPH * mapa1);
void recursApro(int inic,int fin,modChipmunkStruct_Arreglo *coord,uint8_t res[],float umb);
modChipmunkStruct_Arreglo * modChipmunkVertices(GRAPH * map,int i,int j);
modChipmunkStruct_Arreglo *aproxP(modChipmunkStruct_Arreglo *coord,float umb);
int modChipmunkEliminaTerreno(INSTANCE * my, int * params);
int modChipmunkGetElement(INSTANCE * my, int * params);
int modChipmunkCantElement(INSTANCE * my, int * params);
#endif
