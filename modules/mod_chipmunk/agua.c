#include "inl.h"
#include "agua.h"
#include "estructuras.h"

extern cpSpace * modChipmunk_cpEspacio;

//typedef void (*cpSpaceBBQueryFunc)(cpShape *shape, void *data)
//
//void cpSpaceBBQuery(
//	cpSpace *space, cpBB bb,
//	cpLayers layers, cpGroup group,
//	cpSpaceBBQueryFunc func, void *data
//)

//float trr,x0x,y0x,ts;
//int res;
//
//
//void calcV(cpShape *shape, void *datos){
//cpBody * cuerpo=shape->body;
//float v=0.0;
//    if ( cuerpo!=modChipmunk_cpEspacio->staticBody && ((DataPointer)cuerpo->data)->grupoEfector==1
//        ){
//        INSTANCE * i;
//        float a,b;
//        i=instance_get(((DataPointer)shape->body->data)->father);
//        a=LOCINT32(mod_chipmunk, i, LOC_X)-x0x;
//        b=LOCINT32(mod_chipmunk, i, LOC_Y)-y0x;
//        v+=ts/(float)((float)a*(float)a +(float)b*(float)b+0.0001);
//
//    }
//    res=v>trr;
//}

static inline float Max(float a,float b){
return a>b? a:b;
}

static inline float Min(float a,float b){
return a<b? a:b;
}

static inline int Metaball(int x,int y, int*xp,int*yp, int tam, float t, float tr)
{
    //printf("%d %d %d %p %d\n",params[0],params[1],params[2],params[3],params[4]); fflush(stdout);
    float v=0.0,a,b;
    int z;
    //INSTANCE * i;
    for (z=0;z<tam;z++){
//       i=instance_get(ids[z]);
//       a=LOCINT32(mod_chipmunk, i, LOC_X)-x;
//       b=LOCINT32(mod_chipmunk, i, LOC_Y)-y;
       a=xp[z]-x;
       b=yp[z]-y;
       v+=t/(float)((float)a*(float)a +(float)b*(float)b+0.0001);
    }
    //printf("%f\n",v); fflush(stdout);
    return v>tr;

}
//encontrar minx y miny para el pintado...
int modChipmunkPintaAgua(INSTANCE * my, int * params){
    GRAPH * map=bitmap_get( params[0], params[1] );
    int anc=map->width;
    int alt=map->height;
    int x4,y4;
    int *ids=params[3];
    int tam=params[4];
    int x[tam];
    int y[tam];
    int minx=anc, miny=alt, maxx=0,maxy=0;
    INSTANCE * i;

    int z;
    for (z=0;z<tam;z++){
       i=instance_get(ids[z]);
       x[z]=x4=LOCINT32(mod_chipmunk, i, LOC_X)-params[8];
       y[z]=y4=LOCINT32(mod_chipmunk, i, LOC_Y)-params[9];
       //printf("%d %d\n",params[8],params[9]);
       if (minx>x4)
        minx=x4;
       else if (maxx<x4)
        maxx=x4;

       if (miny>y4)
        miny=y4;
       else if (maxy<y4)
        maxy=y4;
    }

    if (tam==1){
        maxx=minx;
        maxy=miny;
    }

    int reaj=params[7];
    minx=Max(minx-reaj,0);
    miny=Max(miny-reaj,0);
    maxx=Min(maxx+reaj,anc);
    maxy=Min(maxy+reaj,alt);

    //int color=params[2];
    for (x4=minx;x4<maxx;x4++){
        for (y4=miny;y4<maxy;y4++){
             if ( Metaball(x4,y4,x,y,tam,*(float*)&params[5],*(float*)&params[6]))
            // *( uint32_t * ) (( uint8_t * ) map->data + map->pitch * y4 + ( x4 << 2 ))=color;
                gr_put_pixel(map,x4,y4,params[2]);
        }
    }
}


int modChipmunkMetaball(INSTANCE * my, int * params)
{
    //printf("%d %d %d %p %d\n",params[0],params[1],params[2],params[3],params[4]); fflush(stdout);
    float v=0.0,a,b,t;
    int z;
    int *ids=params[3];
    INSTANCE * i;
    t=(float)params[2];
    for (z=0;z<params[4];z++){
       i=instance_get(ids[z]);
       a=LOCINT32(mod_chipmunk, i, LOC_X)-params[0];
       b=LOCINT32(mod_chipmunk, i, LOC_Y)-params[1];
       v+=t/(float)((float)a*(float)a +(float)b*(float)b+0.0001);
    }
    //printf("%f\n",v); fflush(stdout);
    return v>(*(float*)&params[5]);
}

int modChipmunkSetEfector(INSTANCE * my, int * params){
    ((DataPointer)((cpShape *)params[0])->body->data)->grupoEfector=params[1];
}

//typedef struct WaterSs{
//  int*ids;
//  int tam;
//  float kNorm;
//  float kNearNorm;
//  float kRestDensity;
//  float kStiffness;
//  float kNearStiffness;
//  float kSurfaceTension;
//  float kLinearViscocity;
//  float kQuadraticViscocity;
//  float KH;
//}WaterS;

int modChipmunkAgua(int*ids,int tam, float kNorm, float kNearNorm,float kRestDensity,
                    float kStiffness,float kNearStiffness, float kDT, float kSurfaceTension,
                    float kLinearViscocity,float kQuadraticViscocity, float KH){
    int x[tam];
    int y[tam];
    float vx[tam];
    float vy[tam];
    INSTANCE *is;//,*in[tam];
    cpBody* bd[tam],*bdy;
    float mass=1;
    float densidad[tam];
    float nearDensidad[tam];
    float p[tam];
    float nearP[tam];
    int i,j;
    float kDT2=kDT*kDT;

    for (i=0;i<tam;i++){
       is=instance_get(ids[i]);
       //printf("%d\n",ids[i]);
       x[i]=LOCINT32(mod_chipmunk, is, LOC_X);
       y[i]=LOCINT32(mod_chipmunk, is, LOC_Y);
       bd[i]=bdy=(cpBody*)LOCINT32(mod_chipmunk, is, LOC_BODY);
       vx[i]=bdy->v.x;
       vy[i]=bdy->v.y;

    }

    float KH2=KH*KH;

    for (i=0;i<tam;i++){
        float density=0,nearDensity=0;
        for (j=0;j<tam;j++){
            if (i==j)
                continue;
            float dx,dy,r2,a,r;
            dx=x[j]-x[i];
            dy=y[j]-y[i];
            r2=dx*dx+dy*dy;
            if (r2>KH2)//no se...
                continue;
            r=sqrt(r2);
            a=1-r/KH;
            float a3m=a*a*a*mass;
            density+=a3m*kNorm;
            nearDensity+=a3m*a*kNearNorm;
        }
        densidad[i]=density;
        nearDensidad[i]=nearDensity;
        p[i]=kStiffness*(density-mass*kRestDensity);
        nearP[i]=kNearStiffness*nearDensity;
    }

    for (i=0;i<tam;i++){
        float x0=0,y0=0;
        for (j=0;j<tam;j++){
            if (i==j)
                continue;
            float dx,dy,r2,a,d,r;
            dx=x[j]-x[i];
            dy=y[j]-y[i];
            r2=dx*dx+dy*dy;
            if (r2>KH2)//no se...
                continue;
            r=sqrt(r2);
            a=1-r/KH;
            float a2=a*a;
            d=kDT2*((nearP[i]+nearP[j])* a2*a*kNearNorm +(p[i]+p[j])*a2*kNorm)/2;
            float rmd=d/(r*mass);
            x0-=rmd*dx;
            y0-=rmd*dy;

            //superficie
            rmd=kSurfaceTension*a2*kNorm;
            x0+=rmd*dx;
            y0+=rmd*dy;

            //viscocidad
            float du=vx[i]-vx[j];
            float dv=vy[i]-vy[j];
            float u=du*dx+dv*dy;

            if(u>0){
                u/=r;
                float It=0.5*kDT*a*(kLinearViscocity*u+kQuadraticViscocity*u*u)*kDT;
                x0-=It*dx;
                y0-=It*dy;
            }

        }
        cpBodySetVel(bd[i],cpv(vx[i]+(x0*kDT),vy[i]+(y0*kDT)) );
        cpBodyUpdateVelocity(bd[i],cpv(*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_X),*(float *)GLOADDR(mod_chipmunk,GLO_GRAVITY_Y)), *(float *)GLOADDR(mod_chipmunk,GLO_DAMPING),kDT);
//cpSpaceReindexShapesForBody(modChipmunk_cpEspacio,bd[i]);
        //LOCINT32(mod_chipmunk, in[i], LOC_X)+=x0;
       // LOCINT32(mod_chipmunk, in[i], LOC_Y)+=y0;

    }
}

int modChipmunkEmulateAgua(INSTANCE * my, int * params){
    float kdt=*(float *)GLOADDR(mod_chipmunk,GLO_INTERVAL)/(float)GLODWORD(mod_chipmunk,GLO_PHRESOLUTION);
    WaterS *ws=params[0];
    //printf("%f\n",kdt);fflush(stdout);
    if (ws->ids==0 || ws->size>1)
        return 0 ;
    modChipmunkAgua(ws->ids,ws->size,ws->kNorm,ws->kNearNorm,ws->kRestDensity,
                    ws->kStiffness,ws->kNearStiffness,kdt,ws->kSurfaceTension,
                    ws->kLinearViscocity,ws->kQuadraticViscocity,ws->KH);
    return 1;
}



