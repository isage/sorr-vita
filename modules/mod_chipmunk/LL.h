#ifndef LL_H
#define LL_H

#include <stdio.h>

typedef struct modChipmunkStruct_nodo{
    void * elem;
    struct modChipmunkStruct_nodo * sig;
} modChipmunkStruct_nodo;

typedef void (*modChipmunk_funcionElm)(void* parametro) ;

typedef int (*modChipmunk_funCom)(void *a, void* b) ;
void LLeliminaTodo(modChipmunkStruct_nodo ** n,modChipmunk_funcionElm funEl,int hFree);
void LLinicializa(modChipmunkStruct_nodo ** n);
void LLagrega(modChipmunkStruct_nodo * n, void * p);
void LLelimina(modChipmunkStruct_nodo * n,void * b, modChipmunk_funCom r,modChipmunk_funcionElm funEl,int hFree);
void * LLbusca(modChipmunkStruct_nodo * n,void * b, modChipmunk_funCom r);
void LLimprime(modChipmunkStruct_nodo * n);

#endif //LL_H
