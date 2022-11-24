## 简介

为方便开发 Brat 而开发的工具链，主要包含以下几个模块：

* yie：y86-84 isa emulator
* assembler.py：用于将官方工具汇编后的汇编代码转换为字节码的Python脚本
* sum：演示 yie 功能的样例程序
* test：yie 的部分测试用例

另外，还包含在 CSAPP 3e arch 实验中使用到的官方工具包，包含 Y86-64 ISA 的汇编器 yas 和指令集模拟器 yis。

## 使用方法

1. 进行 yie 的测试

```bash
$ ./test.sh
```
若测试通过，则输出 `Dead cycle Test passed`。

2. 执行 Makefile，生成 yie。

```bash
$ make
```
3. 切换到输出目录，即可执行 yie。
```bash
$ cd bin
$ ./yie <y86-64 bin filename>
```

另外，lib 目录中是官方的 isa 实现，我粗看了一下，有些太过复杂，所以在实现 yie 的过程中我没有使用它。
但我觉得它很值得研究，如果未来有机会重构 yie，使用这个 isa 库来实现或许会优雅很多，所以先留在这儿。

## 其他

TODO-List:

* 补充完整 yie 的测试用例
* 完善 yie 关于 exception handler 的 features
* 完善 yie 中 `-h` 或者 `<max steps>` 等关于更多命令行参数的 features
* 更新 yie 寄存器值等信息的输出格式
* 完善 yie 对 `.align` 支持的 features
* 修复若干小 bug
* 补全注释
* 支持更多的指令集
* ......
