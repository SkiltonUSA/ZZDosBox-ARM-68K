/* Diagnostic-only wrapper: expose startup progress before any board access.
 * Shared launcher and production applications retain their original buffering. */
#undef main
#include <stdio.h>
int grx_launcher_main(int argc,char **argv);
int main(int argc,char **argv) {
 if(setvbuf(stdout,NULL,_IONBF,0))return 20;
 puts("GLIDE LAUNCH begin (unbuffered diagnostic)");
 return grx_launcher_main(argc,argv);
}
