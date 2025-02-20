lesson2402

# 模拟实现一下C标准版库的函数(只是简单的做一下代码说明

我们需要准备以下文件：

`mystdio.h`  ,  `mystdio.c` ,  `main.c`

## mystdio.h

```c
#pragma once 
```

`#pragma once` 是一种预处理器指令，用于防止头文件被重复包含。其作用主要： 确保头文件内容只被编译一次
- 当一个头文件被多个源文件包含时，如果没有使用 `#pragma once` 或其他类似的措施，头文件中的内容会被重复编译多次。这可能会导致编译错误，例如重复定义变量、函数等。
- 例如，假设有一个头文件 `my_header.h`，其中定义了一个全局变量 `int global_var;`。如果两个源文件 `file1.c` 和 `file2.c` 都包含了 `my_header.h`，那么在编译过程中，`global_var` 会被定义两次，从而引发编译错误。使用 `#pragma once` 后，无论 `my_header.h` 被包含多少次，其内容都只被编译一次，有效避免了这类问题。

```c
typedef struct _myFILE // 封装一个file结构体对象
{
    int fileno;
} myFILE;

extern myFILE *my_fopen(const char *path, const char *mode);
extern int my_fputs(const char *s, myFILE *stream);
extern int fflush(myFILE *stream);
extern int fclose(myFILE* stream);

```

1. 解释 `extern`

`extern` 是一个存储类说明符，用于声明变量或函数在其他文件或代码段中定义。它的主要作用是告诉编译器，==某个变量或函数的定义在其他地方，当前文件中只是对它的引用==。

 对于变量

- 当多个源文件需要共享同一个全局变量时，可以在一个源文件中定义该变量，然后在其他源文件中使用 `extern` 来声明它。
- 例如，假设有一个全局变量 `int global_var;` 定义在 `file1.c` 中：

```c
// file1.c
int global_var = 10;
```

- 在 `file2.c` 中，如果需要使用 `global_var`，可以这样声明：
```c
// file2.c
extern int global_var;
```
- 这样，`file2.c` 就可以访问 `file1.c` 中定义的 `global_var` 了。
对于函数

- 对于函数，`extern` 通常用于声明在其他源文件中定义的函数，以便当前文件可以调用它。
- 例如，假设有一个函数 `void my_function(void);` 定义在 `file1.c` 中：
```c
// file1.c
void my_function(void) {
	// 函数实现
}
```
- 在 `file2.c` 中，如果需要调用 `my_function`，可以这样声明：
```c
// file2.c
extern void my_function(void);
```
- 这样，`file2.c` 就可以调用 `file1.c` 中定义的 `my_function` 了。



 1. 为什么函数可以省略 `extern`，但是变量不行

 函数可以省略 `extern`

- 在 C 语言中，函数声明默认具有 `extern` 属性。这意味着如果你在一个源文件中声明了一个函数，而没有使用 `extern` 关键字，编译器会默认它是 `extern` 的。
- 例如，以下两种声明是等价的：
```c
void my_function(void);  // 默认具有 extern 属性
extern void my_function(void);  // 明确声明为 extern
```
- 因此，通常在头文件中声明函数时，不需要显式使用 `extern` 关键字。

 变量不能省略 `extern`

- 对于变量，情况则不同。如果在一个源文件中定义了一个全局变量，而在另一个源文件中需要使用它，必须使用 `extern` 关键字来声明。
- 这是因为变量的定义和声明在语义上有不同的含义：
    - **定义**：分配存储空间，并可以初始化变量。
    - **声明**：告诉编译器变量的类型和名称，但不分配存储空间。
- 如果在多个源文件中都定义了同一个全局变量，编译器会报错，因为这会导致多个定义冲突。

- 例如，假设在 `file1.c` 中定义了一个全局变量：

```c
// file1.c
int global_var = 10;
```

- 在 `file2.c` 中，如果需要使用 `global_var`，必须这样声明：
```c
// file2.c
extern int global_var;
```

- 如果在 `file2.c` 中直接写 `int global_var;`，这会被视为一个新的定义，而不是对 `file1.c` 中定义的 `global_var` 的引用，从而导致编译错误。

总的来说
- 函数声明默认具有 `extern` 属性，因此可以省略 `extern`。
- 变量声明需要使用 `extern` 关键字，以区分定义和声明，避免多个定义冲突。

## 标志位 flag 的作用

标志位`flag` 的作用是指定文件打开时的模式和行为，通过不同的标志位组合，可以实现对文件的不同的操作方式，如读取，写入，追加等。

1. **标志位的定义和用途**
- `flag` 是一个整数变量，用于存储文件打开时的各种标志位。
- 这些标志位是通过位运算（如按位或 `|`）组合在一起的，每个标志位代表一种特定的文件操作行为。

2. **具体标志位的含义**

- **`O_RDONLY`**：以只读方式打开文件。
- **`O_WRONLY`**：以只写方式打开文件。
- **`O_CREAT`**：如果文件不存在，则创建该文件。
- **`O_TRUNC`**：如果文件已存在，则将其长度截断为 0。
- **`O_APPEND`**：写入时将数据追加到文件末尾。

3. **`flag |= (***)`**

在 C 语言中，`flag |= (****)` 这个操作使用了按位或运算符 `|` 和复合赋值运算符 `|=`。这个操作的目的是将多个标志位组合在一起，并将结果存储在 `flag` 变量中。下面详细解释这个操作的原理：
 1. **按位或运算符 `|`**
按位或运算符 `|` 用于对两个整数的每一位进行逻辑或操作。具体来说，对于两个整数的每一位：
- 如果两个位中至少有一个为 1，则结果位为 1。
- 如果两个位都为 0，则结果位为 0。
例如：
- `0101`（二进制）`|` `1010`（二进制）= `1111`（二进制）
- `5`（十进制）`|` `10`（十进制）= `15`（十进制）

2. **复合赋值运算符 `|=` 或等**

```c
int flag = 0;
flag |= 1;  // flag = flag | 1
```

- 初始时 `flag` 为 `0`（二进制 `0000`）。
- `1` 的二进制表示为 `0001`。
- `flag | 1` 的结果为 `0001`。
- 因此，`flag` 的值变为 `1`。

3. **组合多个标志位**

在文件操作中，多个标志位通常被定义为宏，每个宏代表一个特定的位。通过按位或运算符 `|`，可以将这些标志位组合在一起，形成一个综合的标志位。
例如：
```c
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_CREAT  00000100
#define O_TRUNC  00001000
#define O_APPEND 00010000
```

- `O_WRONLY` 的二进制表示为 `00000001`。
- `O_CREAT` 的二进制表示为 `00000100`。
- `O_TRUNC` 的二进制表示为 `00001000`。

当使用 `flag |= (O_WRONLY | O_CREAT | O_TRUNC)` 时：

- `O_WRONLY | O_CREAT | O_TRUNC` 的结果为 `00001101`。
- `flag |= 00001101` 将 `flag` 的值设置为 `00001101`。


 1. **具体示例**

```c
int flag = 0;
if (strcmp(mode, "w") == 0) // w 以写的方式打开
{
    flag |= (O_CREAT | O_TRUNC | O_WRONLY); // 我们以写的方式来打开这个文件，如果文件不存在则创建。O_CREAT
                                            // 如果文件已经存在，我们还要清空, O_TRUNC
                                            // 最后才是往文件中写入内容。O_WRONLY
}
```

- 初始时 `flag` 为 `0`（二进制 `00000000`）。
- `O_CREAT` 的二进制表示为 `00000100`。
- `O_TRUNC` 的二进制表示为 `00001000`。
- `O_WRONLY` 的二进制表示为 `00000001`。
- `O_CREAT | O_TRUNC | O_WRONLY` 的结果为 `00001101`。
- `flag |= 00001101` 将 `flag` 的值设置为 `00001101`。

5. **总结**
通过 `flag |= (****)` 这个操作，可以将多个标志位组合在一起，形成一个综合的标志位，从而在文件操作中指定多种行为。这种按位或运算符的使用在系统编程中非常常见，因为它可以高效地组合多个标志位，而不会相互干扰。

# main.c

```c
#include "mystdio.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h> // sleep 要用

int main()
{
    myFILE *fp = my_fileno("./log.txt", " w ");
    if (fp == NULL)
    {
        perror("my_fopen");
        return 1;
    }
    int cnt = 20;
    const char *msg = "this is my stdio lib\n";
    while (cnt--)
    {
        my_fwrite(msg, strlen(msg), fp);
        sleep(1);
    }

    my_fclose(fp);

    return 0;
}
```


# stdio.c

```c

#include "mystdio.h"
#include <string.h> // 我们要用到字符串，所以要这个头文件

#include <sys/types.h> // 系统调用接口要用的头文件
#include <sys/stat.h>  // 系统调用接口要用的头文件
#include <fcntl.h>     // 系统调用接口要用的头文件

#include <errno.h>
#include <stdlib.h>   // malloc 函数要用
#include <unistd.h>   // close 函数要用
#define DFL_MODE 0666 // DFL_MODE 的作用DFL_MODE：默认文件权限模式。当使用 open 系统调用创建新文件时，如果指定了 O_CREAT 标志位，需要提供一个权限模式。DFL_MODE 就是这个权限模式的默认值。

int main()
{

    // ******* 打开 **********
    myFILE *my_fopen(const char *path, const char *mode);
    {
        int fd = 0;                 // fd 文件描述符
        int flag = 0;               // flag 标志位
        if (strcmp(mode, "r") == 0) // r 以读的方式打开
        {
            flag |= O_RDONLY;
        }
        else if (strcmp(mode, "w") == 0) // w 以写的方式打开
        {
            flag |= (O_CREAT | O_TRUNC | O_WRONLY); // 我们以写的方式来打开这个文件，如果文件不存在则创建。O_CREAT
                                                    // 如果文件已经存在，我们还要清空, O_TRUNC
                                                    // 最后才是往文件中写入内容。O_WRONLY
        }
        else if (strcmp(mode, "a") == 0) // a 以追加的方式打开
        {
            flag |= (O_CREAT | O_WRONLY | O_APPEND); // 我们以追加的方式向其中写入内容
                                                     // 如果这个文件不存在，我们就创建 O_CREAT
                                                     // 然后向其中写入：O_WRONLY
                                                     // 以追加的方式写入: O_APPEND
        }
        else
        {
            // 其他的还有很多情况，我们这里就只举几种,所以这里是其他的情况
        }
        if (flag & O_CREAT) // 我们对一个文件进行操作，这个文件可能存在，可能不存在，所以我们要用上面的flag 来判断一下,用flag这个标志位的值和特定的O_**** 相与来进行判断，这里是O_CREAT,如果结果为真，说明我们需要创建一个文件
        {
            fd = open(path, flag, DFL_MODE); // open 系统调用接口
                                             // 打开那个文件——path
                                             // 打开的方式——flag
                                             // 创建文件需要权限——DFL_MODE
        }
        else // else 说明flag标志位里没有创建，说明不需要创建文件，不需要创建文件，说明文件存在
        {
            fd = open(path, flag); // 打开那个文件——path
            // 打开文件的方式——flag
            // 这里没有创建文件，所以我们不用带上权限
        }
        if (fd < 0) // 如果fd < 0 ，打开文件失败, open 系统调用：当使用 open 函数打开一个文件时，如果成功，它会返回一个新的文件描述符。如果失败，它会返回 -1。
        {
            errno = 2; // 进程错误码：errno是一个全局变量，用于存储错误码。当系统调用或库函数执行失败时，会设置errno的值。不同的错误码代表不同的错误类型。errno值2通常对应于错误代码ENOENT，表示“没有那个文件或目录（No such file or directory）”。这些错误码用于向程序的其他部分或调用者提供关于错误原因的详细信息。
            return NULL;
        }

        //***************************** 当代码走到这里的时候，说明文件已经成功的打开了            ******************* */

        myFILE *fp = (myFILE *)malloc(sizeof(myFILE));
        if (!fp)
        {
            errno = 3;
            return NULL;
        }
        fp->flag = FLUSH_LINE; // 我们这里默认是行刷新
        fp->end = 0;           // 默认为0，表示没有数据
        fp->fileno = fd;
        return fp;
    }

    // ***********  写入 *******************
    int my_fwrite(const char *s, int num, myFILE *stream)
    {
        // 写入的本质不是把数据写入文件中，而是把数据放进缓冲区中
        memcpy(stream->buffer + stream->end, s,num);
        stream->end += num; // 更新缓冲区的大小
        // 缓冲区的大小改变了，接下来我们要判断是否需要刷新
        if ((stream->flag & FLUSH_LINE) && stream->len >0 &&  stream->buffer[stream->end-1] == '\n') // 判断是否是行刷新，还有用户传入的字符带不带'\n'
        {
            my_fflush(stream); 
        }
        return num;
    }

    // ******  刷新 **********
    int my_fflush(myFILE * stream)
    {
        if (stream->end > 0) // 如果end>0 说明缓冲区中有数据，
        {
            write(stream->fileno, stream->buffer, stream->end);
            stream->end = 0;
        }
        return 0;
    }

    // ********* 关闭 *************
    int fclose(myFILE * stream)
    {
        my_fflush(stream);
        return close(stream->fileno);
    }
    return 0;
}
```

# stdio.h

```c
#pragma once // 防止头文件被重复包含

#define SIZE 4096
#define FLUSH_NONE 1 // 缓冲区的刷新策略
#define FLUSH_LINE (1 << 1) // 行刷新
#define FLUSH_ALL (1 << 2) // 全刷新

typedef struct _myFILE // 封装一个file结构体对象
{
    int fileno;
    int flag;
    char buffer[SIZE];
    int end; // 正常的情况下，我们要用一个start 一个end来限定这块缓冲区的大小区域，只有start和end之间的区域是合理的，我们这里简单点，只用一个end就行了。
} myFILE;

extern myFILE *my_fopen(const char *path, const char *mode);
extern int my_fwrite(const char *s, int num, myFILE *stream);
extern int my_fflush(myFILE *stream);
extern int my_fclose(myFILE *stream);

```