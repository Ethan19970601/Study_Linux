
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


下面👇🏻我们要做的就是根据刚才的分析内容，来得到我们要的退出信息，所以我们要修改一下我们的代码：

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
        exit(10);   
    }
    else
    {
        // 父进程
        //sleep(10);                
        printf("wait before\n");
        int status = 0;     
        pid_t rid = waitpid( id, &status,0);  
        printf("wait after\n");              
        if (rid == id)
        { 
            printf("wait success, pid: %d, rpid: %d, exit sig: %d, exit code: %d\n", getpid(), rid, status&0x7F, (status>>8)&0xFF);
        }
    }
    sleep(10);
    return 0;
}

```


修改的部分：
```c
printf("wait success, pid: %d, rpid: %d, exit sig: %d, exit code: %d\n", getpid(), rid, status&0x7F, (status>>8)&0xFF);
```

```c
rpid: %d ,rid     // 我们可以理解：waitpid 对应的返回值。
```

```c
exit sig: %d, status&0x7F  
// 表示的是退出时收到的信号
//它是位于低 7 位的，所以当我们按位与上(&)：0x7F使用按位与运算来确保只获取 `status` 变量的最低7位，通过与 `0x7F`（二进制 `0111 1111`）进行按位与运算实现。这样，任何高于第7位的位都会被置为0，而低于或等于第7位的位则保持不变。而后 7 位就是我们想要的退出信号了

```

```c
exit code: %d, (status >> 8)&0xFF  
// 表示的是退出时收到的退出码
//我们这里是先给它右移动 8 位，然后在来按位与的操作，这样就可以得到我们的退出码了。
```

#####   🧲不会进制转化的铁汁：

将十六进制数 `0x7F` 转换为二进制的过程相对简单。每个十六进制位可以直接转换为一个四位的二进制序列，因为十六进制是基于16的，而二进制是基于2的，十六进制的每一位可以表示二进制的四位。

下面是转换过程：

- 十六进制的 `7` 转换为二进制是 `0111`。
- 十六进制的 `F` 对应于十进制的15，转换为二进制是 `1111`。

因此，十六进制的 `0x7F` 转换为二进制是：

```
0x7F = 0111 1111
```

运行上面的代码之后，我们就可以在打印的结果中看到退出信息：``exit sig``，退出码：``exit code`` 

我们为了看到打印的退出码和退出信息，我们可以故意将我们的代码写错，特意的制造一些错误：比如：
我们可以对空指针解引用，当我们特意这么做时，我们可以看到打印的结果如下：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/93d915011d8249b6bbb7d47a839cb5ae.png)

### 根据上面的现象，我有几个问题：

#### 当一个进程异常了（即：收到了信号），退出码(``exit code`` )， 还有意义吗?

答：当一个进程出异常了它的退出码是没有意义的，来看我们下面这张图：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/822267e2c8ca46b49a7698f9f314466a.png)




![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/53185f7bf2484a09bb60fc559d9995e7.png)


所以当一个进程异常的时候，它对应的退出码没有被使用，可能就是一些随机值之类的。

#### 那我们怎么判定有没有收到信号？

我们可以通过 ``kill -l`` 来查看我们的信号列表：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/c24369801fc94429a9baa424b0e43230.png)

我们可以知道上面👆🏻的信号列表中是没有 ``0`` 号信号的，所以我们只用知道 ``exit sig `` 是不是 ``0`` 来判断有没有收到信号。 

这样也恰如我们之前所说：

> exit sig : 0, exit code : 0;                // 表示的是，代码跑完，结果正确
> exit sig : 0, exit code : (非 0)；      // 表示的是，代码跑完，结果不正确
> exit sig : (非 0), exit code : （随机数）;        // 表示的是，出异常了。


# 🏷️知识拓展

## 📌 父进程是如何得知子进程的退出信息的？

调用 wait waitpid 这样的系统调用，在操作系统层面是怎么做的。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/81fbcb131d5a478e92050feee3a600f2.png)

 🧲 **忘了进程的状态的老铁，看这里简单了解一下：**
 
在Linux系统中，进程的状态被详细划分为多种，以便操作系统能够有效地管理和调度进程。这些状态反映了进程在其生命周期内的不同执行阶段和等待条件。以下是Linux系统中常见的进程状态及其解释：

##### 1. 运行状态（Running，R）

- 进程正在CPU上执行，或者已经准备好执行并等待CPU时间片。在Linux中，这个状态通常被称为`TASK_RUNNING`。
- 当多个进程处于这个状态时，它们会被放入CPU的可执行队列中，等待调度器分配时间片。

##### 2. 可中断的睡眠状态（Sleeping，S）

- 进程因为等待某些事件（如I/O操作完成、信号等）而被阻塞。这种状态下，进程可以被信号或中断唤醒。
- 在Linux中，这个状态通常与`TASK_INTERRUPTIBLE`相对应。当等待的事件发生时，进程会被唤醒并重新进入就绪状态。

##### 3. 不可中断的睡眠状态（Disk Sleep，D）

- 进程正在等待某些IO操作完成，且不能被任何信号中断。这种状态通常出现在进程对硬件设备进行IO操作时，需要保证操作的原子性。
- 在Linux中，这个状态称为`TASK_UNINTERRUPTIBLE`。由于这种状态下进程无法被中断，因此通常只在非常关键的IO操作中使用。

##### 4. 暂停状态（Stopped，T）

- 进程被暂停执行，通常是因为收到了SIGSTOP、SIGTSTP等信号。在这种状态下，进程不会占用CPU资源，也不会响应任何信号，直到收到SIGCONT信号才能恢复执行。
- 在Linux中，这个状态可能对应`TASK_STOPPED`或`TASK_TRACED`，后者表示进程正在被调试器跟踪。

##### 5. 僵尸状态（Zombie，Z）

- 进程已经执行完毕，但其父进程尚未通过wait()或waitpid()等系统调用回收其资源。在这种状态下，进程占用的PCB（进程控制块）仍然存在，但已经不再执行任何代码。
- 僵尸状态是进程退出过程中的一个过渡阶段，用于保存进程的退出状态等信息，以便父进程查询。

##### 6. 追踪状态（Tracing Stop，t）

- 进程正在被调试器跟踪，并处于暂停状态。这种状态与暂停状态类似，但通常是在调试过程中由调试器主动设置的。

##### 7. 死亡状态（Dead，X）

- 严格来说，死亡状态并不是Linux进程的一个独立状态，因为一旦进程达到这个状态，其资源就已经被回收，进程本身也就不复存在。但是，在描述进程生命周期时，有时会提到这个状态作为进程结束的标志。
 
 **我们上面图中的系统调用的工作：**
 - 将我们的进程状态由 ``Z`` -> ``X`` 
 - 将退出码和退出信息组合起来传递给我们传入的变量：status。 
	 - 具体过程就是：`` *statusp = ( exit_code << 8 ) | exit_siganl ``



上面获取 exit_code 和 exit_siganl 我们都使用了 **位操作** ，但是 Linux 也做了一个封装，让我们可以不用位操作来获取进程的退出码和退出信息。如下👇🏻：

status:

``WIFEXITED(status)``: 若为正常终止子进程返回的状态，则为真。（查看进程是否是正常退出）
 > WIFEXITED,这个宏对 stauts 中的信号值做检测，如果进程是正常终止的这个条件就为真
 
 ``WEXITSTATUS(status)`` : 若WIFEXITED非零，提取子进程退出码。（查看进程的退出码）
 > 如果进程是正常退出的。 WEXITSTATUS,这个宏可以提取进程的退出码


好，我们现在将上面的内容加入我们的代码之中,我们可以这样写：
```c
if(WIFEXITED(status)) //注意哈，这里的拼写和下面 👇🏻printf里的那个拼写是不一样的哈。
{
	printf("child process normal quit, exit code: %d\n", WEXITSTATUS(status));
} else {
	printf("child process quit except!\n");
}
```

完整代码是这样的：

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
        exit(1);
    }
    else
    {
        printf("wait before\n");
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        printf("wait after\n");
        if (rid == id)
        {
            if (WIFEXITED(status)) // 注意哈，这里的拼写和下面 👇🏻printf里的那个拼写是不一样的哈。
            {
                printf("child process normal quit, exit code: %d\n", WEXITSTATUS(status));
            }
            else
            {
                printf("child process quit except!\n");
            }
        }
    }
    return 0;
}

```


## 📌 等待多个进程

我们之前的例子🌰都只是 fork()了一个子进程，现在我们来创建多个子进程看父进程是如何等待的。

我们首先要修改一下我们的代码：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void Worker(int number)
{
    int cnt = 10;
    while (cnt)
    {
        printf("I am child process, pid: %d, ppid: %d, cnt= %d, number: %d\n", getpid(), getppid(), cnt--, number);
    }
    sleep(1);
}

const int n = 10;

int main()
{
    for (int i = 0; i < n; i++)
    {
        pid_t id = fork();
        if (id == 0) // 如果 id=0，说明它是一个子进程
        {
            Worker(i);
            exit(i);
        }
    }

    // 如何等待多个子进程？
    for (int i = 0; i < n; i++)
    {
        int status = 0;
        pid_t rid = waitpid(-1, &status, 0);
        // 如果``pid > 0``,代表的是指定等待的那个id的进程。
        // 如果是 ``-1`` 代表的是任意一个退出的子进程
        if (rid > 0)
        {
            printf("wait child process : %d success, exit_code: %d\n", rid, WEXITSTATUS(status));
        }
    }
    return 0;
}

```



## 📌 waitpid 的第 3 个参数。


#### 问题：

- 我们为什么不用全局变量来获取子进程的退出信息？而用系统调用？？

在我们的代码里，你看看：
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int status = 0; // 我们为什么不在这里定义一个全局变量

void Worker(int number)
{
    int cnt = 10;
    while (cnt)
    {
        printf("I am child process, pid: %d, ppid: %d, cnt= %d, number: %d\n", getpid(), getppid(), cnt--, number);
    }
    sleep(1);
}

const int n = 10;

int main()
{
    for (int i = 0; i < n; i++)
    {
        pid_t id = fork();
        if (id == 0) 
        {
            Worker(i);
			status = i; // 我们在加入这样一行 
            exit(0);
        }
    }

    for (int i = 0; i < n; i++)
    {
        int status = 0;
        pid_t rid = waitpid(-1, &status, 0);
        if (rid > 0)
        {
            printf("wait child process : %d success, exit_code: %d\n", rid, WEXITSTATUS(status));
        }
    }
    return 0;
}
```

你所定义的全局变量，是父进程开始有的。然后当你在创建子进程的时候，你如果要子进程退出时将子进程的状态写到这个全局变量里，此时你写入的这个值父进程是读不到的。父进程为什么读不到？因为从操作系统的层面上在多进程当中，当对同一个变量进行写入操作时，会发生写时拷贝。因为进程具有独立性，所以不要觉得父子进程写在一份代码里，我们就能获取子进程的数据了 。因为进程具有独立性所以父进程没办法拿到子进程的数据，所以我们是要通过系统调用让操作系统帮我去拿。 

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/23f24200a9f44185a94c83112600c4fc.png)

####  ❓什么是非阻塞？

如何来理解呢？

**🍖阻塞：**
想象一下，你在一家餐厅里，服务员问你需要什么。如果你选择“阻塞等待”，就像是你坐在餐桌旁，什么都不做，只等着服务员给你上菜。在这期间，你不能做其他任何事情，比如玩手机或者和朋友们聊天，因为你的注意力完全集中在等待上菜上。

**🍖非阻塞：**
而“非阻塞等待”则像是你可以告诉服务员，你不需要他们立即上菜。这样，你就可以继续做其他事情，比如和朋友聊天或者玩手机。当你的菜准备好了，服务员会过来告诉你，这时候你再开始吃饭。在等待的过程中，你没有被“卡住”，而是可以做其他事情。

在编程中，非阻塞操作就是让计算机程序在等待某个任务完成（比如从网络下载文件）时，不必停下来不做其他事情。程序可以继续执行其他代码，直到那个任务完成并通知程序。这样，程序就可以更有效地利用时间，不会在等待的时候“闲置”。


#### 所以当我们把最后一个参数（options）设置为 ``WNOHANG``

我们的``waitpid`` 就会有三种返回值：

1. rid > 0
2. rid == 0
3. rid < 0

  (1).   rid > 0 : 代表等待成功
      rid < 0 : 代表等待失败
    
（2).  rid == 0 : 代表的是等待是成功的，但是对方还没有退出。

