#include <pthread.h>  
#include <stdio.h>  
#include <sys/signal.h>  
  

  /*
按照 POSIX, 异步 (外部) 信号发送到整个进程.
所有线程共享同一个设置, 即通过 sigaction 设置的线程处置方法.
每个线程有自己的信号掩码, 线程库根据该掩码决定将信号发送到哪个线程.
由于Linux 线程实现上的独特性, 外部信号始终发送到特定的线程.  
  */
#define NUMTHREADS 3  
void sighand(int signo)  
{   
    pthread_t             tid = pthread_self();  
    printf("Thread %lu in signal handler\n", tid);  
    return;  
}   
    
void *threadfunc(void *parm)  
{   
    //没有使用掩码屏蔽，会相应alam信号
    pthread_t             tid = pthread_self();  
    int                   rc;    
    printf("Thread %u entered\n", tid);  
    rc = sleep(30); /* 若有信号中断则返回剩余秒数 */  
    printf("Thread %u did not get expected results! rc=%d\n", tid, rc);  
    return NULL;  
}   
void *threadmasked(void *parm)  
{   
    //使用掩码屏蔽了所有的信号
    pthread_t             tid = pthread_self();  
    sigset_t              mask;  
    int                   rc;  
    printf("Masked thread %lu entered\n", tid);  
    sigfillset(&mask); /* 设置自己的信号集 */  
    
    rc = pthread_sigmask(SIG_BLOCK, &mask, NULL); 
    //使用pthread_sigmask函数来屏蔽某个线程对某些信号的响应处理

    if (rc != 0)  
    {  
        printf("%d, %s\n", rc, strerror(rc));  
        return NULL;  
    }  
    rc = sleep(15);  
    if (rc != 0)  
    {  
        printf("Masked thread %lu did not get expected results! rc=%d \n", tid, rc);  
        return NULL;  
    }  
    printf("Masked thread %lu completed masked work\n", tid);  
    return NULL;  
}   
    
int main(int argc, char **argv)  
{  
    int                     rc;  
    int                     i;  
    struct sigaction        actions;  
    pthread_t               threads[NUMTHREADS];  
    pthread_t               maskedthreads[NUMTHREADS];  
  
    printf("Enter Testcase - %s\n", argv[0]);  
    printf("Set up the alarm handler for the process\n");  
  
    memset(&actions, 0, sizeof(actions));  
    sigemptyset(&actions.sa_mask); /* 将参数set信号集初始化并清空 */  
    actions.sa_flags = 0;  //包含控制位，相当于开关
    actions.sa_handler = sighand;  
  
    /* 设置SIGALRM的处理函数 */  
    sigaction(SIGALRM,&actions,NULL);  //设置对应于第一个参数(信号)的处理函数
  
    printf("Create masked and unmasked threads\n");  
    for(i=0; i<NUMTHREADS; ++i)  
    {  
        pthread_create(&threads[i], NULL, threadfunc, NULL);  
        pthread_create(&maskedthreads[i], NULL, threadmasked, NULL);  

    }  
    
    sleep(3);  
    printf("Send a signal to every masked and unmasked thread\n");  
  
     /* 向线程发送SIGALRM信号 */  
    for(i=0; i<NUMTHREADS; ++i) {  
        pthread_kill(threads[i], SIGALRM);  
        pthread_kill(maskedthreads[i], SIGALRM);  
    }  
  
  /*如果使用kill发信息的话，信号必然是要发给进程，那么正在运行的线程会处理信号，无法控制
  如果这个线程正好没有注册该信号的处理函数，就会出现错误，导致整个进程被终止
  */
    printf("Wait for masked and unmasked threads to complete\n");  
    for(i=0; i<NUMTHREADS; ++i) {  
        pthread_join(threads[i], NULL);  
        pthread_join(maskedthreads[i], NULL);  
    }  
  
    printf("Main completed\n");  
    return 0;  
}  
