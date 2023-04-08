#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEMCARD_BYTES 0x8ef;
//#define A2_CONST = 0x95ed30;

FILE *srLog;
FILE *srSave;


uint32_t a0,a1,a2,a3;

uint32_t t0,t1,t2,t3,t4;

uint32_t v0,v1;

uint32_t randomSeed[0x100];

char *buf;



uint32_t srl(uint32_t n, int shft) {

    int dshft = (~( (1<<31) >> (shft > 1 ? shft-1 : 0 ) ));
    return (n>>shft) & dshft;
}

uint32_t sll(uint32_t n, int shft) {
    return n<<shft;
}

uint32_t nor(uint32_t n, uint32_t n1) {

    return ~(n|n1);
}

uint32_t le(uint32_t b) {
    uint8_t b0 = b&0xff;
    uint8_t b1 = (b>>8) &0xff;
    uint8_t b2 = (b>>16) &0xff;
    uint8_t b3 = (b>>24) & 0xff;
    return (b0<< 24 ) | ((b1&0xff) <<16) | ((b2&0xff) <<8) | (b3)&0xff;
}


void debug() {

   fprintf(srLog,"\na0: %x a1: %x a2: %x a3: %x\nt0: %x t1: %x t2: %x t3: %x t4: %x\nv0: %x v1: %x",a0,a1,a2,a3,t0,t1,t2,t3,t4,v0,v1);
}


void randomSeedGenerator() {

    v0 = 0;
    v1 = 0x4c10000;
    a0 = 0x20;
    a1=0;
    a2=0x4c11db7;
    a3=1<<31;
    t0=8;
    t1=1;

    t3=0;
    t4=0;

    int rCount = 0;


    while(v1!=0) {

        t4 = v0;
        a1 = 0;
        t3 = t1;


         do{
            v1 = t4 & 1;





            if(v1!=0) {
                v1 = (unsigned int)(t0-t3);
                v1 = sll(t1,v1);

                a1 = a1|v1;

            }


            t3++;
            v1 = (t3 < 9) ? 1 :0;


        t4 = t4>>1;

        }while(v1!=0);
           
           
        t4 = sll(a1,0x18);
        t3 = t0;



       //int j = 0;

         do{


            v1 = t4 & a3;

          // debug();if(++j==2&&i>=1)exit(0);

            a1 = 0;
            if(v1!=0) 
                a1 = a2;

            
            v1 = sll(t4,1);
            t3--;      
            t4 = v1^a1;

        }while(t3!=0);

        a1 = 0;
        t3=t1;

        do{

            v1 = t4&0x0001;

            if(v1!=0) {
                v1 = (a0-t3);
                v1 = sll(t1,v1);
                a1 |=v1;

            }
            t3++;
            v1 = (t3<0x21) ? 1: 0 ;
            t4 = t4>>1;

        }while(v1!=0);

        v0++; 
        v1 = v0 < 0x100 ? 1 : 0;


        randomSeed[rCount++] = a1;

        
    };

printf("\n%d randoms generated.\n",rCount);



}


int main(int argc, char *argv[]) {

    randomSeedGenerator();

    int srSaveBytes = 0;
    int fptr = 0x8;

    srLog = fopen("log.txt","w");

    if(argc>0)
        srSave = fopen(argv[1],"rb");
    else{
        printf("usage: srchk filename");
        exit(0);
    }

   // fseek(srSave, 0L, SEEK_END);
   // memcardBytes = ftell(srSave)-fptr;
    //fseek(srSave, 0L, SEEK_SET);


    fseek(srSave,fptr,0); //calculation begins at 0x8


//initial conditions
    a0 = -1;
    a1 = 0;
    a3 = 0;
    v1 = 0;
    t0 = MEMCARD_BYTES;


    do {

    a1= fgetc(srSave)&0xff;

    a3 = a0 & 0x00ff;

    v1 = t0;

    a0 =srl(a0,0x08);

    t0--;

    a1^=a3;

    a1 = sll(a1,2);

    a1 = randomSeed[a1/4];
    
    a0^=a1; 
        
    srSaveBytes++;

    debug();

    }while(v1!=0);


    printf("Read %x bytes.\n",srSaveBytes);
            



  v1 = nor(a0,0);

printf("\nNew checksum is (little endian):\n");

printf("\n%x\n\n",le(v1));

fclose(srSave);
fclose(srLog);

}
