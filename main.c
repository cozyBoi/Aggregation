#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

const int processNum = 16;
//const int processNum = 1;
const char* inputFile = "input.txt";


enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

//aggregation
int main() {
    int pid = 0,numberNum = 0, i;
    
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
    while (splitNum < processNum) {
        if(initFlag){
            char fileNameBuf[60] = {0, };
            sprintf(fileNameBuf, "%s", "spilted");
            sprintf(fileNameBuf + strlen(fileNameBuf), "_%d.txt", splitNum);
            splitedFile = fopen(fileNameBuf, "w");
            initFlag = 0;
            if(access(fileNameBuf, F_OK)){
                splitNum++;
                initFlag = 1;
                fclose(splitedFile);
                continue;
            }
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
    
    for(i = 0; i < processNum; i++){
        pid = fork();
        splitNum++;
        if(pid != 0) break;
    }
    
    if(pid == 0){
        for(i = 0; i < processNum; i++){
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
        
        char outNameBuf[60] = {0, };
        sprintf(outNameBuf, "%s", "out");
        sprintf(outNameBuf + strlen(outNameBuf), "_%d.txt", splitNum);
        FILE*outFile = fopen(outNameBuf, "a");
        
        int testcase = 5, t = 0;
        for(t = 0; t < testcase; t++){
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
            //한번 돌음.
            
            //go to start point of file
            fseek(splitedFile, 0, SEEK_SET);
            
            //measure time
            struct timespec start, finish, delta;
            clock_gettime(CLOCK_REALTIME, &start);
            
            sum = 0;
            while(1){
                int tmp = 0;
                int eof = fscanf(splitedFile, "%d", &tmp);
                
                sum += tmp;
                
                if(eof == EOF){
                    printf("%lf\n", sum);
                    break;
                }
            }
            
            clock_gettime(CLOCK_REALTIME, &finish);
            sub_timespec(start, finish, &delta);
            //time measure end
            
            fclose(splitedFile);
            fprintf(outFile, "child [PID: %d] process finished\n", getpid());
            fprintf(outFile, "latency : %ld.%.9ld\n", delta.tv_sec, delta.tv_nsec);
        }
        fclose(outFile);
    }
    
    return 0;
}
