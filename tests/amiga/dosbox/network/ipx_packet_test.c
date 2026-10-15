#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "ipx_packet.h"
int main(void){unsigned char p[1425]={255,255,0,30};
 assert(zz_ipx_valid(p,30));assert(!zz_ipx_valid(p,29));assert(!zz_ipx_valid(p,31));assert(!zz_ipx_valid(NULL,30));
 p[2]=5;p[3]=144;assert(zz_ipx_valid(p,1424));assert(!zz_ipx_valid(p,1425));p[0]=0;assert(!zz_ipx_valid(p,1424));
 memset(p,0,sizeof(p));p[0]=p[1]=255;p[3]=30;p[17]=p[29]=2;p[10]=10;p[15]=1;
 assert(zz_ipx_registration(p,30));p[15]=0;assert(!zz_ipx_registration(p,30));p[15]=1;p[10]=0;assert(!zz_ipx_registration(p,30));
 puts("PASS IPX length/checksum bounds and registration envelope");
}
