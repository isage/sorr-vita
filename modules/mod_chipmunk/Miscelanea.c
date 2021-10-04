#include "Miscelanea.h"
#include "constantes.h"
void modChipmunk_mayusStr( char *str ){
    int i;
    for( i=0; str[i]!='\0'; i++ ){
        if( str[i]>='a' && str[i]<='z' )
            str[i]=str[i]-32; // change to uppercase
    }
}

float modChipmunkdeg2rad(int ang)
{
    return (ang/180000.0f)*CM_PI;
}

float modChipmunkrad2deg(float ang)
{
    return (float)(ang*180000.0f)/CM_PI;
}

int modChipmunkCompara(void * a, void * b){
    return a==b;
}
