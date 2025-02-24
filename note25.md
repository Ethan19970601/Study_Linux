我们使用ls -l的时候看到的除了看到文件名，还看到了文件元数据。
![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250202133036457.png)

每行包含7列：

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250202133457612.png)
其实这个信息除了通过这种方式来读取，还有一个stat命令能够看到更多信息

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250202133703633.png)

上面的执行结果有几个信息需要解释清楚

在Linux文件系统中，`inode`（索引节点）是文件系统中用于存储文件元数据的数据结构。每个文件或目录在文件系统中都有一个唯一的`inode`与之对应。`inode`中包含了文件的元数据，如文件大小、文件权限、所有者信息、创建时间、修改时间、访问时间、文件类型以及指向文件数据块的指针等。


**inode**
为了能解释清楚inode我们先简单了解一下文件系统

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250202134635512.png)


Linux `ext2`文件系统，上图为磁盘文件系统图（内核内存映像肯定有所不同），磁盘是典型的块设备，硬盘分区被划分为一个个的`block`。一个`block`的大小是由格式化的时候确定的，并且不可以更改。例如`mke2fs`的`-b`选项可以设定`block`大小为1024、2048或4096字节。而上图中启动块（Boot Block）的大小是确定的，


> `Block Group`：ext2文件系统会根据分区的大小划分为数个Block Group。而每个Block                             Group都有着相同的结构组成。
> `超级块（Super Block）`：存放文件系统本身的结构信息。记录的信息主要有：bolck 和                           inode的总量，未使用的block和inode的数量，一个block和inode的大                               小，最近一次挂载的时间，最近一次写入数据的时间，最近一次检验                                磁盘的时间等其他文件系统的相关信息。Super Block的信息被破                                       坏，可以说整个文件系统结构就被破坏了
> `GDT`:   Group Descriptor Table：块组描述符，描述块组属性信息                                                
> `块位图（Block Bitmap）`：Block Bitmap中记录着Data Block中哪个数据块已经被占用，                                             哪个数据块没有被占用
> `inode位图（inode Bitmap）：`每个bit表示一个inode是否空闲可用。                                           
> `i节点表（inode table）:` 存放文件属性 如 文件大小，所有者，最近修改时间等
> `数据区(data block)：`     存放文件内容                                         

> 通过上面的信息：`inode table` : 存放文件的属性。 `data block` ： 存放文件的内容。我们可以知道，在这里面：文件的内容和属性是分开存储的。




![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250202141845673.png)


这些多出来的数字就是该文件的`inode` 编号，一般情况一个文件有一个`inode`编号，基本上每个文件都要有`inode`编号，这个`inode` 是一个数字，在整个分区具有唯一性，在Linux 内核当中，识别文件和文件的名字无关只和`inode`有关，通过上面的截图，我们知道目录也有自己的`inode` ，linux 下一切皆文件，所以目录也是文件，目录内部直接保存文件的文件名和`inode`映射关系。


**问题❓，如果我想在一个目录下，新建，删除，修改一个文件，对于这个目录我需要什么权限？**

> `W` 权限，我们需要的事==写== 权限，不是 `X` 权限，我们要进入一个目录才是`x` 权限，但是如果我们要修改这个目录的内容，我们需要的是写的权限。

> 还有一件事：在Linux 中，文件名不属于文件的属性。所以`inode` 里面没有文件名，文件名只在目录里存着的。


# 软硬链接

## 软链接

假设，我们有一个叫:`log.txt` 的文件，现在我们给他创建一个软连接

创建一个软连接：`ln -s (目标文件) (给这个软连接取个名字)`
比如这里：
`ln -s log.txt log_soft_link.txt`

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250207163052505.png)


![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250207163429634.png)

> 软连接其实可以看作是 windows 操作系统下的那个快捷方式，比如这种，
> ![image.png|200](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220130642423.png)
> z
> ![image.png|300](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220130932851.png)

软连接有自己的 inode ，它是是一个独立的文件，文件的内容是：指向目标文件的路劲

硬链接是通过inode引用另外一个文件，软链接是通过名字引用另外一个文件，在shell中的做法

## 硬链接

我们看到，真正找到磁盘上文件的并不是文件名，而是inode。 其实在linux中可以让多个文件名对应于同一个` inode`,

创建硬链接：`ln (目标文件) (创建的链接的名字)`

比如这里，我们想要链接的目标文件是`log`
我们想给这个硬链接取的名字是`log_hard_link.txt`
我们输入的命令是：`ln log log_hard_link.txt`

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220123734249.png)

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220124517791.png)

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220125345253.png)

由此我们可以得出一个结论==软连接是一个独立的文件，但是硬链接不是一个独立的文件,因为硬链接没有独立的 inode 编号==

 ![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220132959555.png)

# 观察现象

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220134227075.png)


为什么会出现这样的现象呢？

首先，一个新的文件的硬链接数 是1 很好理解，因为我们创建的这个新文件只有他自己，只有一个文件名。

但是为什么，创建一个新的目录，这个新的目录的硬链接数是 2呢？

那我们进入这个新的目录，看看里面有什么吧。

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220134809678.png)

我们发现，这个新的目录有2个隐藏文件`.` 和 `..`  ，这是老早以前我们就知道的知识，但是你有没有好奇过 这些隐藏文件的作用是啥呢？

> 在Linux 系统中，我们知道，`.` 表示的是当前目录，`..` 表示的上级目录，这些隐藏文件的作用就是用来表示当前目录和上级目录的。

所以为什么，一个新的目录的硬链接数 是 2？

因为：1. 这个目录本身的文件名要占一个硬链接数 2. 这个目录中的隐藏文件：`.` ，表示的当前目录，相当于给当前目录取了个名字叫`.` 的硬链接，也是要占一个硬链接数的。所以加起来有2个硬链接数

你可以看下，当前目录的 inode 编号 和 当前目录的隐藏文件`.` 的 inode 编号是不是一样的。

![image.png](https://obsidian-01-1330327465.cos.ap-chengdu.myqcloud.com/20250220135945340.png)

看，inode 编号都是一样的。