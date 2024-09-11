
# 🏷️ 引入：

我们之前所创建的子进程，它执行的代码都是父进程的一部分。如果我们想要我们的子进程去执行全新的的代码，访问全新的数据，不要再和父进程有瓜葛我们应该怎么办呢?

 这里我们就要使用：程序替换

# 🏷️ 见一见程序替换-单进程版的程序替换的代码（没有子进程）

代码如下：
```c
#include <stdio.h>
#include <unistd.h>

int main() 
{
    printf("pid: %d, exec command begin\n", getpid());
    // 用 execl 这个函数来调用系统的命令
    execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
    printf("pid: %d, exec command end\n", getpid());
    return 0;
}
```

上面👆🏻代码的执行结果就和我们在命令行里输入的指令：``ls -a -l `` 相同。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/ea23bc25b2484ec6b5522e86fe72f305.png)

观察一下上面运行的结果你会发现。
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3766baa461ba479c9c3c92244f250277.png)
下面我们简单介绍一下``execl`` 这个函数

**函数原型：**

``int execl(const char *path, const char *arg0, ..., (char *)NULL);``

这里的参数意义如下：

- `path`：要执行的程序的路径。
- `arg0`：传递给新程序的第一个参数，通常是程序的名称。
- `...`：后续的参数列表，这些参数将作为新程序的 `argv` 参数传递。参数列表必须以 `(char *)NULL` 结尾，以表示参数列表的结束。
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/63c9fc3d1a71487f92bd213d7459213f.png)

==好了，``execl`` 介绍完了，我们的问题是：我们的代码里面不是还有一条语句吗，它为什么没有去执行呢？==

我们先把这个问题留着，讲解其他知识之后再做解答。



# 🏷️ 理解和掌握程序替换的原理，更改多进程版的程序替换的代码，扩展理解和掌握程序替换的原理多进程


## 📌 程序替换的原理一
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/e69f001de58a45fd947570c4f9ba6979.png)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/276ecaa97e4c49029fa62d16eaeb63c6.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/061b5b39fe154093b88910b6bc622f62.png)




![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b62b9f9f17ed480abf38e608f4deb156.png)


 **好的，问题来了，那么这个过程有没有创建新的进程呢？** 
 
 答：并没有。因为当我在替换新程序时，只是把我们目标程序的代码和数据替换到原来进程的壳子当中，也就是说整个进程的 pid 是不会反生任何改变的。

## 📌 我们把单进程的改成多进程的

我们之前单进程版本的程序替换的代码如下：
```c
#include <stdio.h>
#include <unistd.h>

int main() 
{
    printf("pid: %d, exec command begin\n", getpid());
    // 用 execl 这个函数来调用系统的命令
    execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
    printf("pid: %d, exec command end\n", getpid());
    return 0;
}
```

我们现在来改成多进程版本的。

我们将代码修改成如下：
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    pid_t id = fork(); // 创建子进程
    if (id == 0)
    {
        // 这里是子进程
        printf("pid: %d, exec command begin\n", getpid());
        sleep(3);
        // 用 execl 这个函数来调用系统的命令
        execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
        printf("pid: %d, exec command end\n", getpid());
        exit(1); // execl 这个函数如果执行成功，就不会来到这下面的语句，如果执行失败才会来到这里，所以我们这里就直接退出
    }
    else 
    {
        // 这里是父进程
        pid_t rid = waitpid(-1, NULL, 0);
        if (rid > 0)
        {
            printf("wait scuess, rid: %d\n", rid);
        }
    }
    return 0;
}
```

代码的运行结果：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/0f43fd326f43463383a27457c8eaff1f.png)
### ✏️如何来理解这个多进程发生程序替换的这个过程？

当我们今天创建子进程的时候，我们自己心里比较清楚的一点是：子进程也有自己的 PCB、地址空间、页表。
我们知道父进程和子进程通过页表的映射指向的同样的代码区和数据区，创建父子进程的时候，子进程要和父进程代码共享，数据以写时拷贝的方式各自私有一份

那么当我们程序替换的时候 实际上是把可执行程序它对应的代码和数据替换到子进程的代码和数据。但是问题来了，子进程的数据和代码是和父进程共享的，你程序替换把子进程的给替换了，那会不会影响到父进程呢？

首先，当你替换对应的数据的时候，因为父子进程的数据是以写时拷贝的方式各种私有一份的，所以并不会影响。
但是如果你替换对应的代码该怎么办呢？同样也是**写时拷贝**，在这种场景之下，我们也要把代码段进行写时拷贝。

程序替换的本质就是将新程序的代码和数据替换到我们之前进程（调用 execl的那个进程）的代码和数据，在替换的过程当中，如果是单进程就直接替换，如果是在子进程当中替换，我们要发生写时拷贝来保证父子进程的独立性


### 回答之前遗留的问题：我代码中最后一行还有个 ``printf`` ,为啥没给我执行这句代码呢？

因为当它调用``exec`` 程序替换 ，只要程序替换成功了，其实 eip都被改掉了，所以你的子进程就转而去执行新的程序了， 如果是在单进程的场景下，你调用 ``execl`` 成功之后，你的子进程的代码和数据都被替换掉了，所以也无法执行那个``printf`` 


```
当我们执行 exec* 这样的函数，如果当前进程执行成功，则后续代码没有机会在执行了!因为被替换掉了!
```

``exec* `` 只有失败的返回值，没有成功的返回值



# 🏷️大量使用其他的程序替换的方法——父子进程场景中

任何程序替换必须解决的两个问题：
- 必须找到这个可执行程序
- 必须告诉``exec*`` 怎么执行


### ✏️ 我们再来认识一下：``execl`` 这个函数

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/34bbf320ed784faf850b2aec052316b3.png)

### ✏️ ``execlp`` 函数


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8d99aceeeb654f118b4ea15371b8347b.png)


```c
execlp("ls", "ls", "-a", "-l", NULL); // 注意，第一个“ls”表示的是文件名，第二个“ls”表示的是命令行里的参数
```


### ✏️ ``execv`` 函数

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/5ab0040896b749e78436ea7089d70003.png)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    pid_t id = fork(); // 创建子进程
    if (id == 0)       // 这里是子进程
    {
        char *const argv[] = {
            "ls",
            "-a",
            "-l",
            NULL};
        printf("pid: %d, exec command begin\n", getpid());
        sleep(1);
        execv("/usr/bin/ls", argv);
        // execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
        // execlp("ls", "ls", "-a", "-l", NULL); // 注意，第一个“ls”表示的是文件名，第二个“ls”表示的是命令行里的参数

        printf("pid: %d, exec command end\n", getpid());
        exit(1);
    }
    else
    {
        // 这里是父进程
        pid_t rid = waitpid(-1, NULL, 0);
        if (rid > 0)
        {
            printf("wait scuess, rid: %d\n", rid);
        }
    }
    return 0;
}
```


### ✏️ ``execvp`` 函数

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/dfbfddc25c664b59ab296f960df9ac60.png)


### ✏️ 问题：我们的程序替换，能替换系统指令程序，能替换我写的程序吗？


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/44196cbfb8324a58bda85e6db4eed3be.png)
``.cc`` 后缀的文件是 c++ 程序文件。

我们可以在这个``mytest.cc`` 写下如下 c++ 代码：
```c++
#include <iostream>

int main()
{
    std::cout << "hello c++" << std::endl;
    std::cout << "hello c++" << std::endl;
    std::cout << "hello c++" << std::endl;
    std::cout << "hello c++" << std::endl;
    std::cout << "hello c++" << std::endl;
    
    return 0;
}
```

同时，我们也要相应的去修改我们的``Makefile`` 文件，

```Makefile

.PHONY:all

all:mytest myprocess
// 这样我们才能在 make 的时候一次形成两个可执行程序

mytest:mytest.cc

g++ -o $@ $^ -std=c++11

  
myprocess:myprocess.c

gcc -o $@ $^ -std=c99

.PHONY:clean

clean:

rm -f myprocess mytest
```

看看我们今天写的程序：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/80d02373f19749268d394f1cb53e77f0.png)

我们现在的实验是：用上面的c程序替换掉我们的 c++程序。（用今天学的程序替换的方式）

在此之前，我们也要对我们写的 C 程序做出相应的修改：

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    pid_t id = fork(); // 创建子进程
    if (id == 0)       // 这里是子进程
    {
        execl("./mytest", "mytest", NULL); 
        printf("pid: %d, exec command end\n", getpid());
        exit(1);
    }
    else
    {
        // 这里是父进程
        pid_t rid = waitpid(-1, NULL, 0);
        if (rid > 0)
        {
            printf("wait scuess, rid: %d\n", rid);
        }
    }
    return 0;
}
```


好，我们做好上面的准备工作之后，开始跑一下：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a4914627d5324474afdd007d70624c45.png)
哎，也能行！

### ✏️ 一个程序是怎么加载到内存里的

通过今天的学习你会发现：``exec*``的这些函数的调用的过程不就是一个程序加载到内存里的那个过程吗？
想想我们之前的那一张图：



![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b62b9f9f17ed480abf38e608f4deb156.png)

这不就是把磁盘中的程序加载到内存中的过程吗，不就是

## 📌 解释一下：``./mytest`` 这个程序从开始加载到调度运行到最后退出的整个过程


我们当前的系统识别到了对应的可执行程序，它要创建对应的进程，它会先创建进程的内核数据结构（什么 PCB，地址空间，页表之类的），然后把磁盘中的相应的代码和数据通过类似 exec* 这样的接口加载到物理内存之中。



