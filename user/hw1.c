//10x10 matmult using fork and pipe
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define N 10
#define A {{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9}}

#define B {{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,7,8,9}}

void dispmatrix(int mat[N][N]);

int main(int argc, char *argv[]){
    int matrixa[N][N]=A;
    int matrixb[N][N]=B;
    int result[N][N]={0};
    printf("Matrix A=B: \n");
    dispmatrix(matrixa);
    for (int i=0;i<N;i++){
        int p[2];
        pipe(p);
        int ps[N]={0}; //partial sum   
        if(fork()==0){
            close(p[0]);
            for (int j=0;j<N;j++){
                for(int k=0;k<N;k++){
                    ps[j]+=matrixa[i][k]*matrixb[k][j];
                }
            }
            write(p[1],ps,N*sizeof(int));
            close(p[1]);
            exit(0);}
        else{
            close(p[1]);
            wait((int*)0);
            read(p[0],ps,N* sizeof(int));
            close(p[0]);
            for(int m=0;m<N;m++){
                result[i][m]=ps[m];
            }
        }
        
    }
    printf("Result: \n");
    dispmatrix(result);
    return 0;
};

void dispmatrix(int mat[N][N]){
    for(int i = 0; i<N; i++){
        for(int j = 0; j<N; j++){
        printf("%d ", mat[i][j]);
    }
    printf("\n");
}
}


