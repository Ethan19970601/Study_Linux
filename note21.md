# 🏷️接口学习:execle

假设我们现在有以下文件：
##### 📄 myprocess.c
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()

{

    pid_t id = fork(); // 创建子进程

    if (id == 0)       // 这里是子进程

    {

        printf("pid: %d, exec command begin\n", getpid());

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

            printf("wait sucess, rid: %d\n", rid);

        }

    }

    return 0;

}
```


##### 📄 test.cc 
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


上面的``myprocess.c``文件中，我们是使用的``execl`` 这个函数来进行程序替换的。

现在我们来修改一下上面👆🏻的代码，我想要``mytest.cc`` 这个文件来实现一个打印环境变量的功能，我们可以做如下修改：

```c++
#include<iostream>

int main(int argc, char* argv[], char* env[])
{
    for (int i = 0; env[i]; i++)
    {
        std::cout << i << ":" << env[i] << std::endl;
    }
    return 0;
}
```

**对上面代码的解释**

```c++
int argc, char* argv[], char* env[] 
// `int argc`：       命令行参数的数量。
// `char* argv[]`：   一个字符串数组，包含了命令行参数。
// `char* env[]`：    一个字符串数组，包含了环境变量。
```

```c++
for (int i = 0; env[i]; i++)
//这是一个`for`循环，用于遍历环境变量数组。循环的条件是`env[i]`不为`NULL`，这意味着当前索引`i`处有环境变量。因为环境变量也是一张表，环境变量所对应的这张表是一个指针数组，这个指针数组最终以 NULL结尾，所以当我们在遍历的时候，退出循环的条件就是当 env[i]走到 NULL 的时候。
```


好了，修改之后，我们使用``./myprocess`` ,运行上面的代码（运行之前记得重新 make一下)，我们就可以看到打印出来的环境变量了。
通过观察打印出来的环境变量，我们可以得出这样的一个结论：

==当我们进行程序替换的时候，子进程对应的环境变量是从父进程那里得来的，而父进程的环境变量是从当前对应的 shell 得来的。==

我们可以验证一下：

使用``export MYVAL=6666666666666666666666666666666666666666666`` 命令来自定义一个环境变量到当前 bash 这个 shell 中，名字叫：MYVAL。对应的内容是：6666666666666666666666666666666666666666666

我们可以使用 echo 命令查看一下：``echo $MYVAL``;

然后重新再运行一下``./myprocess`` ，就可以看到程序打印的环境变量中 有我们自定义的环境变量了。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8be370fbfdb343868691f2378f33a1b3.png)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/10cab4ee6bc04b0fb373554d0d5b87ee.png)

我们还可以进行一次尝试，上面我们导入的环境变量是直接导入到 bash 中的，这次我们在父进程中，导入一个环境变量，看看子进程还拿不拿得到，这个时候我们要修改一下我们的代码。
##### ❓ 我们如何让我们的父进程自己导入一个环境变量呢？

这个时候我们要引入一个新的函数：``putenv``  头文件是: ``stdlib.h`` ，这个函数的作用就是导入一个环境变量

```c
# include <stdlib.h>
int putenv(char* string);
```

好，知道了上面的知识，我们把我们的``myprocess.c`` 代码修改成如下:
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    char *env_val = "MYVAL2=88888888888888888888888888888";
    putenv(env_val);

    pid_t id = fork(); // 创建子进程
    if (id == 0)       // 这里是子进程
    {
        printf("pid: %d, exec command begin\n", getpid());
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
            printf("wait sucess, rid: %d\n", rid);
        }
    }
    return 0;
}
```

make 之后，我们运行一下，也可以发现子进程中有父进程自己导入的环境变量：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/4e2245d0d1194ef79898303ccd5a1cb5.png)


在我们上面的代码之中，我们没有主动地传递过任何环境变量给子进程，但是子进程中会出现父进程的环境变量，这说明==环境变量被子进程继承下去是一种默认行为，不受程序替换的影响==，### 为什么呢？

通过地址空间可以让子进程继承父进程的环境变量数据。
但是环境变量和命令行参数也是数据呀，我们之前讲程序替换的时候不是说要把相应进程的数据段和代码段都替换掉吗？那为什么这里说**环境变量被子进程继承下去不受到程序替换的影响** ？

答案很简单： 程序替换确实是替换掉之前程序的代码段和数据段，但是环境变量不会被替换掉。


 > 环境变量具有全局属性


### ✏️子进程执行的时候，获得环境变量的方法

#### 🧲``execle``函数的相关知识：


`execle`函数是Linux系统编程中exec函数族的一员，它用于在当前进程中执行一个新的程序，替换当前进程的映像。这个函数特别之处在于它允许你指定一个新的环境变量列表来代替当前进程的环境变量。

 **函数原型：**
```c
int execle(const char *path, const char *arg, ..., char *const envp[]);
```

**参数:**
- `path`：要执行的文件的路径。
- `arg`：第一个参数是新程序的名称，后面跟着传递给新程序的参数，参数列表必须以`NULL`结束。
- `envp`：这是一个指向环境变量数组的指针数组，新程序的环境变量设置将由这个数组决定。

**返回值:**
- 如果执行成功，`execle`不会返回。
- 如果执行失败，返回-1，并设置`errno`以指示错误。

**注意事项:**
- 参数列表必须以`NULL`结束，这表示参数列表的结束。
- `envp`参数允许你为新程序定义一个全新的环境变量集合，这在执行需要特定环境配置的程序时非常有用。
- 使用`execle`时，需要注意文件路径、权限等问题，确保可执行文件是存在的，并且有适当的执行权限。

#### 🧲 方法一：将父进程的环境变量原封不动的传递给子进程

##### 1.直接用
##### 2.直接传
	看例子🌰， 这个时候我们要修改一下我们的代码：
	``myprocess.c``要改成这样，我们用的是``execle``函数
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

extern char **environ; // 在这里声明一下。

int main()
{

    pid_t id = fork(); // 创建子进程
    if (id == 0)       // 这里是子进程
    {
        printf("pid: %d, exec command begin\n", getpid());
        execle("./mytest", "mytest", "-a", "-b", NULL, environ); // 这里要报错，说 environ 未定义，但是 environ 是被包在了头文件 unistd.h 中，没办法，我们可以去上面声明一下
        printf("pid: %d, exec command end\n", getpid());
        exit(1);
    }
    else
    {
        // 这里是父进程
        pid_t rid = waitpid(-1, NULL, 0);
        if (rid > 0)
        {
            printf("wait sucess, rid: %d\n", rid);
        }
    }
    return 0;
}
```

``mytest.cc`` ,要修改为下面这样：
```c++
#include <iostream>

int main(int argc, char *argv[], char *env[])
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << i << " -> "  << argv[i] << std::endl; // 打印参数
    }
    std::cout << "#####################################" << std::endl; // 分割线而已
    for (int i = 0; env[i]; i++)
    {
        std::cout << i << ":" << env[i] << std::endl;   // 打印环境变量
    }
    return 0;
}
```

#### 🧲 方法二：传我们自己定义的环境变量---我们可以直接构造环境变量表给子进程传递

这个时候我们修改一下我们的代码，自定义环境变量。myprocess.c
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

extern char **environ; // 在这里声明一下。

int main()
{

    // 自己定义一个环境变量
    char *const myenv[] = {
        "MYVAL1=11111111111111111111111111",
        "MYVAL2=11111111111111111111111111",
        "MYVAL3=11111111111111111111111111",
        "MYVAL4=11111111111111111111111111", NULL // 注意不要忘了，以 NULL 结尾。
    };
    pid_t id = fork(); // 创建子进程
    if (id == 0)       // 这里是子进程
    {
        printf("pid: %d, exec command begin\n", getpid());
        execle("./mytest", "mytest", "-a", "-b", NULL, myenv);
        printf("pid: %d, exec command end\n", getpid());
        exit(1);
    }
    else
    {
        // 这里是父进程
        pid_t rid = waitpid(-1, NULL, 0);
        if (rid > 0)
        {
            printf("wait sucess, rid: %d\n", rid);
        }
    }
    return 0;
}
```

编译之后，我们运行一下来试试。（没说明的话 mytest.cc 就不做改变）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d7b06d206bb045ba9b276ed1be42e6ec.png)


通过观察运行结果我们知道，子进程拿到的只有我们自己定义的环境变量表了。

> 注意：通过上面的运行结果我们可以知道，execle 函数来传递环境变量时，不是新增环境变量，而是覆盖掉之前的环境变量，同理可以推导出其他 exec 族函数中也一样，带``e``的，不是新增，而是覆盖



