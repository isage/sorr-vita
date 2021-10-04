#include "Handlers.h"
#include "estructuras.h"
#include "globales.h"
extern cpSpace * modChipmunk_cpEspacio;
extern int modChipmunk_Crear;
extern modChipmunkStruct_nodo * modChipmunk_ListaHandlers;

typedef struct {int a,b;} modChipmunkStruct_param2d;

modChipmunkStruct_param2d modChipmunkParam(int a,int b){
    modChipmunkStruct_param2d v;
    v.a=a;
    v.b=b;
return v;
}
int compara(void *a, void* bp){
    modChipmunkStruct_param2d* am=a;
    Hands* bm=bp;
    if ((am->a == bm->a && am->b == bm->b) || (am->b == bm->a && am->a == bm->b))
        return 1;
    return 0;
}

 int modChipmunkBeginCall(cpArbiter *arb, struct cpSpace *space, void *data){
    int ret=1;
   INSTANCE * r ;
   Hands * d=data;
   r = instance_new ((d->funciones)[0], 0) ; // Create Function
   PRIDWORD(r, 0) = (int)arb ;    // Argument 1
   PRIDWORD(r, 4) = (d->parametros)[0] ;    // Argument 2
   ret = instance_go(r) ; // Call Function
   return ret;
}

int modChipmunkPreSolveCall(cpArbiter *arb, struct cpSpace *space, void *data){
    int ret=1;
   INSTANCE * r ;
   Hands * d=data;
   r = instance_new ((d->funciones)[1], 0) ; // Create Function
   PRIDWORD(r, 0) = (int)arb ;    // Argument 1
   PRIDWORD(r, 4) = (d->parametros)[1] ;    // Argument 2
   ret = instance_go(r) ; // Call Function
   return ret;
}

void modChipmunkPostSolveCall(cpArbiter *arb, struct cpSpace *space, void *data){
   INSTANCE * r ;
   Hands * d=data;
   r = instance_new ((d->funciones)[2], 0) ; // Create Function
   PRIDWORD(r, 0) = (int)arb ;    // Argument 1
   PRIDWORD(r, 4) = (d->parametros)[2] ;    // Argument 2
   instance_go(r) ; // Call Function
}

void modChipmunkSeparateCall(cpArbiter *arb, struct cpSpace *space, void *data){
   INSTANCE * r ;
   Hands * d=data;
   r = instance_new ((d->funciones)[3], 0) ; // Create Function
   PRIDWORD(r, 0) = (int)arb ;    // Argument 1
   PRIDWORD(r, 4) = (d->parametros)[3] ;    // Argument 2
   instance_go(r) ; // Call Function
}

Hands * modChipmunkNuevoHandler(){
    Hands * h=malloc(sizeof(Hands));
    memset(h->funciones,0,sizeof(PROCDEF *)*4);  //setear enteros
    memset(h->parametros,0,sizeof(void *)*4);
    return h;
}

void modChipmunkFalloHandler(int * params,Hands * h){
string_discard(params[2]);
    string_discard(params[3]);
    string_discard(params[4]);
    string_discard(params[5]);
free (h);
}

 int modcpSpaceAddCollisionHandler(INSTANCE * my, int * params){
    modChipmunkStruct_param2d par=modChipmunkParam(params[0], params[1]);
    if (LLbusca(modChipmunk_ListaHandlers, &par, compara)){
        printf("Se intentó crear un handler a (%d, %d), pero ya existe uno \n",params[0],params[1]);
        return -1;
    }

    Hands * handler = modChipmunkNuevoHandler();
    char * begins = string_get(params[2]);
    char * preSolves = string_get(params[3]);
    char * postSolves = string_get(params[4]);
    char * separates = string_get(params[5]);
    modChipmunk_mayusStr(begins);
    modChipmunk_mayusStr(preSolves);
    modChipmunk_mayusStr(postSolves);
    modChipmunk_mayusStr(separates);
    handler->a=params[0];
    handler->b=params[1];

    PROCDEF * proc;
    if (strlen(begins)!=0){
        proc = procdef_get_by_name (begins); // Get definition function (name must be in uppercase)
        if (!proc)
        {
            printf ("Función %s no encontrada\n", begins) ;
            modChipmunkFalloHandler(params,handler);
            return -1 ;
        }
        handler->funciones[0]=proc;
        handler->parametros[0]=(void *)params[6];
    }else{
        handler->funciones[0]=NULL;
        handler->parametros[0]=NULL;
    }

    if (strlen(preSolves)!=0){
        proc = procdef_get_by_name (preSolves); // Get definition function (name must be in uppercase)
        if (!proc)
        {
            printf ("Función %s no encontrada\n", preSolves) ;
            modChipmunkFalloHandler(params,handler);
            return -1 ;
        }
        handler->funciones[1]=proc;
        handler->parametros[1]=(void *)params[7];
    }else{
        handler->funciones[1]=NULL;
        handler->parametros[1]=NULL;
    }

    if (strlen(postSolves)!=0){
        proc = procdef_get_by_name (postSolves); // Get definition function (name must be in uppercase)
        if (!proc)
        {
            printf ("Función %s no encontrada\n", postSolves) ;
            modChipmunkFalloHandler(params,handler);
            return -1 ;
        }
        handler->funciones[2]=proc;
        handler->parametros[2]=(void *)params[8];
    }else{
        handler->funciones[2]=NULL;
        handler->parametros[2]=NULL;
    }

    if (strlen(separates)!=0){
        proc = procdef_get_by_name (separates); // Get definition function (name must be in uppercase)
        if (!proc)
        {
            printf ("Función %s no encontrada\n", separates) ;
            modChipmunkFalloHandler(params,handler);
            return -1 ;
        }
        handler->funciones[3]=proc;
        handler->parametros[3]=(void *)params[9];
    }else{
        handler->funciones[3]=NULL;
        handler->parametros[3]=NULL;
    }

    cpSpaceAddCollisionHandler(
        modChipmunk_cpEspacio,
        handler->a,handler->b,
        handler->funciones[0]? modChipmunkBeginCall : NULL, //si la string es null, poner null, de lo contrario poner beginCall
        handler->funciones[1]? modChipmunkPreSolveCall : NULL,
        handler->funciones[2]? modChipmunkPostSolveCall : NULL,
        handler->funciones[3]? modChipmunkSeparateCall : NULL,
        (void *)handler
    );
    string_discard(params[2]);
    string_discard(params[3]);
    string_discard(params[4]);
    string_discard(params[5]);
    LLagrega(modChipmunk_ListaHandlers,handler);
    return 1;
}

int modcpSpaceRemoveCollisionHandler(INSTANCE * my, int * params){
    //cpSpaceRemoveCollisionHandler(modChipmunk_cpEspacio, params[0], params[1],funcionElmHand);
    modChipmunkStruct_param2d par=modChipmunkParam(params[0], params[1]);
    LLelimina(modChipmunk_ListaHandlers, &par, compara,funcionElmHand,1);
    return 1;
}

void funcionElmHand(void * v){
    Hands * h=v;
    cpSpaceRemoveCollisionHandler(modChipmunk_cpEspacio, h->a, h->b);
    free (h);
}

void recogeColisionHandler(cpArbiter *arb, cpSpace *space, void *dat)
{
    modChipmunkStruct_colHand* col= (modChipmunkStruct_colHand*)dat;
    cpArray* arr=  col->arr;
    CP_ARBITER_GET_SHAPES(arb, a, b);
    cpContactPointSet set = cpArbiterGetContactPointSet(arb);
    cpContactPointSetM * setM =(cpContactPointSetM*) malloc(sizeof(cpContactPointSetM));

    int i;
    for (i=0; i<set.count; i++)
    {
        setM->points[i].Point.x=set.points[i].point.x;
        setM->points[i].Point.y=set.points[i].point.y;
        setM->points[i].normal.x=set.points[i].normal.x;
        setM->points[i].normal.y=set.points[i].normal.y;
        setM->points[i].dist=set.points[i].dist;
    }
    setM->count=set.count;
    DataPointer dato=(DataPointer)a->body->data;
    if (dato)
        setM->id1=dato->father;
    else
        setM->id1=0;
    setM->shape1=a;
    dato=(DataPointer)b->body->data;
    if (dato)
        setM->id2=dato->father;
    else
        setM->id2;
    setM->shape2=b;
    modChipmunk_ArregloPush(arr, setM);
    //printf("%p",setM);
}

void buscaABCol(void *ptr, void *data)
{
    modChipmunkStruct_colHand* cla=(modChipmunkStruct_colHand*) ptr;
    int * arr=(int *)data;
    if (cla->a==arr[0] && cla->b==arr[1])
    {
        modChipmunk_Crear=0;
    }

}

int modcrearHandler(INSTANCE * my, int * params)
{
    modChipmunk_Crear=1;
    int par[2];
    par[0]=params[0];
    par[1]=params[1];
    modChipmunk_arregloItera(HandlerColisions, buscaABCol, &par);

    if (!modChipmunk_Crear)
        return 0;
    cpArray* arr=modChipmunk_nuevoArreglo(10);
    cpArray* arr2=modChipmunk_nuevoArreglo(10);
    modChipmunkStruct_colHand* col= malloc(sizeof(modChipmunkStruct_colHand));

    col->arr=arr;
    col->colisiones=arr2;

    col->a=params[0];
    col->b=params[1];

    modChipmunk_ArregloPush(HandlerColisions, col);
    cpSpaceAddCollisionHandler(modChipmunk_cpEspacio, params[0], params[1], NULL, NULL, recogeColisionHandler, NULL, col);
    return (int) col;
}

int modremoveHandler(INSTANCE * my, int * params)
{
    modChipmunkStruct_colHand* col=(modChipmunkStruct_colHand*)params[0];
    cpSpaceRemoveCollisionHandler(modChipmunk_cpEspacio, col->a, col->b);
    cpArrayDeleteObj(HandlerColisions, col);
   // modChipmunk_arregloItera(col->arr, (cpArrayIter)eliminaContactPointSetM, NULL);
    modChipmunk_destruyeArreglo(&col->arr,1);
    //modChipmunk_arregloItera(col->colisiones, (cpArrayIter)eliminaContactPointSetM, NULL);
    modChipmunk_destruyeArreglo(&col->colisiones,1);
    free(col);
    return 0;
}

