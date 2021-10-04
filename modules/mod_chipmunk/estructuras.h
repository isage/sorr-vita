#include "Arreglos.h"
#include "LL.h"

typedef struct DatosConstraint{
int fatherA;
int fatherB;
PROCDEF * funcion;
PROCDEF * funcionPre;
}DatosConstraint;

typedef struct cpsegmentqueryinfoB
{
    int shape,id;
    float x,y,t;
    cpVect n;
} cpsegmentqueryinfoB;



typedef struct handlersC
{
    modChipmunkStruct_Arreglo* arr;
    modChipmunkStruct_Arreglo * colisiones;
    cpCollisionType a, b;

} modChipmunkStruct_colHand;


modChipmunkStruct_Arreglo * HandlerColisions;



typedef struct cpContactPointSetM
{
    int count;
    int id1, id2;
    int shape1,shape2;
    struct
    {
        cpVect Point, normal;
        float dist;
    } points[CP_MAX_CONTACTS_PER_ARBITER];
} cpContactPointSetM;

struct modChipmunkStruct_Data_Pointer
{
    int father;
    int estado;
    struct modChipmunkStruct_Data_Pointer * sig;
    int x,y;
    cpBody * body;
    int angle;
  //  int nShapes;
   // cpShape ** Shapes;
   // int nConstraints;
  //  modChipmunkStruct_nodo * Constraints;
    int grupoEfector;       //se usa para reconocer lo elementos de agua y poder calcularlos...
//    cpBody ** constraintsBody;
    int typeShape;
}modChipmunkStruct_Data_Pointer;

typedef struct modChipmunkStruct_Data_Pointer
 *DataPointer;

typedef struct WaterSs{
  int*ids;
  int size;
  float kNorm;
  float kNearNorm;
  float kRestDensity;
  float kStiffness;
  float kNearStiffness;
  float kSurfaceTension;
  float kLinearViscocity;
  float kQuadraticViscocity;
  float KH;
}WaterS;
