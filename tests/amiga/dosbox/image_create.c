/* Host harness for the same formatter used by ZZDosCreate. */
#include "image_create.h"
static int cancel(unsigned p,const char*s,void*v){(void)s;(void)v;return p<10;}
int main(int argc,char**argv){FILE*out;const char*error;if(argc<5)return 2;out=fopen(argv[1],"wb");if(!out)return 2;error=di_create(out,strcmp(argv[2],"-")?argv[2]:NULL,strcmp(argv[3],"-")?argv[3]:NULL,atoi(argv[4]),argc>5?cancel:NULL,NULL);if(fclose(out)&&!error)error="close error";if(error){puts(error);return 1;}return 0;}
