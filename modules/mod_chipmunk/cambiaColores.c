/*****************************************************/
/*         transormador de modos de color            */
/*         Prg,   05/09/2010 p.m.                    */
/*****************************************************/
#include "bgddl.h"
#include "bgdrtm.h"
#include "stdio.h"
#include "libdraw.h"
#include "mod_map.h"

#include "librender.h"

typedef struct Mic
{
    uint8_t rojo;
    uint8_t verde;
    uint8_t azul;

} mic ;

void miget_rgb(int color,int *rojo, int * verde,int *azul,int *alp)
{
    *azul=color&0xff;
    *verde=(color>>8)&0xff;
    *rojo=(color>>16)&0xff;
    *alp=(color>>24)&0xff;
}

int minc(PALETTE * pal2)
{
    mic colors[256];
    int totcolor;
    int interm;
    int vms;
    int z;
//PAL_GET(pal2,0, 256, &colors);
//pal_get(( PALETTE * )( params[0] ), params[1], params[2], ( uint8_t * )params[3] )
    pal_get(pal2, 0, 256, ( uint8_t * ) colors );
    totcolor=colors[1].rojo+colors[1].verde+colors[1].azul;
    for (z=2; z<256; z++)
        interm=colors[z].rojo+colors[z].verde+colors[z].azul;
    if (interm<totcolor && interm>0)
    {
        totcolor=interm;
        vms=z;
    }
    return(vms);
}




int convert_32_8(int file1,int graph1,PALETTE * pal2)
{
    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    uint32_t * buffer2;
    int rojo, azul, verde, alp,fnd;
    GRAPH  * temp_graph;
    int        anc,alt;
    int nc;
    anc=mapa1->width;
    alt=mapa1->height;
    temp_graph = bitmap_new_syslib( anc,alt,8 ) ;
    if (temp_graph==NULL)
        return(0);
    pal_map_assign( 0,temp_graph->code,(PALETTE *)pal2);

    buffer2=mapa1->data;
    nc=minc((PALETTE *) pal2);
    int z=-1;
    int y,x;
    for (y=0; y<alt; y++)
        for (x=0; x<anc; x++)
        {
            z++;
            miget_rgb(buffer2[z],&rojo,&verde,&azul,&alp);
            if (alp>30)
                if ((fnd=gr_find_nearest_color(rojo,verde,azul))==0 && rojo!=0 && verde!=0 &&azul!=0)
                    gr_put_pixel( temp_graph, x,y,nc ) ;
                else
                    gr_put_pixel( temp_graph,x,y,fnd);
            else
                gr_put_pixel( temp_graph, x,y,0 ) ;
        }
    return(temp_graph->code);
}


static int modconversor_32_8( INSTANCE * my, int * params )
{
    return convert_32_8(params[0],params[1],(PALETTE *) params[2]) ;
}

int convert_32_16(file1,graph1)
{
    uint32_t * buffer2;
    uint16_t * buffer;
    int rojo, azul, verde, alp;
    GRAPH *   temp_graph;
    int anc,alt;
    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    anc=mapa1->width;
    alt=mapa1->height;
    temp_graph = bitmap_new_syslib( anc,alt,16 ) ;
    if (temp_graph==NULL)
        return(0);
//temp_graph=new_map(anc%2<>0? anc+1 : anc,alt,16);
    buffer2=mapa1->data;
    buffer=temp_graph->data;
    int z=-1, x, y;
    for (y=0; y<alt; y++)
        for (x=0; x<anc; x++)
        {
            z++;
            miget_rgb( buffer2[z],&rojo,&verde,&azul,&alp ) ;
            if (alp>30)
            {
                if (rojo<16 && verde<16 && azul<16 && buffer2[z]!=0)
                    gr_put_pixel(temp_graph,x,y,2081);
                else
                {
                    azul=azul>>3;
                    verde=(verde>>2)<<5;
                    rojo=(rojo>>3)<<11;
                    gr_put_pixel(temp_graph,x,y,azul|verde|rojo);
                }
            }
            else
                gr_put_pixel(temp_graph,x,y,0);
        }
    return(temp_graph->code);
}

static int modconversor_32_16( INSTANCE * my, int * params )
{
    return convert_32_16(params[0],params[1]) ;
}


int convert_8_32(file1,graph1)
{

    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    int anc=mapa1->width;
    int     alt=mapa1->height;
    GRAPH * temp_graph = bitmap_new_syslib( anc,alt,32 ) ;
    if (temp_graph==NULL)
        return(0);
//map_put(0,temp_graph,temp_graph2,anc/2,alt/2);
    // gr_blit( temp_graph, 0, anc>>1, alt>>1, 0, mapa1 ) ;

    mic colors[256];
    pal_get(mapa1->format->palette, 0, 256, ( uint8_t * ) colors );

    int av1=(mapa1->pitch)-anc;
    uint8_t * buffer2=mapa1->data;
    uint32_t * buffer=temp_graph->data;
    int z=-1,z2=-1, x, y;
    int azul,verde,rojo,col;
    for (y=0; y<alt; y++)
    {
        for (x=0; x<anc; x++)
        {
            z++;
            z2++;
            col=buffer2[z];
            verde=colors[col].verde<<8;
            rojo=colors[col].rojo<<16;
            azul=colors[col].azul;
            buffer[z2]= azul|verde|rojo|0xFF000000;
        }
        z+=av1;

    }


    return(temp_graph->code);
}

static int modconversor_8_32( INSTANCE * my, int * params )
{
    return convert_8_32(params[0],params[1]) ;
}


int convert_8_16(file1,graph1)
{
    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    int anc=mapa1->width;
    int alt=mapa1->height;
    GRAPH * temp_graph = bitmap_new_syslib( anc,alt,16) ;
    if (temp_graph==NULL)
        return(0);

    mic colors[256];
    pal_get(mapa1->format->palette, 0, 256, ( uint8_t * ) colors );

    int av1=(mapa1->pitch)-anc,av2=(temp_graph->pitch/2)-anc;
    uint8_t * buffer2=mapa1->data;
    uint16_t * buffer=temp_graph->data;
    int z=-1,z2=-1, x, y;
    int azul,verde,rojo;
    for (y=0; y<alt; y++)
    {
        for (x=0; x<anc; x++)
        {
            z++;
            z2++;
            verde=colors[buffer2[z]].verde;
            rojo=colors[buffer2[z]].rojo;
            azul=colors[buffer2[z]].azul>>3;
            verde=(verde>>2)<<5;
            rojo=(rojo>>3)<<11;
            buffer[z2]=(uint16_t) (azul|rojo|verde);
        }
        z+=av1;
        z2+=av2;
    }
    return(temp_graph->code);
}

static int modconversor_8_16( INSTANCE * my, int * params )
{
    return convert_8_16(params[0],params[1]) ;
}


int convert_16_32(file1,graph1)
{
    uint16_t * buffer2;
    uint32_t * buffer;
    uint32_t rojo, azul, verde;
    GRAPH *   temp_graph;
    int anc,alt;
    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    anc=mapa1->width;
    alt=mapa1->height;
    temp_graph = bitmap_new_syslib( anc,alt,32 ) ;
    if (temp_graph==NULL)
        return(0);
    buffer2=mapa1->data;
    buffer=temp_graph->data;
    int z=-1, x, y,av2=(mapa1->pitch/2)-anc,z2=-1;
    uint32_t color;
    for (y=0; y<alt; y++)
    {
        for (x=0; x<anc; x++)
        {
            z++;
            z2++;
            color=buffer2[z];
            azul=(color&31)<<3;
            verde=(color&2016)<<5;
            rojo=(color&63488)<<8;
            buffer[z2]= azul|verde|rojo|0xFF000000;
            //buffer[z2]= gr_rgba(rojo,verde,azul,255);
        }
        z+=av2;
    }
    return(temp_graph->code);
}

static int modconversor_16_32( INSTANCE * my, int * params )
{
    return convert_16_32(params[0],params[1]) ;
}

int convert_16_8(int file1,int graph1,PALETTE * pal2)
{
    uint16_t * buffer2;
    uint8_t * buffer;
    uint32_t rojo, azul, verde;
    GRAPH *   temp_graph;
    int anc,alt;
    GRAPH * mapa1=bitmap_get( file1, graph1 );
    if (mapa1==NULL)
        return(0);
    anc=mapa1->width;
    alt=mapa1->height;
    temp_graph = bitmap_new_syslib( anc,alt,8 ) ;
    if (temp_graph==NULL)
        return(0);
    pal_map_assign( 0,temp_graph->code,(PALETTE *)pal2);
    int nc=minc((PALETTE *) pal2);

    buffer2=mapa1->data;
    buffer=temp_graph->data;
    int z=-1, x, y,av2=(mapa1->pitch/2)-anc,z2=-1,av=(temp_graph->pitch)-anc;
    uint32_t color;
    uint8_t fnd;
    for (y=0; y<alt; y++)
    {
        for (x=0; x<anc; x++)
        {
            z++;
            z2++;
            color=buffer2[z];
            azul=(color&31)<<3;
            verde=(color>>3)&252;
            rojo=(color>>8);
            if ((fnd=gr_find_nearest_color(rojo,verde,azul))==0 && color!=0)
                buffer [z2]=nc;
            else
                gr_put_pixel( temp_graph,x,y,fnd);//buffer [z2]=fnd;
        }
        z+=av2;
        z2+=av;
    }
    return(temp_graph->code);
}

static int modconversor_16_8( INSTANCE * my, int * params )
{
    return convert_16_8(params[0],params[1],(PALETTE *)params[2]) ;
}


///*function convert_8_8(file1,graph1,file2,graph2,pal2);
//private
//
//        temp_graph;
//        anc,alt;
//        mic colors[256];
//        paleta[256];
//        byte pointer buffer;
//        pal1;
//begin
//anc=graphic_info (file1, graph1, g_wide );
//alt=graphic_info (file1, graph1, G_HEIGHT   );
//temp_graph=new_map(anc,alt,8);
//pal1=PAL_MAP_GETID(file1, graph1);
//pal_map_assign(0,temp_graph,pal2);
//map_put(0,temp_graph,graph1,anc/2,alt/2);
//PAL_GET(pal1,0, 256, &colors);
//from z=0 to 255;
//paleta[z]=find(colors[z][0],colors[z][1],colors[z][2]);
//end
//convert_palette(0,temp_graph,&paleta);
//
//fpg_add(file2,graph2,file1,temp_graph);
//unload_map(0,temp_graph);
////unload_pal(pal1);
//end
//  */
int convert_8_8(int file1,int graph1,PALETTE *pal2)
{

    GRAPH * temp_graph;

    mic colors[256];
    uint8_t paleta[256];
    PALETTE * pal1;

    int anc,alt;
    GRAPH * map=bitmap_get( file1, graph1 );
    if (map==NULL)
        return(0);
    anc=map->width;
    alt=map->height;
    temp_graph = bitmap_new_syslib( anc,alt,8 ) ;
    pal1=map->format->palette;
    pal_map_assign( 0,temp_graph->code,pal2);

    pal_get(pal1, 0, 256, ( uint8_t * ) colors );
    int z,y,x;
    for(z=0; z<255; z++)
        paleta[z]=gr_find_nearest_color(colors[z].rojo,colors[z].verde,colors[z].azul);



//convert_palette(0,temp_graph,&paleta);

    uint8_t * ptr,* sek, * orig = ( uint8_t * )map->data,* destin = ( uint8_t * )temp_graph->data ;
    for ( y = 0 ; y < alt; y++ )
    {
        ptr = orig + map->pitch * y ;
        sek = destin + map->pitch * y ;
        for ( x = 0 ; x < anc ; x++, ptr++,sek++ )
        {
            *sek = paleta[*ptr] ;

        }
    }
    return(temp_graph->code);

}

static int modconversor_campal( INSTANCE * my, int * params )
{
    return convert_8_8(params[0],params[1],(PALETTE *)params[2]) ;
}

int modconversor_8(INSTANCE * my, int * params )
{
    GRAPH * map=bitmap_get( params[0],params[1] );
    switch (map->format->depth)
    {
    case 8:
        return(convert_8_8(params[0],params[1],(PALETTE *)params[2]) );
        break;
    case 16:
        return(convert_16_8(params[0],params[1],(PALETTE *)params[2]) );
        break;
    case 32:
        return(convert_32_8(params[0],params[1],(PALETTE *)params[2]) );
        break;
    }
    return(0);
}

int modconversor_16(INSTANCE * my, int * params )
{
    GRAPH * map=bitmap_get( params[0],params[1] );
    switch (map->format->depth)
    {
    case 8:
        return(convert_8_16(params[0],params[1]));
        break;
    case 32:
        return(convert_32_16(params[0],params[1]));
        break;
    }
    return(0);
}



int modconversor_32(INSTANCE * my, int * params )
{
    GRAPH * map=bitmap_get( params[0],params[1] );
    switch (map->format->depth)
    {
    case 8:
        return(convert_8_32(params[0],params[1]));
        break;
    case 16:
        return(convert_16_32(params[0],params[1]));
        break;
    }
    return(0);
}


DLSYSFUNCS __bgdexport( mod_conversor, functions_exports )[] =
{
//{ "NOMBRE_FUNCION" , "" , TYPE_INT , modnombre_funcion },
    { "CONV_32_8", "III" , TYPE_INT , modconversor_32_8},
    { "CONV_32_16", "II" , TYPE_INT , modconversor_32_16},
    { "CONV_8_32", "II" , TYPE_INT , modconversor_8_32},
    { "CONV_8_16", "II" , TYPE_INT , modconversor_8_16},
    { "CONV_16_32", "II" , TYPE_INT , modconversor_16_32},
    {"CONV_16_8", "III" , TYPE_INT , modconversor_16_8},
    {"CAM_PAL", "III" , TYPE_INT , modconversor_campal},
    {"CONV_8", "III" , TYPE_INT , modconversor_8},
    {"CONV_16", "II" , TYPE_INT , modconversor_16},
    {"CONV_32", "II" , TYPE_INT , modconversor_32},
    { 0 , 0 , 0 , 0 }
};
