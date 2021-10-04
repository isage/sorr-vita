#include "automGenTerr.h"
#include "bgddl.h"
#include "bgdrtm.h"
#include "stdio.h"
#include "libdraw.h"
#include "mod_map.h"
#include "librender.h"
#include "dlvaracc.h"
#include "Arreglos.h"
#include "globales.h"
#include "inl.h"



void eliminaVect(void *ptr, void *data){
    modChipmunk_destruyeArreglo((modChipmunkStruct_Arreglo **)&ptr,1);
}

void modChipmunkEliminaTerrenoC(modChipmunkStruct_Arreglo * arr){
    modChipmunk_arregloItera(arr, eliminaVect, NULL);
    modChipmunk_destruyeArreglo(&arr,0);
}
int modChipmunkEliminaTerreno(INSTANCE * my, int * params){
    modChipmunkStruct_Arreglo * arr=(modChipmunkStruct_Arreglo *)params[0];
    modChipmunkEliminaTerrenoC(arr);

}

int modChipmunkGetElement(INSTANCE * my, int * params){
    return mCh_AAt(params[0],params[1]);
}

int modChipmunkCantElement(INSTANCE * my, int * params){
    return modChipmunk_ArregloCantidad(params[0]);
}



int modChipmunkGeneraTerreno(INSTANCE * my, int * params){
    GRAPH * map=bitmap_get( params[0], params[1] );
    map=modChipmunkObtenTodosBordes( map);
    //return map->code;
    if (map==0)
        return 0;
    int anc=map->width;
    int alt=map->height;
    modChipmunkStruct_Arreglo * arr= modChipmunk_nuevoArreglo(5);
    int i,j;
    for (i=0;i<anc;i++){
        for (j=0;j<alt;j++){
            if (gr_get_pixel( map, i,j)==1){
                //printf("{%d %d}\n",i,j);
                modChipmunkStruct_Arreglo * list=modChipmunkVertices(map,i,j);
                modChipmunk_ArregloPush(arr,list);
                }
        }
    }
    int z;
    int cant=modChipmunk_ArregloCantidad(arr);
    modChipmunkStruct_Arreglo * res= modChipmunk_nuevoArreglo(cant);
    for (z=0;z<cant;z++){
         modChipmunkStruct_Arreglo * v=mCh_AAt(arr,z);
         modChipmunkStruct_Arreglo * list=aproxP(v,*(float*)&params[2]);
         modChipmunk_ArregloPush(res,list);
    }
    modChipmunkEliminaTerrenoC(arr);
    bitmap_destroy(map);
    return (int)res;

}

static inline float cuad(float v){
    return v*v;
}

static inline float fdist(float ix,float iy,float fx,float fy){
    return sqrt(cuad(ix-fx) + cuad(iy-fy));
}

//x3,y3 son las coordenadas del punto
static inline float rectdist(float x1,float y1,float x2,float y2,float x3,float y3){
    float up=(y1-y2);
    float down=(x1-x2);
    if (up==0)  //la misma y
        return y3-y1;
    if (down==0)  //la misma x
        return x3-x1;
    float m=up/down;
    float b=y1-(m*x1);
    return fabs(m*x3-y3+b)/sqrt(cuad(m)+1);
}

void centroide(modChipmunkStruct_Arreglo *coord,int *x,int *y){
    int cant=modChipmunk_ArregloCantidad(coord);
     cpVect *v;
     int z;
     float x1=0,y1=0;
     for (z=0;z<cant;z++){
         v=mCh_AAt(coord,z);
         x1+=v->x;
         y1+=v->y;
     }
     x1/=cant;
     y1/=cant;
     *x=x1;
     *y=y1;
}

int maxV(float v[],int c){
    int z;
    int maxV=0;
    for (z=0;z<c;z++){
        if (v[z]>v[maxV])
            maxV=z;
    }
    return maxV;
}

static inline int iguales(cpVect *a, cpVect *b,float c){
    return (fabs(a->x - b->x)+fabs(a->y - b->y))<c;
}

//Resolver la cuestión de los polígonos abiertos...
 modChipmunkStruct_Arreglo *aproxP(modChipmunkStruct_Arreglo *coord,float umb){
     int cant=modChipmunk_ArregloCantidad(coord);
     cpVect *v;
     int z;
     int x,y,inic,fin;
     int cerrado;
     uint8_t res[cant];
     for (z=0;z<cant;z++)
            res[z]=0;

     if (iguales(mCh_AAt(coord,0),mCh_AAt(coord,cant-1),5)){
         cerrado=1;
         centroide(coord,&x,&y);
         v=mCh_AAt(coord,cant-1);
         float distancias[cant];
         for (z=0;z<cant;z++){
             v=mCh_AAt(coord,z);
             distancias[z]=fdist(x,y,v->x,v->y);
         }
         inic = maxV(distancias,cant);
         res[inic]=1;

         v=mCh_AAt(coord,inic);
         x=v->x;
         y=v->y;
         for (z=0;z<cant;z++){
             v=mCh_AAt(coord,z);
             distancias[z]=fdist(x,y,v->x,v->y);
         }
         fin = maxV(distancias,cant);
         res[fin]=1;

         recursApro(inic,fin,coord,res,umb);        //llama recursividad para dividir por cuerpos poligonales usando la linea del índice inc al fin que son los índices del principio y final de la primer linea
         recursApro(fin,inic,coord,res,umb);
    }else{
        cerrado=0;
        inic=0;
        fin=cant-1;
        res[inic]=1;
        res[fin]=1;
        recursApro(inic,fin,coord,res,umb);
    }

     int cantRes=0;
     for (z=0;z<cant;z++){
        if (res[z]==1)
            cantRes++;
     }

     if (cerrado)
        cantRes++;
     modChipmunkStruct_Arreglo * resArr= modChipmunk_nuevoArreglo(cantRes);

     for (z=0;z<cant;z++){
        if (res[z]==1){
            modChipmunk_ArregloPush(resArr,mCh_AAt(coord,z));  //lo copia al resultado
            mCh_AAtS(coord,z,0);//y lo borra de la lista original
        }

     }

     if (cerrado){  //si la curva es cerrada, terminar donde comenzó.
         v=mCh_AAt(resArr,0);
         cpVect *i=(cpVect*)malloc(sizeof(cpVect));
         i->x=v->x; i->y=v->y;
         modChipmunk_ArregloPush(resArr,i);  //lo copia al resultado
     }
     /*printf("----------------------------------------\n");
     for (z=0;z<cantRes;z++){
         v=mCh_AAt(resArr,z);
         printf("--[%f %f]\n",v->x,v->y);
     }
     printf("----------------------------------------\n");
    */
     return resArr;
 }


void recursApro(int inic,int fin,modChipmunkStruct_Arreglo *coord,uint8_t res[],float umb){
    cpVect *iniV,*finV,*auxV;
    int ind;
    if (fin<inic){   //buffer circular
        int cant=modChipmunk_ArregloCantidad(coord);
        //printf("%d,,%d,,%d\n",fin,inic,cant);
        int cant2=fin+(cant-inic);
        float distancias[cant2];
        int z;
        iniV=mCh_AAt(coord,fin);
        finV=mCh_AAt(coord,inic);
        for (z=0;z<fin;z++){
            auxV=mCh_AAt(coord,z);
            distancias[z]=rectdist(iniV->x,iniV->y,finV->x,finV->y,auxV->x,auxV->y);
        }
        int indAux=z;
        for (z=inic;z<cant;z++){
            auxV=mCh_AAt(coord,z);
            distancias[indAux]=rectdist(iniV->x,iniV->y,finV->x,finV->y,auxV->x,auxV->y);
            indAux++;
        }
        ind = maxV(distancias,cant2);
        if (distancias[ind]>umb){
            if (ind>fin){             //ajusta i al arreglo completo
                ind+=inic-fin;
            }
            res[ind]=1;
            if (ind!=0)
                recursApro(inic,ind-1,coord,res,umb);
            else if(!(inic>=cant-1))
                recursApro(inic,cant-1,coord,res,umb);

            if (ind!=cant-1)
                recursApro(ind,fin,coord,res,umb);
            else if (fin!=0)
                recursApro(0,fin,coord,res,umb);
        }
        return;
    }
    int cant=fin-inic;
    float distancias[cant];
    int z;
    iniV=mCh_AAt(coord,inic);
    finV=mCh_AAt(coord,fin);
    for (z=inic;z<fin;z++){
        auxV=mCh_AAt(coord,z);
        distancias[z-inic]=rectdist(iniV->x,iniV->y,finV->x,finV->y,auxV->x,auxV->y);
        //printf("(%.1f %.0f %.0f)\t",distancias[z-inic],auxV->x,auxV->y);
    }
    ind = maxV(distancias,cant);
    //printf("\nelegido(%f %f %f)\n",distancias[ind],auxV->x,auxV->y);
    if (distancias[ind]>umb){
        ind+=inic;
        res[ind]=1;
        if (ind!=inic)
            recursApro(inic,ind,coord,res,umb);
        if (ind!=(fin-1))
            recursApro(ind,fin,coord,res,umb);
    }
}



#define I(a,b,c) (gr_get_pixel( a, b,c )==1 )
modChipmunkStruct_Arreglo * modChipmunkVertices(GRAPH * map,int i,int j){
    modChipmunkStruct_Arreglo * arr= modChipmunk_nuevoArreglo(100);
    cpVect *v;
    while (1){
        v=(cpVect*)malloc(sizeof(cpVect));
        v->x=i;
        v->y=j;
        modChipmunk_ArregloPush(arr,v);
        gr_put_pixel(map,i,j,0);
        if (I(map,i,j+1)){
            j=j+1;
            continue;
        }
        if (I(map,i+1,j+1)){
            j=j+1;
            i=i+1;
            continue;
        }
        if (I(map,i+1,j)){
            i=i+1;
            continue;
        }
        if (I(map,i+1,j-1)){
            j=j-1;
            i=i+1;
            continue;
        }
        if (I(map,i,j-1)){
            j=j-1;
            continue;
        }
        if (I(map,i-1,j-1)){
            j=j-1;
            i=i-1;
            continue;
        }
        if (I(map,i-1,j)){
            i=i-1;
            continue;
        }
        if (I(map,i-1,j+1)){
            j=j+1;
            i=i-1;
            continue;
        }
        break;
    }
    return arr;

}

#undef I
#define I(a,b,c) (gr_get_pixel( a, b,c )!=0 && gr_get_pixel( a, b,c )!=0x00ffffff && gr_get_pixel( a,b,c )!=0xff000000)
GRAPH * modChipmunkObtenTodosBordes(GRAPH * mapa1)
{
    if (mapa1==NULL)
        return(0);
    int anc,alt;
    anc=mapa1->width;
    alt=mapa1->height;
    int i,j;
    GRAPH * map = bitmap_new_syslib( anc,alt,1 ) ;

    if ( map ){
        for(i=0,j=0;i<anc;i++)
            gr_put_pixel( map, i,  j ,0);
        for(i=0,j=alt-1;i<anc;i++)
            gr_put_pixel( map, i,  j ,0);
        for(i=0,j=0;j<alt;j++)
            gr_put_pixel( map, i,  j ,0);
        for(i=anc-1,j=0;j<alt;j++)
            gr_put_pixel( map, i,  j ,0);

        anc-=1;
        alt-=1;
        for(i=1;i<anc;i++){
            for (j=1;j<alt;j++){
                gr_put_pixel( map, i,  j ,I(mapa1, i,j) && !( I( mapa1, i,j-1 ) &&
                                                              I( mapa1, i-1,j ) && I( mapa1, i+1,j ) &&
                                                              I( mapa1, i,j+1 )
                                                            )
                             );
            }
        }
        int chi;
        for(i=1;i<anc;i++){
            for (j=1;j<alt;j++){
                if (gr_get_pixel( map, i,j )==1){
                    chi= (gr_get_pixel( map,i+1,j)!=gr_get_pixel( map,i,j-1))+ (gr_get_pixel( map,i,j-1)!=gr_get_pixel( map,i-1,j))+ (gr_get_pixel( map,i-1,j)!=gr_get_pixel( map,i,j+1))+ (gr_get_pixel( map,i,j+1)!=gr_get_pixel( map,i+1,j))+
                         2*(
                            ((!gr_get_pixel( map,i+1,j))&gr_get_pixel( map,i+1,j-1)&(!gr_get_pixel( map,i,j-1)))+((!gr_get_pixel( map,i,j-1))&gr_get_pixel( map,i-1,j-1)&(!gr_get_pixel( map,i-1,j)))+((!gr_get_pixel( map,i-1,j))&gr_get_pixel( map,i-1,j+1)&(!gr_get_pixel( map,i,j+1)))+((!gr_get_pixel( map,i,j+1))&gr_get_pixel( map,i+1,j+1)&(!gr_get_pixel( map,i+1,j)))
                            );
                    if (chi==2 &&
                        (   (gr_get_pixel( map,i-1,j)==1 && gr_get_pixel( map,i,j+1)==1) ||
                            (gr_get_pixel( map,i-1,j)==1 && gr_get_pixel( map,i,j-1)==1) ||
                            (gr_get_pixel( map,i+1,j)==1 && gr_get_pixel( map,i,j+1)==1) ||
                            (gr_get_pixel( map,i+1,j)==1 && gr_get_pixel( map,i,j-1)==1)
                         )
                        )
                    gr_put_pixel( map, i,  j ,0);
                }

            }
        }

    }
    return map;
}
