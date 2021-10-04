#ifndef ARREGLOS_H
#define ARREGLOS_H

#define AVANCE 20

typedef struct{
    int cant;
    int max;
    void ** arreglo;
} modChipmunkStruct_Arreglo;



modChipmunkStruct_Arreglo * modChipmunk_nuevoArreglo(int tam);
void modChipmunk_destruyeArreglo(modChipmunkStruct_Arreglo ** a, int eliminaMiembros);
void modChipmunk_ArregloPush(modChipmunkStruct_Arreglo * a, void * o);
void * modChipmunk_ArregloPop(modChipmunkStruct_Arreglo * a);
typedef void (*modChipmunk_itF )(void *ptr, void *data);
void modChipmunk_arregloItera(modChipmunkStruct_Arreglo * a, modChipmunk_itF iterador, void *datos);
int modChipmunk_ArregloCantidad(modChipmunkStruct_Arreglo * a);
void  modChipmunk_ArregloLimpia(modChipmunkStruct_Arreglo * a, int eliminaMiembros);
void * mCh_AAt(modChipmunkStruct_Arreglo * a,int p);
void mCh_AAtS(modChipmunkStruct_Arreglo * a,int p,void * v);
#endif // ARREGLOS_H
