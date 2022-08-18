`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/18/2022 09:58:03 PM
// Design Name: 
// Module Name: Fetch
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


`include "defines.v"

module Fetch( 
    input              clk,
    input              rst,
    input [`WORD]      pc,
    input [`INSTBUS]   inst,
    output reg [3:0]   icode,
    output reg [3:0]   ifun,
    output reg [3:0]   rA,
    output reg [3:0]   rB,
    output reg [`WORD] valP,
    output reg [`WORD] valC
);
			
	reg [`WORD] valN;  // 转小端

	//fetch icode and ifun
	always @(*)
	begin
		// case匹配相当于fetch icode和ifun，具体操作在case内部完成
		icode = inst[`ICODE];
		ifun  = inst[`IFUN];
		case(icode)
			`INOP: // nop
			begin
			// 更新valP
				valP = pc + 64'h1;
			end
			`IHALT: //halt
			begin
			// 同上
				valP = pc + 64'h1;
			end 
			`IRRMOVQ: //rrmovq
			begin
			// 取rA和rB，操作valP
				valP = pc + 64'h2;
				rA = inst[`RA];
				rB = inst[`RB];
			end
			`IIRMOVQ: //irmovq
			begin
			// rA用不到寄存器，置F；取rB和valC，valC为小端存储，进行拼合
				valP = pc + 64'hA;
				rA = 4'hF;
				rB = inst[`RB];
				valN = inst[`V];
				valC = {valN[7:0], valN[15:8], valN[23:16], valN[31:24], valN[39:32], valN[47:40], valN[55:48], valN[63:56]};
			end
			`IRMMOVQ: // rmmovq
			begin
			// 同上
				valP = pc + 64'hA;
				rA = inst[`RA];
				rB = inst[`RB];
				valN = inst[`D];
				valC = {valN[7:0], valN[15:8], valN[23:16], valN[31:24], valN[39:32], valN[47:40], valN[55:48], valN[63:56]};
			end
			`IMRMOVQ: // mrmovq
			begin
			// 同上
				valP = pc + 64'hA;
				rA = inst[`RA];
				rB = inst[`RB];
				valN = inst[`D];
				valC = {valN[7:0], valN[15:8], valN[23:16], valN[31:24], valN[39:32], valN[47:40], valN[55:48], valN[63:56]};
			end
			`IOPQ: // op
			begin
			// 取rA和rB，更新valP
				valP = pc + 64'h2;
				rA = inst[`RA];
				rB = inst[`RB];
			end
			`IJXX: // jxx
			begin
			// 取Dest（小端拼接）,更新valP；具体跳转与否这类微结构操作在另外的阶段实现
				valP = pc + 64'h9;
				valN = inst[`Dest];
				valC = {valN[7:0], valN[15:8], valN[23:16], valN[31:24], valN[39:32], valN[47:40], valN[55:48], valN[63:56]};
			end
			`ICALL: //call
			begin
			// 同上
				valP = pc + 64'h9;
				valN = inst[`Dest];
				valC = {valN[7:0], valN[15:8], valN[23:16], valN[31:24], valN[39:32], valN[47:40], valN[55:48], valN[63:56]};
			end
			`IRET: // ret
			begin
            // fetch阶段只需要更新valP
				valP = pc + 64'h1;
			end
			`IPUSHQ: // pushq
			begin
			// 取rA和rB，更valP
				valP = pc + 64'h2;
				rA = inst[`RA];
				rB = inst[`RB];
			end
			`IPOPQ: // popq
			begin
			// 同上
				valP = pc + 64'h2;
				rA = inst[`RA];
				rB = inst[`RB];
			end
			default: $stop; // invaild inst
		endcase	
	end

endmodule