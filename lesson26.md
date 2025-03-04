在软件开发中，程序库是一组预先编写好的程序代码，它们存储了常用的函数、变量和数据结构等。这些库可以帮助开发者节省大量的时间和精力，避免重复编写相同的代码。当我们在 Linux 系统中开发程序时，经常会用到两种类型的程序库：静态库和动态库。

- **静态库** (`.a`)：静态库是指在程序==编译时==，将库文件中的代码直接==复制==到可执行文件中。这意味着当程序运行时，库文件中的代码已经包含在可执行文件中，不需要在运行时进行加载。
- **动态库** (`.so`)：动态库是指在程序==运行时==才加载的库文件。程序本身不包含库文件的代码，而是通过链接到动态库来调用其中的函数和数据结构等。动态库通常以共享库的形式存在，多个程序可以共享一个动态库。



# 从库的制作者角度

## 静态库

我们可以自己尝试写一个简单的计算程序来实现简单的加减乘除功能。

我们可以先创建以下文件：

加法：`Add.c` , `Add.h`
减法：`Sub.c` , `Sub.h`
乘法：`Mul.c` , `Mul.h`
除法：`Div.c` , `Divh`


![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250224200226612.png)

如果我们要上面的文件形成一个库，我们是不能在上面的这些文件中写`main`函数的，因为库中是没有`main` 函数的，我们也不能把`main` 函数打入库中。

```c
// Add.h
#pragma once // 防止头文件被重复包含
#include <stdio.h>
int Add(int,int);
----------------------------
// Add.c
#include "Add.h"
int Add(int x, int y)
{
	return x + y;
}
```

```c
// Sub.h
#pragma once
#include <stdio.h>
int Sub(int,int);
----------------------------
// Sub.c

#include "Sub.h"
int Sub(int x, int y)
{
	return x - y;
}

```

```c
//Mul.h
#pragma once
#include <stdio.h>
int Mul(int,int);
----------------------------
// Mul.c

#include "Mul.h"
int Mul(int x, int y)
{
	return x * y;
}


```

```c
//Div.h
#pragma once
#include <stdio.h>
int Div(int,int,int*);
------------------------------
// Div.c

#include "Div.h"
int Div(int x, int y, int *code)
{
	*code = 0;
	if (y == 0)
	{
	 *code = -1;
	 return -1;	
	}
	return x / y;
}


```

我们现在要将上面的一堆的头文件 和 源文件 形成静态库

为 我们的方法进行测试，我们可以建立一个叫`TestMain.c` 的文件。

```c
// TestMain.c
#include "Add.h"
#include "Sub.h"
#include "Mul.h"
#include "Div.h"


 int x = 20;
 int y = 10;

 printf("%d + %d = %d\n",x, y, Add(x,y));
 printf("%d - %d = %d\n",x, y, Sub(x,y));
 printf("%d * %d = %d\n",x, y, Mul(x,y));
 

```


我们可以运行一下上面的测试程序`TestMain.c`，不过我们要先进行编译：
```bash
// Linux 系统 

gcc -o test Add.c Sub.c Mul.c Div.c TestMain.c

// 我们在编译的时候没有加上头文件，因为当前的头文件和源文件是在同一个路径下的，所以我们不用加上头文件。


```

 上面我们是把所有的源文件都进行了编译的，其实如果我们要形成一个可执行程序，我们不太建议将所有的源文件都直接进行编译，因为我们一旦这样做，每一个源文件都要进行预处理，编译，汇编，链接。所以一般在面对这样的多文件项目时我们一般都建议把这样的源文件编译成`.o` 文件。编译成`.o` 文件之后，然后我们在把所有的`.o` 文件进行对应的链接 形成一个可执行。

> `.c`--(预处理，编译，汇编)---> `.o` ----(链接)----> 可执行


```bash

// 介绍一下`-c` 的选项。
gcc -c test.c // 在 test.c形成可执行前（即：链接前） 终止，最终会生成一个`.o` 的文件，默认是同名的`test.o`
```


所以如果我们把上面文件中的`TestMain.o` 去掉，为什么要去掉呢？因为如果我们要做一个静态库，库中是不能有`main` 函数的，`main` 函数只能是用这个库的用户写的。这样一来就只剩下`Add.0`,`Sub.o`,`Mul.o`,`Div.o` 了，我们只需要对`Add.0`,`Sub.o`,`Mul.o`,`Div.o` 这些`.o` 文件进行打包，以后如果某个用户想要用到这些加减乘除的方法，只需要将自己写的`main` 函数编译成`xxx.o` 文件，然后再和打包好的文件中的`.o` 文件进行链接就好了。


### 生成静态库

那我们如何对我们的`.o`文件进行打包呢？

`ar` 命令 
`ar`命令是一个把所有的`.o` 文件打包形成我们对应的库文件的过程。

```bash
 ar -rc libmymath.a add.o sub.o 
```

`ar`是gnu归档工具，选项：`rc`表示(`r`eplace and `c`reate)，他的意思是：把所有的`.o`文件打包生成一个`.a` 文件，如果存在就替换(`replace`)如果不存在就创建(`create`)。


库的名字要以`lib` 开头，以`.a` 结尾，所以上面我们的库是`mymath` ,但是加上前缀和后缀之后就是：`libmymath.a` 了。


> 静态库的原理就是将库中的源代码直接翻译成`.o` 目标二进制文件，然后打包




## 动态库

### **一、什么是动态库？**

动态库，也叫共享库（Shared Library），是一种在程序运行时才被加载到内存中的代码库。它的文件名通常以 `libxxx.so` 的形式命名（`xxx` 是库的名字，`so` 表示 shared object）。
与动态库相对的是**静态库**（Static Library），静态库在程序编译时会被直接嵌入到可执行文件中，文件名通常以 `libxxx.a` 的形式命名。

### **二、生成动态库**

#### **1. 编译选项**

生成动态库需要使用 `gcc` 编译器，并指定以下选项：

- **`-fPIC`**：生成位置无关代码（Position Independent Code, PIC）。这种代码可以在内存中的任何位置运行，是动态库的必要条件。
- **`-shared`**：表示生成共享库格式。

#### **2. 示例代码**

假设我们有以下文件：

- `add.c`：实现加法函数。
- `sub.c`：实现减法函数。

文件内容如下：

```cpp
// add.c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}
```

```cpp
// sub.c
#include <stdio.h>

int sub(int a, int b) {
    return a - b;
}
```

#### **3. 生成动态库的步骤**

```Linux
# 编译源文件为位置无关代码
gcc -fPIC -c add.c sub.c

# 生成动态库
gcc -shared -o libmymath.so add.o sub.o

```
执行完上述命令后，会生成一个名为`libmymath.so` 的动态库文件。



# 从库的使用者角度

## 静态库的使用

上面我们自己写的库`libmymath.a` 是第三方库，`gcc` 默认不认识 。所以我们在使用`gcc` D的时候还要加上一个`-l` 选项来链接到我们的库，还记得上面我们所说的吗，我们的库是以`lib`为前缀的，以`.a` 为后缀的，但是在用`-l` 链接的时候，我们要去掉前缀和后缀，直接用`mymath` 像这样：`-lmymath` ，我们还要用`-L` 来制定库的路径，我们这样来表示`-L.` ,用`.` 来表示当前路径，如果你的静态库在当前路径下的话，所以连在一起就是：
```Linux
[root@localhost linux]# gcc TestMain.c -L. -lmymath
```

- TestMain.c 是用户写的程序。
- -L 指定库路径
- -l 指定库名
- `-I` : 指定头文件的路径

如果我们要把自己写的库交给其他用户使用的话，那我们就要把`.h` 和`.a` 交给他， 

> gcc 默认是动态链接的，



##  动态库的使用


### **使用动态库**

#### **1. 编译选项**

在使用动态库时，需要指定以下选项：

- **`-L`**：指定动态库所在的路径（如果动态库不在系统默认路径中）。
- **`-l`**：指定要链接的动态库的名字（去掉 `lib` 前缀和 `.so` 后缀）。

#### **2. 示例代码**

假设我们有一个主程序 `main.c`，它调用了动态库中的函数：
```cpp
// main.c
#include <stdio.h>

int add(int a, int b);
int sub(int a, int b);

int main() 
{
    printf("add(10, 20) = %d\n", add(10, 20));
    printf("sub(100, 20) = %d\n", sub(100, 20));
    return 0;
}
```

#### **3. 编译主程序**
```bash
# 编译主程序
gcc main.c -o main -L. -lmymath
```
- `-L.` 表示动态库在当前目录下。
- `-lmymath` 表示链接 `libmymath.so` 动态库。

### **运行动态库**

#### **方法一：将 `.so` 文件拷贝到系统共享库路径**
系统默认的动态库路径通常是 `/usr/lib` 或 `/usr/local/lib`或是：`/lib64` 。可以将动态库文件拷贝到这些路径下：

```bash
# 拷贝动态库到 /usr/local/lib
cp libmymath.so /usr/local/lib/
```

#### **方法二：通过使用软连接 查找动态库**

```bash
ln -s /*动态库的路径*/ libmymath.so
```

`/*动态库的路径*/` : 这里填动态库的路径

`libmymath.so` : 这个是软连接的名字。

这样，我们通过在当前目录下建立软连接的方式找到我们对应的库。
gcc 可以在当前目录下找到动态库。所以我们可以在当前目录下建立对应库的软连接，这样来让 gcc 找到，我们也可以在系统的指定目录下建立对应的软连接。



#### **方法三：设置 `LD_LIBRARY_PATH` 环境变量**（LD 的意思就是 load -加载，所以这句话就是 ： 加载库路径环境变量)

如果不想将动态库文件拷贝到系统路径，可以通过设置 `LD_LIBRARY_PATH` 环境变量来指定动态库的路径：
```c++
# 设置 LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/*这里填你写的动态库的路径*/

```

但是这里导入的环境变量是内存级的，所以你重启电脑之后就没了。

#### **方法四：直接更改系统配置文件：使用 `ldconfig` 配置动态库路径**

如果动态库文件存放在非默认路径下，可以通过修改 `/etc/ld.so.conf` 文件或在 `/etc/ld.so.conf.d/` 下添加配置文件来指定动态库路径。

例如：
```c++
# 创建配置文件
echo "/path/to/your/library" > /etc/ld.so.conf.d/mylib.conf

# 更新动态库缓存
ldconfig
```



> 同一组库，提供动静两种库，gcc默认使用动态库。

### **五、使用外部库**

Linux 系统中有很多外部库，例如：

- **`ncurses`**：用于处理屏幕显示和用户输入的库。
- **`glib`**：用于通用工具函数的库。
- **`openssl`**：用于加密和安全通信的库。

使用外部库的步骤与使用自己编写的动态库类似，需要：

1. 安装外部库（通常通过包管理工具，如 `apt` 或 `yum`）。
比如安装`ncurses` 库
`sudo yum install -y ncurses-devel`
1. 在编译时指定库的路径和名称。

例如，使用 `ncurses` 库：
```cpp
# 编译
gcc main.c -o main -lncurses

# 运行
./main
```