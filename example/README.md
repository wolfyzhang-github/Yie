一个数组求和的样例程序，等效的C代码如下：

```c
long sum(long *arr, long count) {
    long sum = 0;
    while (count) {
        sum += *arr;
        arr++;
        count--;
    }
    return sum;
}
```

值得说明的是，文件夹内的`assembler.py`脚本并不支持对内存对齐的`.align`指令，所以`sum.yo`文件中的`.align`指令将被忽略。~~原因是我懒得加这个feature了。~~

不过，为了还原程序真实的运行状态，我还是在转译完毕的`sum.bin`二进制文件中手动添加一部分为内存对齐占位的0......