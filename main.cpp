#include <iostream>
#include <unistd.h>
#include <string.h>
using namespace std;

const int processNum = 5;
//const int processNum = 1;
const char* inputFile = "/Users/ppp123/Desktop/aggregation/aggregation/input.txt";

//aggregation
int main() {
    int pid = 0,numberNum = 0;
    
    FILE* fp = fopen(inputFile, "r+");
    while(1){
        int tmp;
        int eof = fscanf(fp, "%d", &tmp);
        numberNum++;
        if(eof == EOF) break;
    }
    fclose(fp);
    
    fp = fopen(inputFile, "r+");
    int numberPerFile = numberNum / processNum, tmpNum = 0, splitNum = 0;
    int initFlag = 1;
    
    FILE* splitedFile;
    while (1) {
        if(initFlag){
            char fileNameBuf[60] = {0, };
            sprintf(fileNameBuf, "%s", "spilted");
            sprintf(fileNameBuf + strlen(fileNameBuf), "_%d.txt", splitNum);
            splitedFile = fopen(fileNameBuf, "w");
            initFlag = 0;
        }
        
        int tmp = 0;
        int eof = fscanf(fp, "%d", &tmp);
        if(eof == EOF){
            //fprintf(splitedFile, "end\n");
            //printf("end\n");
            break;
        }
        fprintf(splitedFile, "%d\n", tmp);
        //printf("%d\n", tmp);
        
        tmpNum++;
        if(numberPerFile < tmpNum){
            tmpNum = 0;
            splitNum++;
            initFlag = 1;
            fclose(splitedFile);
            //printf("\n\n");
        }
    }
    fclose(splitedFile);
    fclose(fp);
    
    for(int i = 0; i < processNum; i++){
        pid = fork();
        splitNum++;
        if(pid != 0) break;
    }
    
    if(pid == 0){
        printf("hi I'm master\n");
        for(int i = 0; i < processNum; i++){
            int status;
            wait(&status);
        }
    }
    else{
        //children
        splitNum -= processNum;
        
        char fileNameBuf[60] = {0, };
        sprintf(fileNameBuf, "%s", "spilted");
        sprintf(fileNameBuf + strlen(fileNameBuf), "_%d.txt", splitNum);
        splitedFile = fopen(fileNameBuf, "r+");
        
        double sum = 0;
        while(1){
            int tmp = 0;
            int eof = fscanf(splitedFile, "%d", &tmp);
            
            sum += tmp;
            
            if(eof == EOF){
                printf("%lf\n", sum);
                break;
            }
        }
        fclose(splitedFile);
    }
    
    return 0;
}
