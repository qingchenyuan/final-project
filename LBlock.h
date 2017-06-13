#include <iostream>

using namespace std;

unsigned short S[10][16]={{14,9,15,0,13,4,10,11,1,2,8,3,7,6,12,5},{4,11,14,9,15,13,0,10,7,12,5,6,2,8,1,3},{1,14,7,12,15,13,0,6,11,5,9,3,2,4,8,10},
                                {7,6,8,11,0,15,3,14,9,10,12,13,5,2,4,1},{14,5,15,0,7,2,12,13,1,8,4,9,11,10,6,3},{2,13,11,12,15,14,0,9,7,10,6,3,1,8,4,5},
                                {11,9,4,14,0,15,10,13,6,12,5,7,3,8,1,2},{13,10,15,0,14,4,9,11,2,1,8,3,7,5,12,6},{8,7,14,5,15,13,0,6,11,12,9,10,2,4,1,3},
                                {11,5,15,0,7,2,9,13,4,8,1,12,14,10,3,6}};


unsigned short P[8]={1,3,0,2,5,7,4,6};

const unsigned int rounds=32;
void roundkey(unsigned int k[5],unsigned int rk[rounds] ){
        unsigned int i,t1,t2,t3,t4,t0,c1,c2;
        rk[0]=(k[0]<<16)^ k[1];
        for(i=1;i<rounds;i++){
                // 32 left shift (then 3 right shift)
                t1=k[0]; t2=k[1];
                k[0]=k[2];  k[1]=k[3]; k[2]=k[4]; k[3]=t1; k[4]=t2;
                //3 right shift
                t0=(k[0]&0x7);              t1=(k[1]&0x7);              t2=(k[2]&0x7);
                t3=(k[3]&0x7);              t4=(k[4]&0x7);
                k[4]=(k[4]>>3)^(t3<<13);
                k[3]=(k[3]>>3)^(t2<<13);
                k[2]=(k[2]>>3)^(t1<<13);
                k[1]=(k[1]>>3)^(t0<<13);
                k[0]=(k[0]>>3)^(t4<<13);
                //s-box
                t1=(k[0]>>12)&0xF;
                t2=(k[0]>>8)&0xF;
                t1=S[9][t1];
                t2=S[8][t2];
                k[0]=(t1<<12)^(t2<<8)^(k[0]&0x00FF);
                //counter
                c1=i&0x3; c2=i>>2;
                k[2]^=c1<<14; k[1]^=c2;
                //get roundkey
                rk[i]=(k[0]<<16)^ k[1];
        }
}

unsigned int S_Layer(unsigned int x){
        unsigned int temp=0x0;
        int i;
        for(i=0;i<7;i++){
                temp^=S[7-i][(x>>(28-4*i))&0xF];
                temp<<=4;
        }                               
        temp^=S[7-i][x&0xF];
        return temp;
}

unsigned int P_Layer(unsigned int x){
        unsigned short temp[8],i;
        unsigned int t=0x0;
       
        for(i=0;i<8;i++)
                temp[i]=(x>>(28-(4*i)))&0xF;
       
        for(i=0;i<7;i++){
                t^=temp[P[i]];
                t<<=4;
        }
        t^=temp[P[i]];
       
        return t;
}

unsigned int F(unsigned int x, unsigned int k){
        x^=k;
        x=S_Layer(x);
        x=P_Layer(x);
        return x;
}
void swap(unsigned int *left, unsigned int *right){
        unsigned int temp;
        temp=(*left);
        (*left)=(*right);
        (*right)=temp;
}
 
int main(int argc, char **argv)
{
        unsigned int i;
        long long ctext,ptext=0x0;//0x0123456789abcdef;
        unsigned int left, right;
        left=ptext>>32;
        right=ptext&0xFFFFFFFF;
       
        unsigned int k1[5]={0x0,0x0,0x0,0x0,0x0000};//0x0123,0x4567,0x89ab,0xcdef,0xfedc
        unsigned int rk1[rounds];
        roundkey(k1,rk1);
               
        for(i=0;i<rounds;i++){
                right=(right<<8)^(right>>24);       
                right^=F(left,rk1[i]);
                swap(&left,&right);
        }
       
        ctext=right;
        ctext<<=32;
        ctext^=left;
       
        cout<<(hex)<<ctext<<endl;
       
        return 0;
}