`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/18/2022 10:40:14 PM
// Design Name: 
// Module Name: Write
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

module Write(
    input  wire       clk,
    input  wire       rst,
    input  wire       Cnd,
    input  wire [3:0] icode,
    input  wire [3:0] rA,
    input  wire [3:0] rB,
    output reg  [3:0] dstM,
    output reg  [3:0] dstE
);
    
   	 /* 基本都是一些赋值操作 */ 
    always @(*)
    begin
		case(icode)
			`INOP:
			begin
				dstM = `RNONE;
				dstE = `RNONE;
			end
			`IHALT:
			begin
				dstM = `RNONE;
				dstE = `RNONE;
			end 
			`IRRMOVQ:
			begin
				dstE = Cnd == 1 ? rB : `RNONE;
				dstM = `RNONE;
			end
			`IIRMOVQ:
			begin
				dstM = `RNONE;
				dstE = rB;
			end
			`IRMMOVQ:
			begin
				dstM = `RNONE;
				dstE = `RNONE;
			end
			`IMRMOVQ:
			begin
				dstM = rA;
				dstE = `RNONE;
			end
			`IOPQ:
			begin
				dstM = `RNONE;
				dstE = rB;
			end
			`IJXX:
			begin
				dstM = `RNONE;
				dstE = `RNONE;
			end
			`ICALL:
			begin
				dstM = `RNONE;
				dstE = `RRSP;
			end
			`IRET:
			begin
				dstM = `RNONE;
				dstE = `RRSP;
			end
			`IPUSHQ:
			begin
				dstM = `RNONE;
				dstE = `RRSP;
			end
			`IPOPQ:
			begin
				dstM = rA;
				dstE = `RRSP;
			end
		endcase
    end
    
endmodule
