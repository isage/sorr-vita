#include "convexHull.h"
#include "bgddl.h"
#include "bgdrtm.h"
#include "stdio.h"
#include "libdraw.h"
#include "mod_map.h"
#include "librender.h"
#include "dlvaracc.h"

#include "globales.h"

//http://softsurfer.com/Archive/algorithm_0109/algorithm_0109.htm


// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.



// Assume that a class is already given for the object:
//    modChipmunkStruct_Point with coordinates {float x, y;}
//===================================================================

// isLeft(): tests if a modChipmunkStruct_Point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles

//===================================================================

static inline float isLeft( modChipmunkStruct_Point P0, modChipmunkStruct_Point P1, modChipmunkStruct_Point P2 )
{
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}


// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  P[] = an array of 2D points
//                   presorted by increasing x- and y-coordinates
//             n = the number of points in P[]
//     Output: H[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in H[]
int
chainHull_2D( modChipmunkStruct_Point* P, int n, modChipmunkStruct_Point* H )
{
    // the output array H[] will be used as the stack
    int    bot=0, top=(-1);  // indices for bottom and top of the stack
    int    i;                // array scan index

    // Get the indices of points with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    float xmin = P[0].x;
    for (i=1; i<n; i++)
        if (P[i].x != xmin) break;
    minmax = i-1;
    if (minmax == n-1) {       // degenerate case: all x-coords == xmin
        H[++top] = P[minmin];
        if (P[minmax].y != P[minmin].y) // a nontrivial segment
            H[++top] = P[minmax];
        H[++top] = P[minmin];           // add polygon endpoint
        return top+1;
    }

    // Get the indices of points with max x-coord and min|max y-coord
    int maxmin, maxmax = n-1;
    float xmax = P[n-1].x;
    for (i=n-2; i>=0; i--)
        if (P[i].x != xmax) break;
    maxmin = i+1;

    // Compute the lower hull on the stack H
    H[++top] = P[minmin];      // push minmin modChipmunkStruct_Point onto stack
    i = minmax;
    while (++i <= maxmin)
    {
        // the lower line joins P[minmin] with P[maxmin]
        if (isLeft( P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
            continue;          // ignore P[i] above or on the lower line

        while (top > 0)        // there are at least 2 points on the stack
        {
            // test if P[i] is left of the line at the stack top
            if (isLeft( H[top-1], H[top], P[i]) > 0)
                break;         // P[i] is a new hull vertex
            else
                top--;         // pop top modChipmunkStruct_Point off stack
        }
        H[++top] = P[i];       // push P[i] onto stack
    }

    // Next, compute the upper hull on the stack H above the bottom hull
    if (maxmax != maxmin)      // if distinct xmax points
        H[++top] = P[maxmax];  // push maxmax modChipmunkStruct_Point onto stack
    bot = top;                 // the bottom modChipmunkStruct_Point of the upper hull stack
    i = maxmin;
    while (--i >= minmax)
    {
        // the upper line joins P[maxmax] with P[minmax]
        if (isLeft( P[maxmax], P[minmax], P[i]) >= 0 && i > minmax)
            continue;          // ignore P[i] below or on the upper line

        while (top > bot)    // at least 2 points on the upper stack
        {
            // test if P[i] is left of the line at the stack top
            if (isLeft( H[top-1], H[top], P[i]) > 0)
                break;         // P[i] is a new hull vertex
            else
                top--;         // pop top modChipmunkStruct_Point off stack
        }
        H[++top] = P[i];       // push P[i] onto stack
    }
    if (minmax != minmin)
        H[++top] = P[minmin];  // push joining endpoint onto stack

    return top+1;
}
//&& (abs(anty##d + y )+abs(antx##d + x ))<pr
#define encuentraBorde(d) if (gr_get_pixel( mapa1, x,  y )!=0 && gr_get_pixel( mapa1, x,  y )!=0x00ffffff && gr_get_pixel( mapa1, x,  y )!=0xff000000){\
                b=1;\
                p[cantP].x=x;\
                p[cantP].y=y;\
                cantP++;\
                anty##d=y;\
                antx##d=x;\
                break;\
            }
static int mod_chipmunkMayor(modChipmunkStruct_Point p1,modChipmunkStruct_Point p2){
    if ((abs(p1.x)+abs(p1.y))>(abs(p2.x)+abs(p2.y)))
        return 1;
    return 0;
}

int modChipmunkBorde(GRAPH * mapa1,modChipmunkStruct_Point * p)
{
//    printf("ee %p\n",r);
    if (mapa1==NULL)
        return(0);
    int anc,alt;
    anc=mapa1->width;
    alt=mapa1->height;
   // int pr=(GLODWORD(mod_chipmunk,GLO_DISTPERCENT)*(anc+alt)/2) /100;
    int cantP=0;
    int antx1=-1000,antx2=-1000, anty1=-1000, anty2=-1000;
    int x,y;
    for (y=0; y<alt; y++){
    int b=0;
        for (x=0; x<anc; x++)
        {
            encuentraBorde(1)
        }
        if (b)
        for (x=anc-1; x>-1; x--)
        {
            encuentraBorde(2)
        }
    }
    for (x=0; x<anc; x++){
    int b=0;
        for (y=0; y<alt; y++)
        {
            encuentraBorde(1)
        }
        if (b)
        for (y=alt-1; y>-1; y--)
        {
            encuentraBorde(2)
        }
    }
    for (x=0;x<cantP;x++){              //se eliminan repetidos
        for (y=x+1;y<cantP;y++){
            if (p[y].x==p[x].x && p[y].y==p[x].y){
                cantP--;
                p[y].x=p[cantP].x;
                p[y].y=p[cantP].y;

            }
        }
    }

//    for (x=0;x<cantP;x++){              //se ordenan
//        for (y=x+1;y<cantP;y++){
//            if (mod_chipmunkMayor(p[x],p[y])){
//                modChipmunkStruct_Point aux;
//                aux.x=p[x].x;
//                aux.y=p[x].y;
//                p[x].x=p[y].x;
//                p[x].y=p[y].y;
//                p[y].x=aux.x;
//                p[y].y=aux.y;
//            }
//        }
//    }

    return cantP;
}


/*
void main(){
modChipmunkStruct_Point p[]={18,51, 36,113, 39,147, 69,75, 73,145, 113,47, 109,142, 140,32, 151,70, 144,118};
modChipmunkStruct_Point h[10];
int cant=chainHull_2D(&p,10,&h);
int z;
for (z=0;z<cant;z++)
    printf("%f %f \n",h[z].x,h[z].y);
    system("pause");
}
*/
