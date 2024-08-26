
### ✏️ 代码引入：

```c
#include <stdio.h>
#include <unistd.h> // _exit()要此头文件，使用方法与 exit()类似
#include <stdlib.h> // exit(),要此头文件

// int fun()
//{
//   printf("call fun function done!\n");
//   return 11;
//   // 任意地点调用exit，表示进程退出，不进行后续执行。
//   // exit(21);
// }
//
int main()
{
    printf("you can see me !");
    sleep(3);
    _exit(1);
    //  fun();
    //  printf("i am a process, pid: %d, ppid: %d\n", getpid(), getppid());
    //  // 1. exit(参数)， 参数是进程的退出码，类似与 main 函数的return n
    // _exit(12);

    // 1. 在 main 函数中直接进行 return
    // 2. 在其他函数中调用return 表示的是函数调用结束
    // return 21;

    return 0;
}
```


### ✏️ `_exit`函数

```c
#include <unistd.h>  
void _exit(int status);
```

- `status`定义了进程的终止状态，父进程通过`wait`来获取该值。

###  ✏️  `exit`函数

`exit`在调用前会：

1. 执行用户通过`atexit`或`on_exit`定义的清理函数。
2. 关闭所有打开的流，所有的缓存数据均被写入。
3. 调用`_exit`。

### 示例

```c
#include <unistd.h>
#include <stdio.h>
int main()
{
    printf("hello");
    exit(0);
}
```



### ✏️ 关于``_exit``和``exit``的区别：

- ``_exit``是系统调用。
- ``exit``是库函数。
两者在终止进程时对缓冲区的处理差异：
- 当``exit``被调用终止进程时，会自动刷新缓冲区。
- 当``_exit``被调用终止进程时，不会自动刷新缓冲区。




#  🏷️ 进程等待

## 📌 什么是进程等待

通过``wait/waitpid``的方式，让父进程（一般）对子进程进行资源回收的等待过程

## 📌 为什么要进行进程等待

a. 解决子进程僵尸带来的内存泄漏问题---- 目前必须要解决
b. 父进程为什么要创建一个子进程呢？ ---- 要让子进程来完成任务。
子进程任务完成的怎麽样，父进程要不要知道？要知道，所以我们要通过进程等待的方式来获取子进程退出的信息。----即：获取子进程退出时的那两个数字（1. 信号编号 2. 进程退出码）

### ✏️  进程等待必要性

- 之前讲过，子进程退出，父进程如果不管不顾，就可能造成‘僵尸进程’的问题，进而造成内存泄漏。

- 另外，进程一旦变成僵尸状态，那就刀枪不入，“杀人不眨眼”的kill -9 也无能为力，因为谁也没有办法杀死一个已经死去的进程。

- 最后，父进程派给子进程的任务完成的如何，我们需要知道。如，子进程运行完成，结果对还是不对，或者是否正常退出。

- 父进程通过进程等待的方式，回收子进程资源，获取子进程退出信息


## 📌 如何进行进程等待（重点）

### ✏️  进程等待的方法

- `wait`方法
- `waitpid`方法

### 📎 `wait`方法, 先验证 2 个问题

1. ``wait``是真的能帮我回收子进程，也就是僵尸状态，最后由有一直到无我们要通过``wait``来回收到让大家看到
2. 再验证一下wait调用时父进程在调的时候，如果子进程没退出，那么在父进程在干什么？


####  ``wait``的头文件，返回值 
```c++

#include<sys/types.h>

#include<sys/wait.h>

pid_t wait(int*status);

返回值：

成功返回被等待进程pid，失败返回-1。

参数：

输出型参数，获取子进程退出状态,不关心则可以设置成为NULL

```

**程序验证** 代码如下：
```c++
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
        exit(0);
    }
    else
    {
        // 父进程
        sleep(10);
        pid_t rid = wait(NULL);
        if (rid == id)
        {
            printf("wait success, pid: %d\n", getpid());
        }
    }
    sleep(10);
    return 0;
}
```

> [!NOTE] Liunx
>  在命令行中，我们使用 `` ./myprocess``，来运行程序，在这里我的可执行的程序名叫:myprocess
>  然后我们新建一个远程链接，在同样的目录下输入我们的监控指令，监控指令如下：
>  **`` while :; do ps ajx |head -1 && ps ajx | grep myprocess |grep -v grep; echo '-------------------------------------------------------------------------'; sleep 1; done``**
>  

我们可以看到如下结果：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a5a8355dcacb4dc79c944da8247c5e25.png)


下面我来解释一下：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/e78ce397dbe844ec81e531692d58c59d.png)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d1f7941a626549b89f109b09ecaafa36.png)


现在我们来回答之前的两个问题：
1. ``wait``是真的能帮我回收子进程，也就是僵尸状态，最后由有一直到无我们要通过``wait``来回收到让大家看到

> 进程等待能够回收子进程僵尸状态，就是系统把子进程的状态由 ``Z-``变成 ``X``, ``x`` 状态就可以彻底宣判此进程的资源可以回收了。此时操作系统就会瞬间把资源给回收了，所以我们看不到这个 ``x`` 状态， 

2. 再验证一下wait调用时父进程在调的时候，如果子进程没退出，那么在父进程在干什么？

我们来观察一下之前的代码：
```c
#include <stdio.h>
#include <unistd.h> // _exit()要此头文件，使用方法与 exit()类似
#include <stdlib.h> // exit(),要此头文件
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
        exit(0);
    }
    else
    {
        // 父进程
        sleep(10); /************************ 看这里 ****************************/
        pid_t rid = wait(NULL);
        if (rid == id)
        {
            printf("wait success, pid: %d\n", getpid());
        }
    }
    sleep(10);
    return 0;
}
```

之前的时候，我们让我们的父进程``sleep``了``10``秒， 在父进程 sleep 的时候是我们的子进程在跑，跑了 5 秒。如果我们不要父进程 sleep，那么父进程在运行的时候，就会立马执行到代码里的``pid rid = wait(NULL)``， 立马执行到 了``wait`` , 这个时候问题来了，**在子进程执行期间（也就是还没有退出的时候），父进程有没有调用 wait 呢？如果调用了的话，那么在干些什么呢？** 
为了解决我们上面👆🏻的问题，我们对代码做出如下的修改：

```c
#include <stdio.h>
#include <unistd.h> // _exit()要此头文件，使用方法与 exit()类似
#include <stdlib.h> // exit(),要此头文件
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
        exit(0);
    }
    else
    {
        // 父进程
        // sleep(10);  /********************  看这里    ***************************/
        printf("wait before\n"); // 我们在wait 前面加上这个
        pid_t rid = wait(NULL);
        printf("wait after\n");  // 在 wait 后面加上这句 ，我们在观察打印出来的结果
        if (rid == id)
        {
            printf("wait success, pid: %d\n", getpid());
        }
    }
    sleep(10);
    return 0;
}
```


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a2cf8146b0cb4d59845a5cc68d141d8f.png)


上面👆🏻这个例子说明，当我们在执行wait的时候，如果人家子进程没有退出，那么你的等待就要让父进程进行阻塞时等待，即：如果子进程根本没有退出，父进程必须在 wait 上进行阻塞等待，直到子进程僵尸🧟‍♀️， wait 自动回收返回！

同时通过上面的例子🌰，我们也可以知道，一般而言，父子进程谁先运行我们不知道，但是一般都是父进程最后退出。因为父进程要等待子进程死掉了，然后父进程再把这些僵尸进程给回收♻️了，才退出。 所以在多进程代码中，往往是多进程的 多执行流由父进程发起 ，最后由父进程统一回收。


> 到这里，我们已经成功的解决了[[#📌 为什么要进行进程等待]]这个问题里面的``a``问题（a. 解决子进程僵尸带来的内存泄漏问题)。
> 现在我们来解决``b`` 问题：（b. 父进程为什么要创建一个子进程呢？
>  ---- 要让子进程来完成任务。子进程任务完成的怎麽样，父进程要不要知道？要知道，所以我们要通过进程等待的方式来获取子进程退出的信息。----即：获取子进程退出时的那两个数字（1. 信号编号 2. 进程退出码）
>  为了获取进程退出时的那两个数字（1.信息编号 2. 进程退出码）我们可以使用 ``waitpid`` 这个函数来获取


### 📎 ``waitpid`` --- 获取退出的信息 


#### 函数原型

`pid_t waitpid(pid_t pid, int* status, int options);`

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/9cb37086cec04d9eac8faaca8be59323.png)
#### 返回值:

waitpid 和 wait 的返回值的意思一模一样
  - 如果 >0 正常返回时，返回收集到的子进程的进程ID。
  - 如果调用中出错，则返回-1，并且`errno`会被设置成相应的值。

#### 参数部分:
  - `pid`: 指定等待的子进程的进程ID。
    - `Pid=-1`: 等待任一个子进程，与`wait`函数等效。
    - `Pid>0`: 等待其进程ID与`pid`相等的子进程。
  - `status`: 用于存储子进程状态信息的指针。
    - `WIFEXITED(status)`: 若为正常终止子进程返回的状态，则为真，用于检查进程是否是正常退出。
    - `WEXITSTATUS(status)`: 若`WIFEXITED`非零，提取子进程退出码。
  - `options`:
    - `WNOHANG`: 若指定的子进程没有结束，则`waitpid()`函数返回0，不予以等待。若正常结束，则返回该子进程的ID。




- 当子进程已经退出时，调用`wait`或`waitpid`会立即返回，同时释放相关资源，并提供子进程的退出信息。
    
- 如果在任意时刻调用`wait`或`waitpid`，并且子进程正在正常运行，则调用进程可能会阻塞。
    
- 如果尝试调用`wait`或`waitpid`的子进程不存在，则调用会立即出错并返回。

### 我们使用 waitpid 来做两个实验

-  实验一： 用 waitpid 来替代掉我们之前代码中的 wait 来达到同样的效果
-  实验二： 用 waitpid 来获取我们想要获得的参数


#### 我们先来看实验 1：

我们之前的代码是这样的：
```c
#include <stdio.h>
#include <unistd.h> // _exit()要此头文件，使用方法与 exit()类似
#include <stdlib.h> // exit(),要此头文件
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
        exit(0);
    }
    else
    {
        // 父进程
        // sleep(10); 
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
```

我们将其中的``wait`` 全部替换为 ``waitpid`` 同时参数部分也应该要做相应的修改：

```c
pid_t rid = wait(NULL);---------> pid_t rid = waitpid(id, NULL, 0);
```

我们修改之后的代码：

```c
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
        exit(0);
    }
    else
    {
        // 父进程
        sleep(10);                // 为了看到更好的效果，我们这里取消注释
        printf("wait before\n");
        pid_t rid = waitpid( id, NULL,0); // 修改之后
        printf("wait after\n");
        if (rid == id)
        {
            printf("wait success, pid: %d\n", getpid());
        }
    }
    sleep(10);
    return 0;
}
```

观察代码的运行结果，你会发现结果也 同样的符合我们的预期

#### 接下了，我们进行实验二

下面👇🏻我们细细说一下``waitpid`` 的 ``status`` 参数：

在编程中，参数可以被分为几种类型，其中之一就是“输出型参数”（Output Parameter）。输出型参数是一种特殊的参数，它在函数调用时被传递给函数，并且在函数执行过程中被修改，其修改后的值在函数返回后可以被调用者访问。
`status` 参数是一个指向整数的指针（`int *status`），这意味着它是一个输出型参数。当 `waitpid` 函数被调用时，如果 `status` 不是 `NULL`，函数会将子进程的退出状态写入到 `status` 指针指向的内存位置。这样，调用者就可以在 `waitpid` 函数返回后检查这个状态，了解子进程是如何终止的。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/4b3f2b32a4cb48d5803af3334d20441d.png)

好了，在了解 ``status`` 之后，我们来修改一下之前的代码：

```c
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
```

我们在 Linux 命令行中使用命令来运行上面的代码之后,会看到这样的结果：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/5f99d673fc96499fb7b029a0c12f3ff8.png)


这个时候，我们就有一个疑问❓，我们不是设置了``exit(10)``吗？那为什么最后打印的``status`` 不是 10 而是 2560？

为了理解这个问题，我们要谈一谈下一个话题：status 的一个构成问题。

 在 Linux 系统中我们的``waitpid`` 的第二个参数，即：status 是一个 int 类型的数 ，但是这里的整数并不是整存整取的，我们知道一个整数有 32 个 bit， 这里是按照我们将一个整数进行区域划分 ，从不同的 比特位区域来表示不同的含义的，而我们在后续使用这个``status``的时候，只考虑 ``status``整数的低 16 位，（从左向右一共 32 个 bit，我们只需要最右侧的那 16 个 bit 就可以了）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/53185f7bf2484a09bb60fc559d9995e7.png)


现在我们来解释为什么打印的的时候``status = 2560`` 。（记住：我的代码中子进程的退出码是设置的 10，即：exit(10) ）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/f1bbac8dcb614d57938d9d4755534843.png)










