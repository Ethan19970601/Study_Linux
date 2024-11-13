
# 🏷️ 预备工作

❓ 当我们在系统中创建一个空文件时，这个文件要不要占据磁盘的空间（注意是空文件哟）

答案：当然是要占据磁盘的空间的，文件不仅只包括内容，还有它的属性呀，就是创建时间，用户是谁，修改时间之类的，所以即便这个文件是一个空文件，但它也有属性呀，所以也要占据空间

> 文件 = 内容 + 属性

## 📌 所有对文件的操作，无非就是两种：
1. 对内容的操作
2. 对属性的操作

## 📌 文件的内容是数据，文件的属性也是数据，我们存储文件必须把文件的内容和属性都存储

## 📌 我们如果要访问一个文件，我们要先打开它，问题来了 ❓：
1. 我们指的是谁？
2. 文件打开前是什么意思?
3. 文件打开之后是什么意思？

- 1. 这里的我们指的是==进程==，不是你自己哈。比如你在你写的程序之中写了 `fopen` 这个函数来打开一个文件，但是只有这个程序编译形成可执行程序之后才可以去执行这个`fopen`的操作，一个被加载到内存的可执行程序叫什么呀？—— 不就是==进程==吗？
- 2. 文件打开之前，就是==一个普通的磁盘文件==，是在磁盘上面的
- 3. 打开后这个文件，其实上是将这个文件加载到内存 

## 📌 一个进程可以打开多个文件吗？ 多个进程可以打开多个文件吗？

- 一个进程可以打开多个文件，多个进程也可以打开多个文件

加载到内存中被打开的文件，可能存在多个

文件本来是在磁盘中的，将文件从磁盘中加载到内存里一定要涉及到访问磁盘设备，但是磁盘是外设，把磁盘里的数据加载到内存中的这个工作只能是由操作系统来做

## 📌 操作系统在运行的时候可能会打开很多个文件，那么操作系统是如果来管理这些文件的呢？

由我们之前学习的知识可知，如果涉及到管理，一般的步骤是：**先描述在组织**

### ✏️先描述：
一个文件要被打开，一定要在内核中形成被打开的文件对象
类似这样：
```c
struct xxxx
{
	// 文件的属性
	xxxxx
	struct xxxx *next;
};
```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/131ab7a0cf11443db4ba52de596aa4f6.png)


## 📌 文件按照是否被打开分为：被打开的文件和没有被打开的文件


被打开的文件是存放在内存中的，没有被打开的文件是存放在磁盘中的



## 📌 本次学习的目的是：研究进程和打开文件之间的关系








# 🏷️ 复习一下常见的 C 的文件接口

## 📌 材料准备：

我们需要：``test.c`` , `Makefile`

### Makefile:
```c
myfile:myfile.c

	gcc -o $@ $^ 

.PHONY:clean

clean:

	rm -f myfile
```

### test.c

```c
#include <stdio.h>

int main()
{
    // "w": 按照写的方式来打开文件，如果文件不存在就创建
    FILE *fd = fopen("log.txt", "w");
    if (fd == NULL)
    {
        perror("fopen");
        return 1;
    }
	fclose(fd);  // 关闭文件
    return 0;
}
```

1. `FILE *fd = fopen("log.txt", "w");`
   这行代码调用了C标准库函数`fopen`，尝试以写入模式（"w"）打开名为`log.txt`的文件。如果文件不存在，`fopen`会尝试创建它。`fopen`函数返回一个指向`FILE`对象的指针，这个对象包含了所有与文件操作相关的信息。这个指针被存储在变量`fd`中，`fd`是一个指向`FILE`类型的指针。

2. `if (fd == NULL) {`
   这行代码检查`fopen`是否成功打开了文件。如果`fopen`不能打开文件（可能是因为文件不存在且没有权限创建，或者磁盘空间已满等原因），它会返回`NULL`。因此，这个`if`语句检查`fd`是否为`NULL`。

3. `perror("fopen");`
   如果`fd`是`NULL`，这意味着`fopen`函数失败了。在这种情况下，`perror`函数被调用来打印一条错误消息。`perror`函数的第一个参数是一个字符串，后面跟着一个冒号和一个空格。这个字符串是用户提供的，用来标识错误消息的来源。在这个例子中，字符串是`"fopen"`。`perror`会将这个字符串与`errno`全局变量中存储的错误代码相结合，`errno`是在尝试打开文件时由`fopen`设置的。然后，`perror`将打印出相应的错误消息到标准错误输出（通常是你的控制台或终端）。

4. `return 1;`
   如果文件打开失败，函数返回`1`。这是一种约定，表示函数因为遇到错误而提前终止。返回值`1`通常表示错误或异常状态，而返回`0`通常表示成功。

运行上面👆🏻的代码，`./mytest` ，可以发现我们的当前工作目录中已经出现了`log.txt` 这个文件。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b22a3ec3b4d34703b5c4a81528531b43.png)


**向这个文件中写入内容** 

```c    // 向这个文件中写入内容
    const char *msg = "hello linux file\n";
    int cnt = 10; 
    while(cnt)
    {
        fputs(msg, fd);
        cnt--;
    }

```

`fputs` 是 C 语言标准库中的一个函数，用于将一个字符串写入到文件中。这个函数声明在 `<stdio.h>` 头文件中，其原型如下：

```c
int fputs(const char *str, FILE *stream);
```

**整体代码**
```c
// myfile.c

#include <stdio.h>

int main()
{
    // "w": 按照写的方式来打开文件，如果文件不存在就创建
    FILE *fd = fopen("log.txt", "w");
    if (fd == NULL)
    {
        perror("fopen");
        return 1;
    }

        // 向这个文件中写入内容
        const char *msg = "hello linux file\n";
        int cnt = 10;
        while(cnt)
        {
            fputs(msg, fd);
            cnt--;
        }
    
    fclose(fd);
    return 0;
}
```

运行上面的代码之后我们可以放心 log.txt 中已经被写入了内容。


> 注意：以 "w" 的方式打开这个文件，会覆盖掉这个文件之前的内容。只要你以"w"的方式打开了，无论你做没做修改，之前的文件的内容都会被清空。
> 我们如果要清空一个文件的内容，可以使用命令：`> 文件名` ， 比如：`> log.txt` 。`>` 是重定向，这里由于没有任何的前置操作，所以会被我们的 shell 解释成： 首先我们要重定向，就要先把这个文件给打开，但是由于没有前置的命名，所以最后又关闭这个文件，但是默认是以写的方式来打开这个文件的，所以这个文件的内容被清空了。

**除了 “w”的方式来写入，我们还可以使用“a”方式来写入，代码如下：

```c
#include <stdio.h>

int main()
{
    // "w": 按照写的方式来打开文件，如果文件不存在就创建
    FILE *fd = fopen("log.txt", "a"); /////////////////////// 这里 a 方式
    if (fd == NULL)
    {
        perror("fopen");
        return 1;
    }

        // 向这个文件中写入内容
    const char *msg = "message text";
    fputs(msg,fd);
    fclose(fd);
    return 0;
}
```

`a` 方式也是写入，和 `w` 方式的区别是：`a` 方式不会删除之前的内容，它是从文件的结尾处开始写入，即：==追加，不清空==

> 我们上面说的 `>` :是输出重定向，使用 `> log.txt` 会删除这个`log.txt` 之前的内容，我们也可以使用 `>>` :这个是追加重定向，使用`>> log.txtt` 不会删除`log.txt` 之前的内容而是追加
> 



# 🏷️ 认识系统接口，操作文件

 一个进程是通过操作系统来打开文件的，所以操作系统一定会提供相应的系统调用接口，我们学习的 c语言的文件相关的函数`fopen`,`fclose` 之类的。底层一定是封装了系统的调用接口的 

## 📌 认识系统调用接口：open

在Linux系统中，`open` 系统调用用于打开或创建一个文件，并返回一个文件描述符，该文件描述符用于后续的文件操作。以下是对 `open` 系统调用的详细解释：

### 头文件

要使用 `open` 系统调用，你需要包含以下头文件：

```c
#include <fcntl.h>
```

`fcntl.h` 头文件包含了文件控制选项，包括 `open` 系统调用的定义。

### 函数原型

`open` 系统调用的函数原型如下：

```c
int open(const char *pathname, int flags);
```

`pathname` : 是我们要打开的文件的名字
`flags` : 是标志位

在C语言中，`open` 函数是用来打开文件的系统调用，它定义在 `<fcntl.h>` 头文件中。`open` 函数的两个参数 `pathname` 和 `flags` 决定了如何打开文件。

`pathname` 参数是一个指向字符数组的指针，它包含了要打开的文件的路径。这个路径可以是相对路径也可以是绝对路径。

`flags` 参数是一个或多个标志位的组合，这些标志位定义了文件打开的方式。在Linux系统中，常用的标志位包括：
- `O_RDONLY`：以只读方式打开文件。如果文件不存在，打开操作将失败。
- `O_WRONLY`：以只写方式打开文件。如果文件不存在，会创建一个新文件。
- `O_RDWR`：以读写方式打开文件。如果文件不存在，打开操作将失败。
- `O_CREAT`：如果文件不存在，则创建新文件。通常与 `O_WRONLY` 或 `O_RDWR` 结合使用。
- `O_TRUNC`：如果文件已存在且成功打开，则将其长度截断为0。
- `O_APPEND`：设置文件的读写位置在文件末尾。通常用于写操作。
- `O_EXCL`：与 `O_CREAT` 一起使用，如果文件已存在，则 `open` 调用失败。
- `O_NONBLOCK`：以非阻塞方式打开文件。对于某些类型的文件（如终端设备），这可以使 `read` 和 `write` 调用立即返回而不是阻塞。
- `O_SYNC`：打开文件进行同步I/O操作。每次 `write` 调用都会等待数据实际写入磁盘。

这些标志位可以组合使用，以提供不同的文件打开选项。例如，如果你想以读写方式打开一个文件，并且如果文件不存在则创建它，你可以这样设置 `flags`：

```c
int flags = O_RDWR | O_CREAT;
```

在实际使用中，`flags` 参数的值通常是这些标志位的位或（bitwise OR）操作的结果。

#### **❓ 如何理解这个标记位 flags**

我们可以做一个简单的实验：
我们创建一个 `test.c` 文件
文件内容如下：
```c
#include <stdio.h>

  

#define Print1 1  // 0001 ------ 这是 1 的二进制表达形式

#define Print2 (1<<1) // 0010  左移 1 位

#define Print3 (1<<2) // 0010  左移 2 位

#define Print4 (1<<3) // 1000  左移 3 位

  

void Print(int flags) // 这个函数的作用是用来打印我们想打印的数

{

  if (flags&Print1) printf("hello 1\n");

  if (flags&Print2) printf("hello 2\n");

  if (flags&Print3) printf("hello 3\n");

  if (flags&Print4) printf("hello 4\n");

}

  

int main()

{

  Print(Print1);  // 我们想打印 1  

  Print(Print1|Print2); // 我们想打印 1，2,观察我们传入的参数,是 Print1 和 Print2 相或的结果作为参数：flags ,flags 被传入我们的函数 Print 之后在通过相应的 & 运算来得出我们想要的结果，我们这里就是来类比体会一下 open 函数中标志位的用法,以下同理

  Print(Print1|Print2|Print3); // 我们想打印 1,2,3，注意我们这里参数的写法，这样写法就类似 open 函数里面的标志位 flags 的用法

  Print(Print3|Print4); // 我们想打印 3， 4

  Print(Print4);  // 我们想打印 4。

  return 0;

}
```

运行结果：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8021ea79303e49dc944c9f0edbc26a0c.png)

==即：我们可以采用宏的方式 ，来向一个函树，可以批量化的传递多种标志位，任意标志位进行组合 ==


在Linux中，`open` 系统调用还有一个额外的参数 `mode`，用于设置新创建文件的权限：

```c
int open(const char *pathname, int flags, mode_t mode);
```

### 返回值

- 如果 `open` 系统调用成功，它将返回一个非负的文件描述符（它返回的那个 int 类型的数，我们称之为文件描述符）。
- 如果调用失败，它将返回 `-1`，并设置全局变量 `errno` 以指示错误原因。

### 参数

1. `pathname`：一个指向文件名的指针。这个文件名可以是相对路径或绝对路径。

2. `flags`：一个标志位，用于指定文件打开的方式。常见的标志位包括：
   - `O_RDONLY`：以只读方式打开文件。(read only 缩写)
   - `O_WRONLY`：以只写方式打开文件。(write only)
   - `O_RDWR`：以读写方式打开文件。(read  write)
   - `O_CREAT`：如果文件不存在，则创建一个新文件。（ creat ）
   - `O_TRUNC`：如果文件已存在且成功打开，则将文件长度截断为0。
   - `O_APPEND`：如果文件已存在，写入操作会在文件末尾追加数据。(appear end)
   - `O_EXCL`：与 `O_CREAT` 一起使用，如果文件已存在，则 `open` 调用失败。

3. `mode`（可选）：当创建新文件时，`mode` 参数指定了文件的权限模式。如果不需要创建新文件，这个参数通常被设置为0。`mode_t` 是一个数据类型，用于表示文件的权限模式。

### 用法

以下是使用 `open` 系统调用打开文件的示例：
```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用


int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0) // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }
    close(fd);
    return 0;
}
```

我们运行上面的程序，发现结果是这样的：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/2fa3f7ff6ec04667b889e003e04069aa.png)

当我们在新建这个文件的时候，我们并没有给它指明我们要以什么样的权限来创建这个文件，所以这里的权限 是乱码。所以我们要修改一下我们的代码，给`open` 函数加上第 3 个参数:`mode_t mode` 代表的就是权限。

```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用


int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT, 0666); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0) // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }
    close(fd);
    return 0;
}

```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/16698a50ff39427485fac18598c704bc.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7ee4345ebcb64b86bc47f0aed47bdff4.png)
==(1.48)==

## 📌 认识系统调用接口： write （对文件进行操作）

在 Linux 系统中，`write` 系统调用是一种基本的系统调用接口，用于将数据从用户空间写入到文件描述符指向的文件中。以下是对 `write` 系统调用的详细解释：

### 函数原型

```c
ssize_t write(int fd, const void *buf, size_t count);
```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/2191faa515ed45eeb71b8525d2c34193.png)
### 参数

- `fd`：文件描述符（file descriptor），这是一个非负整数，用于标识一个已经打开的文件或者套接字。
- `buf`：指向要写入数据的缓冲区的指针。这个缓冲区中的数据将被写入到文件描述符 `fd` 指向的文件中。
- `count`：要写入的字节数。

### 返回值

- 成功时，`write` 返回写入的字节数。这个值应该与 `count` 相等，除非遇到错误或者文件末尾（EOF）。
- 失败时，返回 `-1`，并设置全局变量 `errno` 以指示错误的具体原因。

### 行为

- `write` 系统调用将尝试将 `count` 个字节的数据从缓冲区 `buf` 写入到文件描述符 `fd` 指向的文件中。
- 如果写入成功，文件的当前偏移量（文件指针）会增加 `count` 个字节。
- 如果文件是不可变的（例如，只读文件系统上的文件），`write` 调用将失败。
- 如果写入的数据超过了文件的最大大小限制（例如，inode 描述的磁盘空间不足），`write` 调用也会失败。

### 错误

- `EACCES`：没有写入文件的权限。
- `EBADF`：文件描述符 `fd` 不是一个有效的文件描述符。
- `EFAULT`：`buf` 指向的内存区域不可访问。
- `EFBIG`：试图写入的数据超过了文件系统的最大文件大小。
- `EINTR`：`write` 调用被信号中断。
- `EIO`：I/O 错误。
- `ENOSPC`：文件系统没有足够的空间。
- `EROFS`：试图在只读文件系统上写入。

### 注意事项

- `write` 系统调用不会保证数据被物理写入到磁盘上，它只是将数据写入到内核缓冲区。数据可能会在之后被异步写入磁盘。
- 如果需要确保数据被写入磁盘，可以使用 `fsync` 或 `O_SYNC` 标志。
- `write` 调用可以写入的数据量可能受到多种因素的限制，包括文件系统的块大小、内核缓冲区的大小等。

### 用法

```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用
#include <string.h> // strlen 要用

int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT, 0666); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0)                                         // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }

    // 操作这个文件：
    const char *msg = "hello file system call \n";
    write(fd, msg, strlen(msg) + 1);

    close(fd);
    return 0;
}
```

运行上面的程序之后，我们打开被创建的`log.txt` 发现了有乱码：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/166ddf30c24444caa7f2ebe4e3ae461b.png)


## 📌 了解一些标志位

### ✏️ O_WRONLY

> 还有一件事： O_WRONLY ，以写的方式来打开文件，但是它并不会清空原来文件里的内容，它是以覆盖的方式来写的。
> 举个例子：
> 如果你原来文件的内容是：aaaa
> 你新写入的内容是：bbb
> 最终的结果是：bbba
> 原因就是：它是以覆盖的方式来写的，前面的 3 个 a 就被新的 3 个 b给覆盖了。

看下面的代码的例子：
```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用
#include <string.h> // strlen 要用

int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT, 0666); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0)                                         // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }

    // 操作这个文件：
    const char *msg = "aaaa";
    write(fd, msg, strlen(msg));

    close(fd);
    return 0;
}
```


运行上面的代码之后，log.txt 中的文件内容就是：aaaa

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/03bfc601b30a4eeda520a0bcb3b8634f.png)


然后我们修改一下代码,把字符串改成bbb：
```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用
#include <string.h> // strlen 要用

int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT, 0666); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0)                                         // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }

    // 操作这个文件：
    const char *msg = "bbb";
    write(fd, msg, strlen(msg));

    close(fd);
    return 0;
}
```

我们来查看一下这时的结果：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/843a531d2fcc48379c65f722a8db6f8e.png)


### ✏️ O_TRUNC

好了，问题来了，如果我们想要打开这个文件的时候把这个文件里面的内容全部清空的话，我们要用的是一个新的选项---- `O_TRUNC`，他会将我们文件打开的长度清0，就是会先把文件清空，我们的代码如下：
```c
  int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666); 
```

![](https://img-blog.csdnimg.cn/img_convert/e8277b16a08864592b2aed41ac36f0d2.png)


### ✏️ O_APPEND
除此之外我们还有一些其他的选项，比如：`O_APPEND` ，这个选项让我们可以在文件的结尾处开始写入，类似于追加，不会清空文件
```c
int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); 
```

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/e7730027399d40aba6ae1354ebf6603b.png)


## 📌 文件的返回值，对打开文件的本质理解

通过上面的代码我们可以知道，文件的返回值十几个整数

```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close 要用
#include <string.h> // strlen 要用

int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); // O_WRONLY:只写入，O_CREAT:文件不存在就创建
    if (fd < 0)                                                    // 如果失败的话，会返回-1
    {
        perror("open");
        return 1;
    }

    printf("fd:%d\n", fd); // 我们可以把这个整数打印出来看一下
    
    // 操作这个文件：
    const char *msg = "ccc\n";
    write(fd, msg, strlen(msg));

    close(fd);
    return 0;
}
```

运行上面的代码，打印的结果是:``fd = 3``

为了更好的观察文件的返回值我们执行以下代码：

```c
// myfile.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <string.h> 

int main()
{
    int fda = open("loga.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); 
    int fdb = open("logb.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); 
    int fdc = open("logc.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); 
    int fdd = open("logd.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); 
    
    printf("fda:%d\n", fda);
    printf("fdb:%d\n", fdb);
    printf("fdc:%d\n", fdc);
    printf("fdd:%d\n", fdd);

    return 0;
}
```



![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/e5441c5b6b9d4c6f96d0f527d5a944b5.png)


我们发现文件的返回值是一串连续的整数，是不是有点像数组的下标呀。

### ✏️ 理解文件在操作系统中的表现

一个文件要被访问必须先被打开，谁来打开这个文件呢，是进程来打开这个文件，所以我们研究文件的本质，是研究进程和文件之间的关系。

我们可以类比学习进程时操作系统创建的``task_struct`` 来理解文件被打开时的状态，操作系统也会生成被打开文件的结构体描述对象``struct_file`` 


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1687bcf48436449dae38cb730d2fd80c.png)

这里提出一个新的问题，我们怎么知道这些被打开的文件是属于哪一个进程的？换句话说就是：进程和被打开文件的对应关系是怎么样的❓

其实操作系统为进程设计了一个结构体用来解决上面👆🏻的问题，这个结构体叫做：`struct files_struct`，这个结构体里包含了一个数组，这是数组的类型是：`struct file*` 数组的名字叫做：`fd_arry` ，连起来就是：`struct file* fd_arry[]` 

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3afe8c21f9ad46e0a996abaa164278a2.png)


当我们使用 open 来打开一个文件的时候会发生以上行为：
1. 会创建（malloc）一个 struct_file 的对象 。
 2. 把创建到底哪个文件对象的地址填入 fd_arry这个数组中。
 3. 将该数组的下标返回给上层。----- 这个数组的下标我们就叫文件描述符。

 上面的👆🏻的那张数组表我们就叫做文件描述符表---全称：进程文件描述符表


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/de8fee94a0f949bf833d37352ffa73eb.png)

### ✏️  所以文件描述符的本质就是数组的下标，此时问题来了，观察我们上面的代码的运行结果:
![](https://i-blog.csdnimg.cn/direct/e5441c5b6b9d4c6f96d0f527d5a944b5.png)

我们发现，文件描述符是从 3 开始的，那他为什么不从 0 1 2 开始呢?

解答如下：进程在启动时会默认打开 3 个文件：标准输入（键盘）标准输出（显示器）标准错误（显示器） ，有同学会感到疑惑---- 难道键盘和显示器也是文件吗？ 其实不用疑惑，因为 Linux 下一切皆是文件

| 标准输入 | 键盘  | stdin  | 0   |
| ---- | --- | ------ | --- |
| 标准输出 | 显示器 | stdout | 1   |
| 标准错误 | 显示器 | stderr | 2   |
所以0，1， 2 都被用掉了，我们新打开的文件就只能从 3 开始了 


## 📌 理解一下 struct file 对象（内核对象）

struct file 里面要包含哪些内容？
- 被打开文件的所有属性
- 文件缓冲区


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/03ee4c028b6d422c84d80b7b536cf682.png)


如果我们今天要读这个文件那么是谁在读？
答：进程，回想一下我们是怎么做的要么是 open 来打开文件 要么是 fopen来打开文件，这些都是进程干的事

我们看看下图来理一下思绪：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/84e3be12cd8247dcb9aa742da40a7b74.png)

如果我们要读数据，
我们要先把磁盘中的数据加载到文件缓冲区中，所以我们要先将数据加载到内存

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/69515ea3286247678d8812e2eda69bdb.png)

我们如果是写数据也同样需要将文件加载到内存中。
加载到内存这个动作是由操作系统来做的

我们在应用层进行数据读写的本质是什么？
答：本质是将内核缓冲区中的数据进行来回拷贝！ 





## 📌 fd 的分配规则

我们执行以下代码：
```c
#include <stdio.h>
#include <sys/types.h> // open 函数要用
#include <sys/stat.h>  // open 函数要用
#include <fcntl.h>     // open 函数要用
#include <unistd.h>    // close 函数要用

#define FILE_NAME "log.txt"

int main()
{
    // O_CREAT : 文件不存在就创建 ,O_WRONLY: 以写的方式来打开文件, O_TRUNC: 清空这个文件之前的内容
    int fd = open(FILE_NAME, O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    printf("fd:%d\n", fd);

    // 关闭这个文件
    close(fd);
    return 0;
}
```


打印的结果是：`fd:3` 符合我们的预期，因为0，1,   2 , 被标准输出，标准输入，标准错误给占据了，并且系统会默认给我们打开这三个文件。
现在问题来了，既然操作系统已经提前给我们打开了键盘显示器的文件，那我们是不是可以直接使用呢？

我们可以通过一个系统调用接口来实现我们的目的:`read`

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1faf31159f754f78bf436b787af7d07d.png)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/86b543eb0c574640b6bf524f4e485693.png)


















