#include "stdio.h"
#include <string>
#include <SHLibCommon.h>
#include "utilit.h"
#include <stdlib.h>
#include <string.h>
#include <SHConfig.h>
#ifdef WIN32
#else
   #include <stdlib.h>
   #include <stdio.h>
   #include <unistd.h>
   #include <sys/types.h>
   #include <string.h>
   #include <getopt.h>
   #include <sys/time.h>
   #include <sys/resource.h>
   #include <sys/wait.h>
   #include <sys/socket.h>
   #include <sys/stat.h>
   #include <sys/syscall.h>
   #include <sys/prctl.h>
   #include <ucontext.h>
   #include <arpa/inet.h>
   #include <fcntl.h>
   #include <sched.h>
   #include <pthread.h>
   #include <syslog.h>
   #include <signal.h>
   #include <dlfcn.h>
   #include <error.h>
   #include <errno.h>

   void daemonize(void){
       return ;
       pid_t pid;
       struct rlimit rl;
       int fd, i;
       printf("Daemonize() is starting...\n");
       umask(0);
       if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
          printf("getrlimit(): %d: %s.", errno, strerror(errno));
          exit(1);
       }
       if ((pid = fork()) < 0) {
          printf("fork(): %d: %s.", errno, strerror(errno));
          exit(1);
       } else if (pid != 0)
          exit(0);
       setsid();
       if ((pid = fork()) < 0) {
          printf("fork(): %d: %s.", errno, strerror(errno));
          exit(1);
       } else if (pid != 0)
          exit(0);
       if (chdir("/") < 0) {
          printf("chdir(): %d: %s.", errno, strerror(errno));
          exit(1);
       }

       if (rl.rlim_max == RLIM_INFINITY)
          rl.rlim_max = 1024;
       for (i = 0; i < rl.rlim_max; i++)
          close(i);
       fd = open("/dev/null", O_RDWR);
       dup(0);
       dup(0);
       printf("Daemonize() OK.");
   }
#endif
