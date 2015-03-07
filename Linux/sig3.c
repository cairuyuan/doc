#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
void sig_handler(int signum)
{
    printf("Receive signal. %d\n", signum);
}

void* sigmgr_thread()
{
    sigset_t   waitset, oset;
    int        sig;
    int        rc;
    pthread_t  ppid = pthread_self();

    pthread_detach(ppid);//使主线程免于因join而阻塞

    sigemptyset(&waitset);
    sigaddset(&waitset, SIGRTMIN);
    sigaddset(&waitset, SIGRTMIN+2);
    sigaddset(&waitset, SIGRTMAX);
    sigaddset(&waitset, SIGUSR1);
    sigaddset(&waitset, SIGUSR2);

    while (1)  
    {
        rc = sigwait(&waitset, &sig);
        if (rc != -1)
         {
         printf("Receive signal. %d\n", sig);
            //sig_handler(sig);
        }
        else 
        {
            printf("sigwaitinfo() returned err: %d;\n", errno);
        }
    }
}


int main()
{
    sigset_t bset, oset;
    int                          i;
    pid_t                  pid = getpid();
    pthread_t       ppid;

    sigemptyset(&bset);
    sigaddset(&bset, SIGRTMIN);
    sigaddset(&bset, SIGRTMIN+2);
    sigaddset(&bset, SIGRTMAX);
    sigaddset(&bset, SIGUSR1);
    sigaddset(&bset, SIGUSR2);
    
    pthread_sigmask(SIG_BLOCK, &bset, &oset) ;

    kill(pid, SIGRTMAX);
    kill(pid, SIGRTMAX);
    kill(pid, SIGRTMIN+2);
    kill(pid, SIGRTMIN);
    kill(pid, SIGRTMIN+2);
    kill(pid, SIGRTMIN);
    kill(pid, SIGUSR2);
    kill(pid, SIGUSR2);
    kill(pid, SIGUSR1);
    kill(pid, SIGUSR1);

    // Create the dedicated thread sigmgr_thread() which will handle signals synchronously
    pthread_create(&ppid, NULL, sigmgr_thread, NULL);
    sleep(100);
    return 0;
}
