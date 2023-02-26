# CS:APP-Lab

本项目包含《深入理解计算机系统》第3版配套实验的相关文件，以及我的笔记

## 项目结构

### [Notes](https://github.com/kcxain/CSAPP-Lab/tree/master/Notes)

我对各个实验的深入解析文档，也可前往我的[知乎专栏](https://www.zhihu.com/column/c_1480603406519238656)获得更好的阅读体验

- [Lab01-Data_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab01-Data_Lab.md)
- [Lab02-Bomb_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab02-Bomb_Lab.md)
- [Lab03-Attack_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab03-Attack_Lab.md)
- [Lab04-Architecture_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab04-Architecture_Lab.md)
- [Lab05-Cache_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab05-Cache_Lab.md)
- [Lab06-Performance_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab06-Performance_Lab.md)
- [Lab07-Shell_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab07-Shell_Lab.md)
- [Lab08-Malloc_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab08-Malloc_Lab.md)
- [Lab09-Proxy_Lab](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab09-Proxy_Lab.md)

### [initial_labs](https://github.com/kcxain/CSAPP-Lab/tree/master/initial_labs)

包含全部实验的原始文件，也可前往[课程主页](http://csapp.cs.cmu.edu/3e/labs.html)下载

### [solutions](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions)

是我进行实验操作后的文件

## 实验进度：

- [x] [Data Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/01_Data%20Lab)
- [x] [Bomb Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/02_Bomb%20Lab)
- [x] [Attack Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/03_Attack%20Lab)
- [x] [Architecture Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/04_Architecture%20Lab)
- [x] [Cache Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/05_Cache%20Lab)
- [x] [Performance Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/06_Performance%20Lab)
- [x] [Shell Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/07_Shell%20Lab)
- [x] [Malloc Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/08_Malloc%20Lab)
- [x] [Proxy Lab](https://github.com/kcxain/CSAPP-Lab/tree/master/solutions/09_Proxy%20Lab)

## 实验介绍

### lab 1  [*Data Lab*](http://csapp.cs.cmu.edu/3e/datalab-handout.tar)

Students implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C. For example, they might be asked to compute the absolute value of a number using only bit-level operations and straightline code. This lab helps students understand the bit-level representations of C data types and the bit-level behavior of the operations on data.

### lab 2  [*Bomb Lab*](http://csapp.cs.cmu.edu/3e/bomb.tar)

A "binary bomb" is a program provided to students as an object code file. When run, it prompts the user to type in 6 different strings. If any of these is incorrect, the bomb "explodes," printing an error message and logging the event on a grading server. Students must "defuse" their own unique bomb by disassembling and reverse engineering the program to determine what the 6 strings should be. The lab teaches students to understand assembly language, and also forces them to learn how to use a debugger. It's also great fun. A legendary lab among the CMU undergrads.

Here's a [Linux/x86-64 binary bomb](http://csapp.cs.cmu.edu/3e/bomb.tar) that you can try out for yourself. The feature that notifies the grading server has been disabled, so feel free to explode this bomb with impunity. If you're an instructor with a CS:APP account, then you can download the [solution](http://csapp.cs.cmu.edu/im/bomb-solution.txt).

### lab 3  [*Attack Lab*](http://csapp.cs.cmu.edu/3e/target1.tar)

*Note: This is the 64-bit successor to the 32-bit Buffer Lab.* Students are given a pair of unique custom-generated x86-64 binary executables, called *targets*, that have buffer overflow bugs. One target is vulnerable to code injection attacks. The other is vulnerable to return-oriented programming attacks. Students are asked to modify the behavior of the targets by developing exploits based on either code injection or return-oriented programming. This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.

If you're a self-study student, here are a pair of [Ubuntu 12.4 targets](http://csapp.cs.cmu.edu/3e/target1.tar) that you can try out for yourself. You'll need to run your targets using the **"-q"** option so that they don't try to contact a non-existent grading server. If you're an instructor with a CS:APP acount, you can download the solutions [here](http://csapp.cs.cmu.edu/im/labs/target1-sol.tar).

### lab 4  [ *Architecture Lab*](http://csapp.cs.cmu.edu/3e/archlab-handout.tar)

*Note: Updated to Y86-64 for CS:APP3e.* Students are given a small default Y86-64 array copying function and a working pipelined Y86-64 processor design that runs the copy function in some nominal number of clock cycles per array element (CPE). The students attempt to minimize the CPE by modifying both the function and the processor design. This gives the students a deep appreciation for the interactions between hardware and software.

Note: The lab materials include the master source distribution of the Y86-64 processor simulators and the *Y86-64 Guide to Simulators*.

### lab 5  [*Cache Lab*](http://csapp.cs.cmu.edu/3e/cachelab-handout.tar)

At CMU we use this lab in place of the Performance Lab. Students write a general-purpose cache simulator, and then optimize a small matrix transpose kernel to minimize the number of misses on a simulated cache. This lab uses the Valgrind tool to generate address traces.

Note: This lab must be run on a 64-bit x86-64 system.

### lab 6  [*Performance Lab*](http://csapp.cs.cmu.edu/3e/perflab-handout.tar)

Students optimize the performance of an application kernel function such as convolution or matrix transposition. This lab provides a clear demonstration of the properties of cache memories and gives them experience with low-level program optimization.

### lab 7  [*Shell Lab*](http://csapp.cs.cmu.edu/3e/shlab-handout.tar)

Students implement their own simple Unix shell program with job control, including the ctrl-c and ctrl-z keystrokes, fg, bg, and jobs commands. This is the students' first introduction to application level concurrency, and gives them a clear idea of Unix process control, signals, and signal handling.

### lab 8  [*Malloc Lab*](http://csapp.cs.cmu.edu/3e/malloclab-handout.tar)

Students implement their own versions of malloc, free, and realloc. This lab gives students a clear understanding of data layout and organization, and requires them to evaluate different trade-offs between space and time efficiency. One of our favorite labs. When students finish this one, they really understand pointers!

### lab 9  [ *Proxy Lab*](http://csapp.cs.cmu.edu/3e/proxylab-handout.tar)

Students implement a concurrent caching Web proxy that sits between their browser and the rest of the World Wide Web. This lab exposes students to the interesting world of network programming, and ties together many of the concepts from the course, such as byte ordering, caching, process control, signals, signal handling, file I/O, concurrency, and synchronization.