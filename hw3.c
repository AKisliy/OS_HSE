#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("You passed no arguments!!\n");
        return 0;
    }
    
    int a;
    if (sscanf(argv[1], "%d", &a) != 1){
        printf("Can't convert input to int!\n");
        return 0;
    }
    
    unsigned long long max = ULLONG_MAX;
    
    pid_t pid, ppid, chpid;
    chpid = fork();
    pid = getpid();
    ppid = getppid();
    // extra information about current process
    printf("Hello! I'm process with pid = %d, my parent is %d and my child is %d\n",(int)pid, (int)ppid, (int)chpid);
    
    if(chpid <= -1){
        printf("Something went wrong with fork!\n");
    } else if(chpid == 0){
        unsigned long long res = 1;
        unsigned long long cur = 1;

        while(cur <= a){
            if(max / cur < res){
                printf("Can't calculate full factorial, because of overflow.\n");
                printf("The last found value is %llu! = %llu\n", cur - 1,res);
                break;
            }
            res *= cur;
            ++cur;
        }
        if(cur > a){
            printf("%d! = %llu\n",a, res);
        }
        printf("\n");
    } else{
        if(a == 1){
            printf("Fibonacci_1 = 0\n");
        } else if(a == 2){
            printf("Fibonacci_2 = 1\n");
        } else{
            unsigned long long prevprev = 0;
            unsigned long long prev = 1;
            int index = 3;
            while(index <= a){
                unsigned long long temp = prev;
                if(max - prev < prevprev){
                    printf("Can't calculate Fibonacci, because of overflow.\n");
                    printf("The last found value is: Fibonacci_%d = %llu\n",index - 1, prev);
                    break;
                }
                prev = prev + prevprev;
                prevprev = temp;
                ++index;
            }
            if(index > a){
                printf("Fibonacci_%d = %llu\n", a, prev);
            }
        }
        printf("\n");
    }
    return 0;
}
