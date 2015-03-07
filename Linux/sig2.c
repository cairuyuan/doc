#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* Simple error handling functions */

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void * sig_thread(void *arg)
{
      sigset_t *set = (sigset_t *) arg;
      int s, sig;
      while(1) {
            sigwait(set, &sig);//同步处理信号事件
            printf(" It got signal %d\n", sig);
      }
}

int main(int argc, char *argv[])
{
      pthread_t thread;
      sigset_t set;
      int s;
      sigemptyset(&set);                //清空
      sigaddset(&set, SIGQUIT);
      sigaddset(&set, SIGUSR1);
      pthread_sigmask(SIG_BLOCK, &set, NULL);
      //屏蔽了SIG_BLOCK，接受SIGQUIT，SIGUSR1
      pthread_create(&thread, NULL, &sig_thread, (void *) &set);

      pause(); 
      return 0;
}
