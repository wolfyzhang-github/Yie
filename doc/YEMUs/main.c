/*
 * 假设一个 8 位模型机，有 4 个通用寄存器 r0-r3，有 16 个内存单元 m0-m15.
 * 每个内存单元 ALU、通用寄存器、IR（指令寄存器）、MDR（主存数据寄存器）的宽度都是 8 位.
 * PC 和 MAR（主存地址寄存器）的宽度都是 4 位，总线中有 4 位地址线、8 位数据线和若干位信号线.
 *
 * 该模型机采用 8 位定长指令字，每条指令的长度为 8 位，指令格式有 R 型和 M 型两种.
 *
 * 指令格式：
 *
 *   格式     4 位    2 位   2 位                功能说明
 *   ------------------------------------------------------------------------
 *     R 型 |  op  |  rt  |  rs  | R[rt] <- R[rt] op R[rs]，或 R[rt] <- R[rs]
 *   ------------------------------------------------------------------------
 *     M 型 |  op  |    addr     | R[0] <- M[addr]，或 M[addr] <- R[0]
 *
 *   op：指令类型，取值为mov、add、load 或 store.
 *
 * 指令手册：
 *
 *      汇编指令           数据指令               功能类型     内存信息
 *  ------------------------------------------------------------------
 *    mov   rt,rs  |  R[rt] <- R[rs]          |  R 型  |  0000 rt rs
 *  ------------------------------------------------------------------
 *    add   rt,rs  |  R[rt] <- R[rs] + R[rt]  |  R 型  |  0001 rt rs
 *  ------------------------------------------------------------------
 *    load  addr   |  R[0] <- M[addr]         |  M 型  |  1110 addr
 *  ------------------------------------------------------------------
 *    store addr   |  M[addr] <- R[0]         |  M 型  |  1111 addr
 *
 * 如下实现了一个该模型机的指令执行器.
 */

#include <stdint.h>
#include <stdio.h>

#define NREG 4   // 寄存器位宽
#define NMEM 16  // 内存位宽


/*
 * 操作数译码的实现宏
 * 说明：操作数译码即从指令中提取出操作数.
 *      以 DECODE_R(inst) 为例，inst 为传入参数，即指令.
 *
 *      该宏定义了两个 uint8_t 变量 rt 和 rs（ 8 位无符号），用于存放操作数.
 *      利用指令格式 union 中 struct 的成员来获取操作数并赋值给 rt 和 rs.
 */
#define DECODE_R(inst) uint8_t rt = (inst).rtype.rt, rs = (inst).rtype.rs
#define DECODE_M(inst) uint8_t addr = (inst).mtype.addr

/*
 * 定义指令格式（即指令模板）
 * 说明：uint8_t 实际为 unsigned char 类型，即 8 位无符号整数，以此来代表对应寄存器中的数据.
 *      定义一个新的数据类型 inst_t，具体构造为一个 union.
 *      union 中的成员变量为 2 个 struct 和 1 个 uint8_t.
 *
 *      rtype 结构变量代表 R 型指令，mtype 结构变量代表 M 型指令，以下以 rtype 为例.
 *      rtype 的成员变量为：op、rt 和 rs，分别占用各自的位字段，共 8 位，即一个 uint8_t 类型的长度.
 *
 *      指令格式通过 struct 描述，但我们还需要一个 8 位的变量 inst 来存储指令的内容（指代存放指令的内存），
 *      之后利用 struct 中的成员变量来解析指令中的不同部分（利用 struct 描述的指令格式）.
 *
 *      union 中所有成员占用同一块内存，可利用它的这个性质，首先将指令写入（假定指令存储变量为 this）this.inst 中，
 *      然后通过 this.rtype.op 和 this.rtype.rt 即可获取指令操作码和操作数.
 *      即对于同一块内存，this.inst 和 this.rtype.op 和 this.rtype.rt 可以互相访问，
 *      我们是以变化的视角来看待这块不变的内存.
 */
typedef union {
    struct { uint8_t rs : 2, rt : 2, op : 4; } rtype;
    struct { uint8_t addr : 4      , op : 4; } mtype;
    uint8_t inst;
} inst_t;

/*
 * 定义存储器
 */
uint8_t pc = 0;         // PC，C 语言中没有 4 位的数据类型，采用 8 位类型来表示
uint8_t R[NREG] = {};   // 寄存器，一个长度为 NREG 的 uint8_t 数组
uint8_t M[NMEM] = {     // 内存，一个长度为 NMEM 的 uint8_t 数组，其中包含一个计算 z = x + y 的程序，x、y 均已定义
        0b11100110,  // load  r0, 6# | R[0] <- M[6] = y
        0b00000100,  // mov   r1, r0 | R[1] <- R[0]
        0b11100101,  // load  r0, 5# | R[0] <- M[5] = x
        0b00010001,  // add   r0, r1 | R[0] <- R[0] + R[1]
        0b11110111,  // store 7#, r0 | M[7] = z <- R[0]
        0b00010000,  // x = 16
        0b00100001,  // y = 33
        0b00000000,  // z = 0
};

int halt = 0;           // 结束标志

/*
 * 执行一条指令
 */
void exec_once() {
    inst_t this;        // 定义当下执行的指令，即指令存储变量，实质上它指代的是一块内存，其中存储了一条指令
    this.inst = M[pc];  // 从 pc 指向内存地址（该内存数组的索引）的存储单元中读取一条指令，
                        // 并将上述内存视作一个 uint8_t 变量，将取到的指令赋值给其，
                        // 留待后续利用该 union 中 struct 的成员变量来解析指令中的不同部分.
    switch (this.rtype.op) {
        //   操作码译码  操作数译码       执行
        case 0b0000: { DECODE_R(this); R[rt]   = R[rs];                  break; }
        case 0b0001: { DECODE_R(this); R[rt]  += R[rs];                  break; }
        case 0b1110: { DECODE_M(this); R[0]    = M[addr];                break; }
        case 0b1111: { DECODE_M(this); M[addr] = R[0]; halt = 1; pc = -1; break; }
        default:
            printf("Invalid instruction with opcode = %x, halting...\n", this.rtype.op);
            halt = 1;
            break;
    }
    pc++; // 更新PC
}

/*
 * 入口程序
 * 说明：main() 中包含一个死循环，循环体中调用 exec_once() 来执行一条指令.
 *      直至 halt 为 1，即有结束标志时，循环结束，输出计算结果.
 */
int main() {
    unsigned int x, y;
    uint8_t x_t, y_t;
    char buff;

    printf("8 bits computer simulator v1.0, currently only supports addition.\n");
    while (1) {
        printf("Please enter the first operand(0-255): ");
        scanf("%u", &x);
        printf("Please enter the second operand(0-255): ");
        scanf("%u", &y);
        if ((x + y)> 255) {
            printf("The result is too large, please try again.\n");
            continue;
        }
        M[5] = (x_t = x);
        M[6] = (y_t = y);
        while (1) {
            exec_once();
            if (halt) break;
        }
        printf("The result of %d + %d is %d\n", M[5], M[6], M[7]);
        halt = 0;
    }
    return 0;
}