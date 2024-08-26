#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>

void Worker()
{
    int cnt = 5;
    while (cnt)
    {
        printf("I am child process, pid: %d, ppid: %d, cnt= %d\n", getpid(), getppid(), cnt--);
    }
}

int main()
{
    pid_t id = fork();
    if (id == 0)
    {
        // 子进程
        Worker();
        exit(10);     /***************       我们将退出结果修改为 10     **************/
    }
    else
    {
        // 父进程
        //sleep(10);                
        printf("wait before\n");
        int status = 0;     /*************  我们加入这一行，默认设为 0 **************/
        pid_t rid = waitpid( id, &status,0);  // 这里也可以体会一下 status 输出型参数的用法，
										      // 把子进程退出时的退出信息通过 status 给我们返回。
        printf("wait after\n");              
        if (rid == id)
        { 
            printf("wait success, pid: %d, status: %d\n", getpid(), status);
        }
    }
    sleep(10);
    return 0;
}
