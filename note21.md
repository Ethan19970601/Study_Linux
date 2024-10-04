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



---

# 🏷️ 材料准备

创建一个文件:``myshell.c``:
```c
#include <stdio.h>

int main()
{

    return 0;
}
```

创建一个 ``Makefile`` 文件，文件内容如下：

```Makefile
  1 mybash:myshell.c

  2   g++ -o $@ $^ -std=c++11                                                   

  3 .PHONY:clean             

  4 clean:           

  5   rm -f mybash
```


# 🏷️ 打印提示符，获取用户命令字符串

## 📌 （打印提示符）填写：用户名，主机名，当前所在的工作目录

我们打开终端，我们会看到这样的东西：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/37fb94effab0420d9cde1e7ae0dfad1d.png)


所以我们自己定义的 shell 文件系统也要获取这些东西，我们可以写一些函数：

```c
// 获取当前的用户名
const char* getUsername()
{}

// 获取当前的主机名
const char* getHostname()
{}

// 获取当前的工作目录
const char* getCwd()
{}
```


使用``getenv()`` 这个函数可以获取当前的环境变量，头文件是``stdlib.h`` ，所以我们可以如下来写：

```c
#include <stdio.h>
#include <stdlib.h>

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name; // 获取成功就返回名字
    else
        return "none"; // 获取失败就返回 none
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int main()
{
    printf("[%s@%s %s]￥\n", getUsername(), getHostname(), getCwd()); // 这里的 printf 输出是仿造终端那种格式的，我把 $ 换成了人民币的 ￥，你可以自己换成其他的
    return 0;
}
```

``make`` 之后 ，运行一下：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7f35774f2eda41e283c5813586ba1f5c.png)
但是我们发现一个问题，我们运行之后程序就结束了，按理说正常的 shell 是可以输入命令的，我们也想输入命令，所以我们改一下我们的代码：

我们加入以下命令：
```c
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
 char usercommand[NUM]; // 用来存储用户输入的命令
scanf("%s", usercommand); // 用户输入命令
```

合在一起是这样的：
```c
#include <stdio.h>
#include <stdlib.h>
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节


// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd()); 

    scanf("%s", usercommand); // 用户输入命令
    return 0;
}
```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/27bf660d997246e9bfc69491ae5fa507.png)
## 📌 （获取用户命令字符串）向我们自定义的 bash 中输入命令

接着，我们可以用`` echo`` 来打印一下我们输入的命令是怎么样的 
```c
printf("echo:\n%s", usercommand);  // 我们用这条语句来显示一下输入的命令到底是怎么样的
```

在上面的代码中加上这一行：
```c
#include <stdio.h>
#include <stdlib.h>
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节


// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());

    scanf("%s", usercommand); // 用户输入命令
    /******************************************************************************/
    printf("echo:\n%s", usercommand);  // 我们用这条语句来显示一下输入的命令到底是怎么样的
    /******************************************************************************/
    return 0;
}
```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/798fa2d6df2649d6b3fee2f0a3c1a070.png)


因为，我们是用的``scanf`` ,它遇到空格就会停下来。所以我们要换一个,使用 ``fgets()`` 函数，头文件是：``stdio.h``;

```c
#include <stdio.h>
char *fgets(char *str, int num, FILE *stream);
```

**参数:**
- `str`：指向字符数组的指针，用于存储从文件流中读取的字符串。
- `num`：指定最多读取的字符数，包括最后的空字符（`\0`）。
- `stream`：指向 `FILE` 结构的指针，指定了要读取的文件流。

**返回值:**
- 如果成功读取，`fgets` 返回一个指向 `str` 的指针。(获取成功就是你输入的字符串的起始地址)
- 如果遇到错误或文件结束符（EOF），返回 `NULL`。（获取失败就返回 NULL）

 
 🧲 知识补充：
C 语言会默认打开三个输入输出流，分别叫做：``stdin``, ``stdout``, ``stderr``; 它们的类型都是 ``FILE*``  
 ``stdin``    :  键盘
 ``stdout``  :  显示器
 ``stderr ``:  显示器

 有了上面的知识，所以我们将``scanf()`` 函数修改一下，修改成这样：
 ```c
  fgets(usercommand, sizeof usercommand, stdin);
```

1. `sizeof usercommand`：用于计算 `usercommand` 数组的大小（即数组中元素的数量，通常以字节为单位）。在这里，它用来确定 `fgets` 函数可以读取的最大字符数。注意，`sizeof` 后面紧跟变量名，不需要括号。
2. `stdin`：是 C 语言标准库提供的一个全局变量，代表标准输入流。它通常与键盘输入关联。`stdin` 是 `FILE` 类型的指针，指向标准输入设备。

关于 `sizeof` 操作符的使用，它是一个编译时运算符，用于获取变量或类型所占用的字节数。它返回的是一个常量值，所以不需要括号。括号通常用于 `sizeof` 操作符的语法是当它用于数据类型本身，而不是变量的时候，例如 `sizeof(int)`。

这个时候，我们的代码是这样的：
```c
#include <stdio.h>
#include <stdlib.h>
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(usercommand, sizeof usercommand, stdin);
    if (r == NULL)
        return 1;                     // 获取失败
    printf("%s\n", usercommand); // 我们用这条语句来显示一下输入的命令到底是怎么样的
    return 0;
}
```

运行之后这里有一个小的细节：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d9e0cb7beb0c4311a4b597aeea09135a.png)

为了避免这样的情况，我们可以再代码中加入下面这样一行👇🏻：

```c
#include <string.h>

usercommand[strlen(usercommand) - 1] = '\0'; // 将倒数第二个字符变成‘\0’,这样就不会读取到我们最后的那个回车换行了
```

我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(usercommand, sizeof usercommand, stdin);
    if (r == NULL)
        return 1;   // 获取失败

    usercommand[strlen(usercommand) - 1] = '\0';
    printf("%s\n", usercommand); // 我们用这条语句来显示一下输入的命令到底是怎么样的
    return 0;
}

```

问一下：我们这样写会不会越界呢？
```c
usercommand[strlen(usercommand) - 1] = '\0';
```
万一``usercommand`` 用 strlen 求长度之后是 0 ， 那再减 1 不就越界了吗？
答案是：不会越界。
因为：你的``usercommand`` 的长度不可能是 0 ，因为你就算什么也不输入，最后也要输入个 ``回车`` 来运行这个命令，所以``usercommand`` 中肯定有一个``回车``的。长度就不可能是 0 。

我们把上面的代码封装一下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int getUserCommand(char *command, int num) // 获取用户输入的命令
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令
    printf("%s", usercommand);                            
    return 0;
}
```



# 🏷️ 分割用户输入的字符串

❓什么意思呢？
假设用户输入的是：``ls -a -l`` ，我们要把这个字符串拆解为:``“ls”``, ``"-a"`` , ``"-l"``。

### **创建数组:**
我们要把拆分之后的这些字符串存起来，所以我们要创建一个数组：
```c
#define SIZE 64
char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL
```
并且我们希望这个数组以 NULL 结尾。
### **如何分割这个字符串呢？：**

##### 函数：``strtok``

使用函数：``strtok()``。头文件：``string.h``

```c
char *strtok(char *str, const char *delim);
```

 **参数**

- `str`：指向要分割的字符串的指针。在第一次调用 `strtok` 时，这个参数必须提供要分割的字符串；在随后的调用中，应该传递 `NULL`。
- `delim`：指向分隔符集的指针，指定了用于分割字符串的一组字符。

**返回值**

- `strtok` 返回一个指向下一个标记的指针。如果没有更多的标记或遇到字符串结束，返回 `NULL`。

##### 为了使用这个函数，我们要定义一个分隔符来传给它

```c
#define SEP " " // 注意我们这里引号里的是空格
```

##### 使用函数

```c
    argv[argc++] = strtok(usercommand, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (argv[argc++] = strtop(NULL, SEP)); // 第二次传参和之后的传参就只能传：NULL
```

让我们逐步分析这段代码：

1. `argv[argc++] = strtok(usercommand, SEP);`
    
    - `strtok(usercommand, SEP)`：调用 `strtok` 函数，将 `usercommand` 字符串按照 `SEP` 中定义的分隔符进行分割，并返回第一个标记。
    - `argv[argc++]`：将分割得到的第一个标记赋值给 `argv` 数组的当前位置，并且将 `argc`加 1。
2. `while (argv[argc++] = strtok(NULL, SEP));`
    - 这个循环继续调用 `strtok` 函数，但是这次传入的第一个参数是 `NULL`。在 `strtok` 中，如果第一个参数是 `NULL`，它会接着处理上次调用时使用的字符串（在这个例子中是 `usercommand`）。
    - `strtok(NULL, SEP)`：返回下一个标记，直到没有更多的标记可以返回，这时 `strtok` 会返回 `NULL`。(所以当 strtok 返回 NULL 的时候就意味着字符串已经被分割完了，并且这个 NULL 也会被赋值给 argv数组（满足了我们的目标：argv 这个数组最后以 NULL 结尾）)
    - `argv[argc++]`：将每个新的标记存储在 `argv` 数组的下一个位置，并将 `argc` 加 1。
    - 循环继续直到 `strtok` 返回 `NULL`，这意味着所有的标记都已经被提取。


在上面的代码中，`while` 循环的结束条件是 `strtok` 函数返回 `NULL`。这个行为是由 `strtok` 函数的特性决定的。
`strtok` 函数在每次调用时会返回下一个标记（token），直到没有更多的标记可以返回。当 `strtok` 遇到字符串的结尾或者只包含分隔符的字符串时，它会返回 `NULL`。在 `while` 循环的条件中，`strtok` 的返回值被赋值给 `argv[argc++]`，然后条件检查这个赋值的结果是否为 `NULL`。
这里是详细的过程：
1. 首次调用 `strtok(usercommand, SEP)` 返回第一个标记，并将这个标记赋值给 `argv[argc]`，然后 `argc` 自增。
    
2. 在随后的 `while` 循环中，`strtok(NULL, SEP)` 被调用，它继续处理上次调用 `strtok` 时留下的字符串。每次调用都会返回下一个标记，直到没有更多的标记。
    
3. 每次 `strtok` 返回一个非 `NULL` 值时，这个值都会被赋值给 `argv[argc]`，然后 `argc` 自增，循环继续。
    
4. 当 `strtok` 返回 `NULL` 时，`while` 循环的条件不再满足（因为 `NULL` 等价于逻辑假），循环结束。
    
`while` 循环结束的原因是因为 `strtok` 已经读取了所有的输入字符串，并且没有更多的标记可以返回。这通常意味着已经到达了字符串的末尾，或者字符串中只剩下了分隔符。



### 我们要用一个变量来存储分割好之后的子串的个数
这个变量就是：argc
```c
int argc = 0;
```


#### 我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

int getUserCommand(char *command, int num) // 获取用户输入的命令
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL
    int argc = 0;              // 用来存储分割好之后的字串的个数

    /********************************************************************************* */
    /******                  1.  打印提示符&&获取用户命令字符串                  ********* */
    /******************************************************************************* */

    getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

    /********************************************************************************* */
    /******                  2.       分割命令                                   ****** */
    /******************************************************************************** */
    argv[argc++] = strtok(usercommand, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (argv[argc++] = strtop(NULL, SEP))
        ;

    // 验证一下我们分割对没有：
    for (int i = 0; argv[i]; i++)
    {
        printf("%d:%s\n", i, argv[i]);
    }

    /******************************************************************************** */
    /******                        3.   执行对应的命令                        ********* */
    /******************************************************************************* */
    printf("%s", usercommand); // 我们用这条语句来显示一下输入的命令到底是怎么样的
    return 0;
}
```

我们运行一下： 
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/196eb98b0bc14af4aae4aae51144ca53.png)

##### 我们把这个切割字符串的功能封装一下：

```c
// 字符串的切割
void commandSplit(char* in, char* out[])
{
    int argc = 0;              // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;

    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
}
```

我们在修改一下上面的代码，因为 for 循环只是我们用来验证分割对没有的，我们只有在调试的时候才会用到 for循环里面的代码，所以我们可以加上条件编译

```c
// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}
```

`#ifdef` 和 `#endif` 是 C 语言预处理器指令，用于条件编译。这些指令允许你根据是否定义了特定的宏来决定是否编译代码的某个部分。

** `#ifdef Debug`**

`#ifdef` 是 "if defined" 的缩写。`#ifdef Debug` 检查是否定义了名为 `Debug` 的宏。如果 `Debug` 已经被定义（通常是在编译时通过编译器选项定义的），则编译器会编译 `#ifdef` 和 `#endif` 之间的代码。

** `#endif`**

`#endif` 表示条件编译的结束。它标志着 `#ifdef` 或其他条件编译指令（如 `#if`, `#ifndef`, `#elif`）开始的条件编译代码块的结束。

在上面我们的代码中：
```c
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
```
如果 `Debug` 宏被定义，那么这段代码将被编译和执行，用于打印出分割后的字符串数组 `out` 中的每个元素。这通常用于调试目的，以确保字符串被正确分割。
如果没有定义 `Debug` 宏，那么 `#ifdef` 和 `#endif` 之间的代码将不会被编译，也不会出现在最终的执行程序中。这样做可以避免在发布版本的程序中包含调试代码，这样可以减少程序的大小和提高性能。

如果我们以后要调试它的话，只需要把``Debug`` 定义出来就行了，像这样：
```c
#define Debug 1
```
1. `#define`：指令告诉预处理器定义一个宏。
2. `Debug`：是要定义的宏的名称。
3. `1`：是宏的值。在宏定义中，通常使用 `1` 或者不赋值（即只有 `#define Debug`），因为宏定义本身仅仅是一个标识符的替换，而不是一个条件表达式。
当我们在代码中使用 `#ifdef Debug` 时，预处理器会检查是否定义了 `Debug` 宏。由于 `Debug` 被定义为 `1`，它被视为真值（在 C 语言中，除了 `0` 以外的所有值都被视为真值），因此 `#ifdef Debug` 内的代码块将被包含在编译过程中

##### 如果我们要调试的话，我们需要定义 Debug 我们的代码是这样的：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
#define Debug 1
// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

    /********************************************************************************** */
    /******                  1.  打印提示符&&获取用户命令字符串                   ********* */
    /******************************************************************************** */

    getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

    /******************************************************************************** */
    /******                  2.       分割命令                               ********* */
    /******************************************************************************* */

    commandSplit(usercommand, argv);
    /******************************************************************************** */
    /******                        3.   执行对应的命令                        ********* */
    /******************************************************************************** */

    return 0;
}
```




# 🏷️ 执行对应的命令

我们创建子进程来执行我们对应的命令，所以我们要用``fork()`` 函数，头文件：``unistd.h`` 

```c
// 3.执行对应的命令
    pid_t id = fork();
    if (id < 0)
        return 1;     // 如果创建子进程失败就返回 1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp(),把用户输入的命令传进去。

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }
```

我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// #define Debug 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

int main()
{
    char usercommand[NUM]; // 用来存储用户输入的命令

    char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

    //  1.打印提示符&&获取用户命令字符串
    getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

    // 2.分割命令
    commandSplit(usercommand, argv);

    // 3.执行对应的命令
    pid_t id = fork();
    if (id < 0)
        return 1;     // 如果创建子进程失败就返回 1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp()

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }
    return 0;
}
```

我们运行上面的代码：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/00b1cfc4649542ad91aea84e94c32d43.png)

哎！ 成功了。

但是！我们的 mybash 只能执行一次而系统的bash是执行完这个命令之后还可以执行下一个，所以我们也要优化一下之前的代码，让我们的 mybash 也能周而复始的执行。

这简单呀：外面套一个死循环就行了：
```c
while (1)
{
	// 我们之前的的代码逻辑
}
```

整体代码如下：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// #define Debug 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

int main()
{
    while (1)
    {
        char usercommand[NUM]; // 用来存储用户输入的命令

        char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

        //  1.打印提示符&&获取用户命令字符串
        getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

        // 2.分割命令
        commandSplit(usercommand, argv);

        // 3.执行对应的命令
        pid_t id = fork();
        if (id < 0)
            return 1;     // 如果创建子进程失败就返回 1；
        else if (id == 0) // id == 0 ,说明是子进程
        {
            // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
            execvp(argv[0], argv); // 我们使用接口 execvp()

            exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
        }
        else // 这里是父进程
        {
            // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
            pid_t rid = waitpid(id, NULL, 0);
            if (rid > 0) // 说明等待成功了
            {
            }
        }
    }
    return 0;
}
```

看，我们就能一直输入命令了：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/49f2d439d0404f86b7165d63e91992b4.png)

好了，我们再把执行命令的代码封装一下：
```c
// 执行用户输入的命令
int execute(char *argv[])
{
    pid_t id = fork();
    if (id < 0)
        return -1;    // 如果创建子进程失败就返回 -1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp()

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }

    return 0;
}
```

我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// #define Debug 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

// 执行用户输入的命令
int execute(char *argv[])
{
    pid_t id = fork();
    if (id < 0)
        return -1;    // 如果创建子进程失败就返回 -1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp()

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }

    return 0;
}

int main()
{
    while (1)
    {
        char usercommand[NUM]; // 用来存储用户输入的命令

        char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

        //  1.打印提示符&&获取用户命令字符串
        getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

        // 2.分割命令
        commandSplit(usercommand, argv);

        // 3.执行对应的命令
        execute(argv);
    }
    return 0;
}
```


# 🏷️ 有一批命令要由父进程来执行----- 内建命令

##### 什么是内建命名：

内建命令就是``bash`` 自己执行的，类似于自己内部的一个函数。

 

上面的代码我们已经可以实现一个shell 的一些功能了，但是我们会发现这样的一些问题，比如：你运行``mybash`` 之后，可以使用：``pwd``, ``ls`` 之类的命令，但是当我们想做路劲切换的时候缺发现不行，比如：``cd ..`` 。
因为我们代码中的命令都是 ``fork()``了一个子进程出来执行的。但是如果我们想做路径切换，切换的是当前这个 mybash 的路径，所以要由父进程来执行。

##### 所以我们要区分一下，用户输入的命令是不是内建命令（build-in）

```c
// 3. 检查对应的命令是不是内建命令, 用 1 表示是内建命，用 0 表示不是内建命令
int doBuildin(char *argv[])
{
    if (strcmp(argv[0], "cd") == 0)
    {
        char *path = NULL;   // 如果我们使用 cd 命令，那么后面是有路劲的，我们这里用 path来表示
        if (argv[1] == NULL) // 如果用户没有输入路劲，那我们默认是当前路劲："."
        {
            path = ".";
        }
        else
        {
            path = argv[1]; // else 的话就说明用户输入了路径
        }

        cd(path);
        return 1; // 是内建命令我们 return 1；
    }
    return 0; // 不是内建命令我们就return 0;
}
```

##### 如果是内建命令``cd`` ，我们如何进行路径切换

这里我们要使用一个新的函数：``int chdir(const char *path);`` 头文件：``"stdio.h"``

**函数原型**

```c
int chdir(const char *path);
```

**参数**
- `path`：指向一个以 null 结尾的字符串，表示要更改到的目标目录的路径。

**返回值**
- 如果函数成功执行，返回 `0`。
- 如果函数执行失败，返回 `-1` 并设置 `errno` 以指示错误原因。

**行为**：
`chdir` 函数将当前工作目录更改为 `path` 指定的目录。如果 `path` 是一个绝对路径，它将直接更改到该路径所表示的目录。如果 `path` 是一个相对路径，它将更改到相对于当前工作目录的路径所表示的目录。

```c
// 路径切换函数
void cd(const char *path)
{
    // 这里我们要使用一个新的函数：int chdir(const char *path); 头文件："stdio.h"
    // chdir 函数将当前工作目录更改为 path 指定的目录。
    //如果 path 是一个绝对路径，它将直接更改到该路径所表示的目录。
    //如果 path 是一个相对路径，它将更改到相对于当前工作目录的路径所表示的目录
    chdir(path);
}
```

我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// #define Debug 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

// 执行用户输入的命令
int execute(char *argv[])
{
    pid_t id = fork();
    if (id < 0)
        return -1;    // 如果创建子进程失败就返回 -1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp()

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }

    return 0;
}

// 路径切换函数
void cd(const char *path)
{
    // 这里我们要使用一个新的函数：int chdir(const char *path); 头文件："stdio.h"
    // chdir 函数将当前工作目录更改为 path 指定的目录。
    //如果 path 是一个绝对路径，它将直接更改到该路径所表示的目录。
    //如果 path 是一个相对路径，它将更改到相对于当前工作目录的路径所表示的目录
    chdir(path);
}

// 3. 检查对应的命令是不是内建命令, 用 1 表示是内建命，用 0 表示不是内建命令
int doBuildin(char *argv[])
{
    if (strcmp(argv[0], "cd") == 0)
    {
        char *path = NULL;   // 如果我们使用 cd 命令，那么后面是有路劲的，我们这里用 path来表示
        if (argv[1] == NULL) // 如果用户没有输入路劲，那我们默认是当前路劲："."
        {
            path = ".";
        }
        else
        {
            path = argv[1]; // else 的话就说明用户输入了路径
        }

        cd(path);
        return 1; // 是内建命令我们 return 1；
    }
    return 0; // 不是内建命令我们就return 0;
}
int main()
{
    while (1)
    {
        char usercommand[NUM]; // 用来存储用户输入的命令

        char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

        //  1.打印提示符&&获取用户命令字符串
        getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

        // 2.分割命令
        commandSplit(usercommand, argv);
        // 3. 检查对应的命令是不是内建命令
        int n = doBuildin(argv);
        if (n) // 如果它是内建命令，我们执行它之后，跳过这个循环剩余的部分，不用再往后走了
            continue;
        // 4.执行对应的命令
        execute(argv);
    }
    return 0;
}
```


运行上面的代码之后，我们终于可以在自定义的 shell中 进行路径切换了

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3d2bfeb6e7e04899aef021c0ef42fb50.png)


所以我们要修改一下我们的代码：

``char cwd[1024] ``这里面存放的是当前工作目录，是环境变量，环境变量不能是存放在临时的空间中，所以我们把它定义成全局变量
```c
char cwd[1024]; 
// 路径切换函数
void cd(const char *path)
{
    chdir(path);
    char tmp[1024];
    getcwd(tmp, sizeof(tmp));
    sprintf(cwd, "PWD=%s", tmp); 
    putenv(cwd);
}
```

 对上面代码的解释：

1. **定义全局变量 `cwd`**:
   ```c
   char cwd[1024];
   ```
   这里定义了一个全局字符数组 `cwd`，用于存储当前工作目录的路径。由于环境变量需要持久存储，所以 `cwd` 被定义为全局变量，以便在程序的任何地方都可以访问和修改它。

2. **定义 `cd` 函数**:
   ```c
   void cd(const char *path)
   ```
   这是一个自定义的 `cd` 函数，接受一个指向字符串的指针 `path` 作为参数，该参数是要切换到的目标目录路径。

3. **更改当前工作目录**:
   ```c
   chdir(path);
   ```
   使用 `chdir` 函数尝试更改当前工作目录到 `path` 指定的目录。如果更改失败，`chdir` 会返回 `-1`.

4. **获取当前工作目录**:
   ```c
   char tmp[1024];
   getcwd(tmp, sizeof(tmp));
   ```
   声明一个临时数组 `tmp`，然后使用 `getcwd` 函数获取当前工作目录的完整路径，并将其存储在 `tmp` 中。

5. **更新全局变量 `cwd`**:
   ```c
   sprintf(cwd, "PWD=%s", tmp); 
   ```
   使用 `sprintf` 函数将 `tmp` 中的路径格式化为一个字符串，并存储在全局变量 `cwd` 中。格式化的字符串是 "PWD=路径"，其中 `PWD` 是一个常用的环境变量，用于存储当前工作目录的路径。

6. **更新环境变量**:
   ```c
   putenv(cwd);
   ```
   使用 `putenv` 函数更新环境变量。`putenv` 会修改或添加一个环境变量，这里它会更新 `PWD` 环境变量为 `cwd` 中存储的路径。这样，任何依赖于 `PWD` 环境变量的程序都会看到更新后的路径。

这段代码的 `cd` 函数不仅更改了当前工作目录，还更新了 `PWD` 环境变量，以便命令行提示符可以显示正确的当前工作目录路径。

我们现在的代码：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 64
#define NUM 1024 // 这里定义一下，我们的命令行最大的输入是 1024 个字节
#define SEP " "
// #define Debug 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
char cwd[1024]; // 这里面存放的是当前工作目录，是环境变量，环境变量不能是存放在临时的空间中，所以我们把它定义成全局变量

// 获取当前的用户名
const char *getUsername()
{
    const char *name = getenv("USER");
    if (name)
        return name;
    else
        return "none";
}

// 获取当前的主机名
const char *getHostname()
{
    const char *hostname = getenv("HOSTNAME");
    if (hostname)
        return hostname;
    else
        return "none";
}

// 获取当前的工作目录
const char *getCwd()
{
    const char *cwd = getenv("PWD");
    if (cwd)
        return cwd;
    else
        return "none";
}

// 获取用户输入的命令
int getUserCommand(char *command, int num)
{

    printf("[%s@%s %s]￥", getUsername(), getHostname(), getCwd());
    char *r = fgets(command, num, stdin);
    if (r == NULL)
        return 1; // 获取失败

    command[strlen(command) - 1] = '\0';
    return 0;
}

// 字符串的切割
void commandSplit(char *in, char *out[])
{
    int argc = 0;                  // 用来存储分割好之后的字串的个数
    out[argc++] = strtok(in, SEP); // 第一次传参:传我们要分割的那个字符串，
    while (out[argc++] = strtok(NULL, SEP))
        ;
#ifdef Debug
    // 验证一下我们分割对没有：
    for (int i = 0; out[i]; i++)
    {
        printf("%d:%s\n", i, out[i]);
    }
#endif
}

// 执行用户输入的命令
int execute(char *argv[])
{
    pid_t id = fork();
    if (id < 0)
        return -1;    // 如果创建子进程失败就返回 -1；
    else if (id == 0) // id == 0 ,说明是子进程
    {
        // exec commmand, 让子进程进行程序替换，目的是执行用户输入的命令
        execvp(argv[0], argv); // 我们使用接口 execvp()

        exit(1); // 子进程执行完成之后，我们不希望它继续往下走，所以我们用 exit 来退出
    }
    else // 这里是父进程
    {
        // 父进程的作用是等待子进程的退出状态，所以我们要使用：waitpid,头文件是："sys/types.h" 和 “sys/wait.h”
        pid_t rid = waitpid(id, NULL, 0);
        if (rid > 0) // 说明等待成功了
        {
        }
    }

    return 0;
}

// 路径切换函数
void cd(const char *path)
{
    chdir(path);
    char tmp[1024];
    getcwd(tmp, sizeof(tmp));
    sprintf(cwd, "PWD=%s", tmp); 
    putenv(cwd);
}

// 3. 检查对应的命令是不是内建命令, 用 1 表示是内建命，用 0 表示不是内建命令
int doBuildin(char *argv[])
{
    if (strcmp(argv[0], "cd") == 0)
    {
        char *path = NULL;   // 如果我们使用 cd 命令，那么后面是有路劲的，我们这里用 path来表示
        if (argv[1] == NULL) // 如果用户没有输入路劲，那我们默认是当前路劲："."
        {
            path = ".";
        }
        else
        {
            path = argv[1]; // else 的话就说明用户输入了路径
        }

        cd(path);
        return 1; // 是内建命令我们 return 1；
    }
    return 0; // 不是内建命令我们就return 0;
}
int main()
{
    while (1)
    {
        char usercommand[NUM]; // 用来存储用户输入的命令

        char *argv[SIZE] = {NULL}; // 分配SIZE个指针，初始值均为NULL

        //  1.打印提示符&&获取用户命令字符串
        getUserCommand(usercommand, sizeof(usercommand)); // 获取用户输入的命令

        // 2.分割命令
        commandSplit(usercommand, argv);
        // 3. 检查对应的命令是不是内建命令
        int n = doBuildin(argv);
        if (n) // 如果它是内建命令，我们执行它之后，跳过这个循环剩余的部分，不用再往后走了
            continue;
        // 4.执行对应的命令
        execute(argv);
    }
    return 0;
}
```

