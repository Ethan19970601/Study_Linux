#include <stdio.h>
#include <unistd.h> // _exit()要此头文件，使用方法与 exit()类似
#include <stdlib.h> // exit(),要此头文件
#include <sys/types.h>
#include <sys/wait.h>


void Worker()
{
  int cnt = 5;
  while(cnt)
  {
    printf("I am child process, pid: %d, ppid: %d, cnt= %d\n", getpid(),getppid(), cnt--);
  }
}

int main()
{
  pid_t id = fork();
  if (id == 0)
  {
    // 子进程
    Worker();
    exit(0);
  } else {
    // 父进程
    //sleep(10);
    printf("wait before\n");
    pid_t rid = wait(NULL);
    printf("wait after\n");
    if (rid == id)
    {
      printf("wait success, pid: %d\n", getpid());
    }
  }
  sleep(10);
  return 0;
}
//int fun()
//{
//  printf("call fun function done!\n");
//  return 11;
//  // 任意地点调用exit，表示进程退出，不进行后续执行。
//  // exit(21);
//}
//
//int main()
//{
//  printf("you can see me !");
//  sleep(3); 
//  _exit(1);
////  fun();
//  printf("i am a process, pid: %d, ppid: %d\n", getpid(), getppid());
//  // 1. exit(参数)， 参数是进程的退出码，类似与 main 函数的return n
  // _exit(12);
 
  // 1. 在 main 函数中直接进行 return
  // 2. 在其他函数中调用return 表示的是函数调用结束
  // return 21;

