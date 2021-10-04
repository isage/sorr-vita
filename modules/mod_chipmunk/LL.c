#include "LL.h"
//#include <stdio.h>

void LLinicializa(modChipmunkStruct_nodo ** n){
    *n= malloc(sizeof(modChipmunkStruct_nodo));
    (*n)->elem=NULL;
    (*n)->sig=NULL;
}

void LLagrega(modChipmunkStruct_nodo * n, void * p){
    modChipmunkStruct_nodo * sig=n->sig;
    n->sig= malloc(sizeof(modChipmunkStruct_nodo));
    n->sig->elem=p;
    n->sig->sig=sig;
}

void LLelimina(modChipmunkStruct_nodo * n,void * am, modChipmunk_funCom r, modChipmunk_funcionElm funEl,int hFree){
    for(;n->sig!=NULL;n=n->sig){
        if (r(am,n->sig->elem)){
            modChipmunkStruct_nodo * sig = n->sig->sig;
            funEl(n->sig->elem);
            if (hFree)
                free(n->sig);
            n->sig=sig;
            if (n->sig==NULL)
                return;
        }
    }
}

void * LLbusca(modChipmunkStruct_nodo * n,void * bm, modChipmunk_funCom r){
    for(;n->sig!=NULL;n=n->sig){
        if (r(bm,n->sig->elem)){
//            printf("busca  %d  %d\n",n->sig->elem, bm); fflush(stdout);
                return n->sig->elem;
        }
    }
    return NULL;
}

void LLeliminaTodo(modChipmunkStruct_nodo ** nod,modChipmunk_funcionElm funEl,int hFree){
    modChipmunkStruct_nodo *n=*nod;
    while(n!=NULL && n->sig!=NULL){
            modChipmunkStruct_nodo * sig = n->sig->sig;
            funEl(n->sig->elem);
            if (hFree){
                free(n->sig);
                n=n->sig;
            }
    }
        if (*nod!=NULL){
        free(*nod);
        }
        *nod=NULL;
}

void LLimprime(modChipmunkStruct_nodo * n){
    if (n==NULL){
        printf("Lista vacia \n");
        return;
        }
    printf("____________________________\n");fflush(stdout);
    for(;n->sig!=NULL;n=n->sig){
            printf("%d\n",n->sig->elem);fflush(stdout);
    }
    printf("____________________________\n");fflush(stdout);
}
