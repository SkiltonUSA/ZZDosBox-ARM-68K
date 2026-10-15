/* Accuracy against double oracle plus guards/tails; no host timing claims. */
#include "hlsl_kernels.h"
#include "kernels.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstring>
static unsigned rng=7803;
static float random_float(){rng=rng*1664525+1013904223;return (int(rng>>8)-8388608)/4194304.0f;}
static double largest;
static void close(float got,double want,double scale){
    double error=std::fabs(got-want)/std::fmax(1.,scale);
    assert(std::isfinite(got)&&error<2e-6);if(error>largest)largest=error;
}
int main(){
    const zz_mat_fn mf[]={zz_mat_scalar,zz_mat_neon,zz_mat_hlsl};
    const zz_vec_fn vf[]={zz_vec_scalar,zz_vec_neon,zz_vec_hlsl};
    for(unsigned t=0;t<1000;t++){
        float a[16],b[16];for(unsigned j=0;j<16;j++){a[j]=random_float();b[j]=random_float();}
        float saved_a[16],saved_b[16];memcpy(saved_a,a,sizeof a);memcpy(saved_b,b,sizeof b);
        for(auto fn:mf){
            float out[18];for(auto &x:out)x=12345;
            fn(a,b,out+1);assert(out[0]==12345&&out[17]==12345);
            for(unsigned r=0;r<4;r++)for(unsigned c=0;c<4;c++){
                double sum=0,scale=0;for(unsigned k=0;k<4;k++){double v=double(a[r*4+k])*b[k*4+c];sum+=v;scale+=std::fabs(v);}
                close(out[1+r*4+c],sum,scale);
            }
        }
        assert(!memcmp(a,saved_a,sizeof a)&&!memcmp(b,saved_b,sizeof b));
    }
    for(unsigned n: {0u,1u,2u,3u,4u,5u,7u,8u,9u,15u,16u,17u,31u,32u,33u,127u,256u,1023u,1024u,1025u,3072u,4096u}){
        float a[16];for(auto &x:a)x=random_float();std::vector<float> v(n*4),out(n*4+2,12345);
        for(auto &x:v)x=random_float();auto saved=v;
        for(auto fn:vf){
            if(n==0)fn(nullptr,nullptr,nullptr,0);else fn(a,v.data(),out.data()+1,n);
            assert(out.front()==12345&&out.back()==12345&&v==saved);
            for(unsigned i=0;i<n;i++)for(unsigned r=0;r<4;r++){
                double sum=0,scale=0;for(unsigned k=0;k<4;k++){double x=double(a[r*4+k])*v[i*4+k];sum+=x;scale+=std::fabs(x);}
                close(out[1+i*4+r],sum,scale);
            }
        }
    }
    /* A genuine 3-element source catches accidental XYZ+W overreads in ASan. */
    float columns[16];for(auto &x:columns)x=random_float();
    float *xyz=new float[3];for(unsigned i=0;i<3;i++)xyz[i]=random_float();float out[4];
    zz_vertex_hlsl(columns,xyz,out);
    for(unsigned r=0;r<4;r++){
        double sum=columns[12+r],scale=std::fabs(sum);
        for(unsigned k=0;k<3;k++){double x=double(columns[k*4+r])*xyz[k];sum+=x;scale+=std::fabs(x);}
        close(out[r],sum,scale);
    }
    delete[] xyz;
    printf("PASS hlsl++/NEON/scalar: 1000 matrix cases; 22 vector sizes, guards, immutable inputs, XYZ bounds; max scaled error %.9g\n",largest);
}
