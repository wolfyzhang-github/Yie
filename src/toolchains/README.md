在 CSAPP 3e arch 实验中使用到的官方工具包，包含 Y86-64 ISA 的汇编器 yas 和指令集模拟器 yis。

Y86 指令执行模拟器 yie (Y86-64 ISA Emulator)，可以模拟一个虚拟的 Y86-64 ISA CPU，以初步验证指令执行的效果。

使用方法：
1. 进入源码目录，执行 Makefile，生成 yie。
    ``````
    $ make
2. 切换到输出目录，即可执行 yie。
    ``````
    $ cd bin
    $ ./yie
   