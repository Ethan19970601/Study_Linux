#  3种重定向

我们有如下的代码：
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "log.txt"

int main()
{
    close(1); // 关闭标准输出
    int fd = open(FILE_NAME, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    printf("fd:%d\n", fd);
    printf("stdout->fd: %d\n", stdout->_fileno);
    fflush(stdout);
    close(fd);

    return 0;
}
```


运行上面的代码之后，我们发现结果如下：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a23da51da780442c9f354cc1b6c8208c.png)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/66886630673a4328b26f17237fa4c448.png)

在上面的代码执行之前，文件描述符是这样的：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/030a3e715e6f49849fdab29450a16e67.png)

上面的代码执行时：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/efff252d0c744451b39e9e1491897066.png)

然后创建了一个 log.txt 出来，根据文件 fd的分配规则[[文件 fd#📌 fd 的分配规则]]：寻找最小的，没有被使用的数组下标分配给指定的打开文件，这里由于 1 首先被关闭了，所以 1 就是那个最小的没有被使用的数组下标了，它被分配给了 log.txt

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8f6d4ac7c5a840a7ba305f8fd5c7c211.png)

好了，这里在说明一下：`printf`函数是向屏幕上打印的，Linux 下一切皆是文件，也就是说，`printf` 是向显示器文件里写入内容的。`printf `是根据 `stdout `来向显示器文件里写入内容的，而`stdout`这个里面有一个`fileno`,这个 `fileno` 的值是等于 `1` 的，这里也就可解释为什么 `printf`往 `log.txt `里打印内容了，因为此时 `log.txt`的文件描述符是:`1`

综上所诉，重定向的本质就是修改特定文件的数组下标的内容。

## `fflush(stdout)`是什么？

上面的代码，如果我们不加 `fflush`会有什么结果？我们可以试一试。

如果不加`fflush` 我们会发现，屏幕上不会打印任何内容，**log.txt**里也没有内容写入。

这个和我们的一个概念有关：==用户级缓冲区==，语言中提供的缓冲区

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3d57d04b51a84672904ad34a842eab0d.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/e613408892dc4434bd012220d0d955b1.png)

## 输入重定向

我们事先将`log.txt`中的内容修改成123 456，然后再运行下面的程序：

```c
  #include<stdio.h>    
  #include<sys/types.h>    
  #include<sys/stat.h>    
  #include<fcntl.h>    
  #include<string.h>    
  #include<unistd.h>    
      
  #define LOG "log.txt"    
      
  int main()    
  { 
    close(0); 
    int fd = open(LOG, O_RDONLY);
    int a, b;
    scanf("%d %d", &a, &b);
    printf("a = %d, b = %d\n", a, b);
    return 0;
  }


```

原理：
因为我们一开始执行了close(0)关闭文件描述符1对应的文件，其实也就是我们的stdin，那么我们再打开log.txt文件，根据文件描述符的规则：分配的是当前最小的没有被占用的文件描述符！那么我们的log.txt就顺理成章的拿到了fd = 0；这时候printf函数内部肯定是封装了操作系统接口read的，read只会根据文件描述符来区分文件，所以它默认的就是向文件描述符为0的文件中读取，所以就读取到了log.txt中的123 和 456！
## 输出重定向

```c
 #include<stdio.h>
  #include<sys/types.h>
  #include<sys/stat.h>
  #include<fcntl.h>
  #include<string.h>
  #include<unistd.h>
  
  #define LOG "log.txt"
  
  int main()
  {

  int fd1 = open(LOG, O_CREAT | O_WRONLY | O_TRUNC, 0666);
      
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");    
  return 0;    
  }  

```
**运行结果：**
我们发现`printf`函数运行的结果没有出现在屏幕上，而是出现在`log.txt`文件中。
**原理：**
因为我们一开始执行了close(1)关闭文件描述符1对应的文件，其实也就是我们的stdout，那么我们再打开log.txt文件，根据文件描述符的规则：分配的是当前最小的没有被占用的文件描述符。那么我们的log.txt就顺理成章的拿到了fd = 1；这时候printf函数内部肯定是封装了操作系统接口write的，write只会根据文件描述符来区分文件，所以它默认的就是向文件描述符为1的文件中写入，所以就写入到了log.txt中

## 追加重定向

我们只需要修改输出重定向中的代码：在`open`函数的参数中添加上追加的参数即可

```c
  #include<stdio.h>
  #include<sys/types.h>
  #include<sys/stat.h>
  #include<fcntl.h>
  #include<string.h>
  #include<unistd.h>
  
  #define LOG "log.txt"
  
  int main()
  {

  int fd1 = open(LOG, O_CREAT | O_WRONLY | O_APPEND, 0666);  
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");    
    printf("hello \n");                            
    return 0;    
  }  

```

运行上面的代码，我们可以知道，内容是追加输出到文件中的。这就叫做我们的追加重定向。

## 重定向基本原理

根据上面的例子，我们可以知道，上层的fd 不变，底层fd指向的内容在改变，从而导致了我们输入输出的地方的改变，这就是重定向的基本原理

但是，问题来了，你看上面的代码，我们为了实现重定向，还要先`close()`还要指定文件的打开方式，这样也太麻烦了，有没有简单一点的方法，害，还真有！！
![](https://pinoss.com/pdan123/i/pdan1/2024/1/c5.jpg)

![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501101548488.png)
我们的想法是，直接把 3 号数组下标对应的内容直接拷贝到 1 号里面，

![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501101552806.png)

然后我们把之前的 3 号关掉，这样就重定向了呀，本来 1 是指向标准输出的，现在指向了 log.txt了

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/f2c129b1939e4f28b668cd3bdddcbe7c.png)

 上面的操作其实就是文件描述符表级别的数组内容的拷贝，而且，我们也有这样的接口可以实现上面的功能：`dup2` 

```c
int dup2(int oldfd, int newfd);
```

```c
dup2(要拷贝的，目的地) 
```

这个时候出现了 2 个问题
一个上面图所示：


![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501101552806.png)
这个时候我们有 2 个文件描述符指向`log.txt`，即使一个正常使用，一个关了，这个时候也会出问题，所以我们如何在有多个指针指向`log.txt`的时候保证它能够正常工作呢？

所以这个时候我们要引出引用计数的概念:
```c
atomic_long_t f_count;
```

`f_count`就是用来计数的，有几个指针指向它，`f_count`就是 几


以下代码我们什么也没有干，只是将文件打开后，关闭了而已。
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "log.txt"

int main()
{
	// 打开了
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }
	// 打印
    printf("hello printf\n");
    fprintf(stdout, "hello fprintf\n");
	// 关闭
    close(fd);
    return 0;
}
```

 💥 怕你忘记，这里写一下 open 函数里面的参数的意思：
 打开文件的标志:
- `O_WRONLY`：以只写模式打开文件。如果文件已存在，可以写入数据；如果文件不存在，打开操作将失败。
- `O_CREAT`：如果文件不存在，则创建它。通常与 `O_WRONLY` 或 `O_RDWR` 一起使用。
- `O_TRUNC`：如果文件已存在，并且以写模式打开，则将文件长度截断为 0。这意味着文件中的原有内容将被删除。

文件权限:
- `0666`：这是一个八进制数，表示文件的权限。八进制数 `0666` 对应的权限是：
    - `6`（即 `110` 二进制）表示读写权限。
    - 第一个 `6` 表示文件所有者（owner）的权限。
    - 第二个 `6` 表示文件所属组（group）的权限。
    - 第三个 `6` 表示其他用户（others）的权限。
    因此，`0666` 表示文件所有者、所属组和其他用户都有读写权限。

运行上面的程序，我们可以看到：
![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501101647463.png)

好了，基于上面的程序，我们要完成输出重定向：
我们只需要在上面的程序中加入`dup2`就行了,用 dup2 就可以解决多个指针同时指向 log.txt 还能让它正常工作的问题

```c
dup2(fd, 1); 
```

完整代码：
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "log.txt"

int main()
{
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    dup2(fd, 1); // <-----------------------------------------------------
    printf("hello printf\n");
    fprintf(stdout, "hello fprintf\n");

    close(fd);

    return 0;
}
```

运行之后，就没有向屏幕中输出了，而是向 log.txt这个文件中输入，因为我们改了输出的文件
![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501101655732.png)

# 文件描述符`2` 是干什么的？

我们知道，文件描述符 2 对应的是标准错误，标准错误是干什么的，为什么要存在标准错误呢？而且标准错误也是指向的显示器

假设我们有如下程序：
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "log.txt"

int main()
{
    fprintf(stdout, "hello stdout\n"); //标准输出
    fprintf(stderr, "hello stderr\n"); // 标准错误
    return 0;
}
```

运行结果如下：
![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501102141636.png)


这个时候，我们做一个实验，我们重定向一下，把内容输出到 log.txt中：
![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501102143856.png)

我们发现结果很神奇：1. stderr 还是往显示器上打印的   2. stdout 的确是往 log.txt 中打印的

为啥？❓
因为，我们重定向的时候仅仅只是将标准输出重定向了，并没有将标准错误重定向。

那我们非要将标准错误也重定向到`log.txt`中咋办？

我们可以这样写：
```bash
./myfile > log.txt 2>&1
```

![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501102153166.png)

![](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501102157542.png)


![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/202501102202838.png)

还有一件事，其实：`./myfile > log.txt` 是一种简写，完整的写法是:`./myfile 1 > log.txt`。

这样一来，我岂不是可以这样操作：
```c
./myflie 1>log001.txt 2>log002.txt
```
就是把这个内容，一个放进 1 号里，一个放进 2 号里

回到正题，那为什么要有标准错误呢？

在我们写程序的时候，遇到报错是在正常不过的了，但是如果这个程序也有普通的打印任务，比如：要在屏幕上打印一个`hello word`，这个时候，我们可以将`hello word` 作为普通输出打印进 `log001.txt`中，将这个程序的错误输出放入`log002.txt` 中，这就是标准错误存在的意义。


# 📌缓冲区

## 🎹预备知识

什么是缓冲区？
> 其实就是一部分的内存

这一部分的内存是由谁提供的？
>  比如，我们自己写代码的时候定义的char bufer 或是 char 数组[ ] ，其实这就是我们自己定义的用户缓冲区。如果这个缓冲区是由c语言提供的，这就叫做c标准库提供的缓冲区 ，如果是操作系统提供的，那么就叫做操作系统缓冲区。我们可以简单的理解为都是`malloc` 出来的。
>

 为什么要有缓冲区？
 > 我们有这样的一段代码样例：
 > ``printf("hello linux");``   
 > ``sleep(3);`` 
 > 在sleep期间，屏幕上是不会打印 hello linux 的，但是c语言是从上往下执行的，所以当在执行sleep(3) 的时候，printf 语句是肯定已经执行过了，这个时候 hello linux 就存在于缓冲区里。 

缓冲区的优点：
> 缓冲区的作用是提高使用者的效率
> 因为有缓冲区的存在，我们可以积累一部分在统一发送
> 提高发送的效率


 缓冲区因为能够暂存数据，必定要有一定的刷新方式：
 1. 无缓冲（立即刷新）
 2. 行缓冲（行刷新）
 3. 全缓冲（缓冲区满了再刷新）

上面的刷新方式是一般的情况，以下是特殊情况下的刷新方式：
1. 强制刷新
2.  进程退出的时候，一般要进行刷新缓冲区

> 一般对于显示器，我们采用的刷新策略是：行刷新(行缓冲)
> 对于磁盘上的文件，我们采取的刷新策略是：全缓冲（缓冲区满再刷新）



## 🎹介绍一个样例

现在，我们要准备以下几个文件：
```makefile
￼k``makefile``:
```makefile
myfile:myfile.c
	gcc -o $@ $^ 
.PHONY:clean
clean:
	rm -rf myfile

```

`myfile.c:`
```c
 #include <stdio.h>
#include <unistd.h>
kude <string.h>
#include <unistd.h>

int {
  fpkmain()
{
  fprintf(stdout,"C:hello fprintf\n");
  pputs("C: hello fputs\n",stdout); 
  ckrintf("C: hello printf\n");
  fputs("C: hello fputs\n",stdout); 
  const char *str = "system call: hello wirte\n";
  write(1,str,strlen(str));
 return 0;
}


```

运行的操作：
![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250113203734589.png)


上面，我们写了 4 个向显示器打印的语句。 

接下来，我们输出重定向到`log.txt` 中；
![ik'kmage.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250113204114271.png)

上面的操作做完之后，现在 我们来开始试验，现在，我们在`myfile.c` 中加上一行代码；
```c
#include <stdio.h>
#include <string.h>
#i
inknclude <unistd.h>

int main()
{
  fprintf(stdout,"C:hello fprintf\n");
  printf("C: hello printf\n");
  fputs("C: hello fputs\n",stdout); 
  const char *str = "system call: hello wirte\n";
  write(1,str,strlen(str));
  fork(); // <---------------------------------------- 注意fork的位置
 return 0;
}

```

加上这一句`fork();` 之后，我们在把之前的执行流程重新走一遍看看有什么不同。
!k
![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250113204940720.png)


> 当我们直接向显示器打印的时候，缓冲区采取的刷新方式是：行刷新，而且上面的代码中，我们每一个打印的字符的后面都是有`\n` 的，这说明 fork() 执行之前，上面的那些打印的语句都被刷新过了也包括 system call ,所以是一样的

> `\n` 是换行，显示器是行刷新，所以有`\n` 就会进行行刷新


![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250113205057292.png)

观察上面打印的结果，我么发现，只有C语言的接口被打印了2次，而系统调用接口只被打印了1次

这样的结果一定与我们在代码中添加的`fork()` 有关

问题是，我们的`fork()` 是放在代码的最后的呀，如果你这个`fork()`放在的是代码的最前面，这都好解释，因为，fork之后创建子进程，父子进程代码共享，会打印2次，你这里的fork 是放在代码最后的呀，fork() 之后啥都没有了；它咋还打印了呢？

> 当我们使用`./myfile > log.txt` 是**输出重定向** 本质是向==磁盘上的文件== 进行写入（不再是显示器了），而磁盘上文件默认的刷新策略是：全缓冲刷新 （缓冲区满了再刷新），全缓冲就意味着缓冲区变大，上面我们打印的语句不足以把缓冲区填满，这就意味着，fork() 执行的时候，数据依旧在缓冲区中。

> 我们发现，系统调用在上面进行的操作过程中都只打印了一次，这说明我们目前系统调用和缓冲区是没有关系的,是和 C 语言有关的（通过我们上面的操作推出），这就说明，我们日常用的最多的缓冲区是c/c++ 提供的语言级别的缓冲区








## 🎹理解样例



## 🎹用户缓冲区 vs 内核缓冲区