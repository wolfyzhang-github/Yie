`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/18/2022 10:31:12 PM
// Design Name: 
// Module Name: Execute
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

module Execute(
    input  wire               clk,
    input  wire               rst,
    input  wire        [3:0]  icode,
    input  wire        [3:0]  ifun,
    input  wire signed [63:0] valA,
    input  wire signed [63:0] valB, 
    input  wire signed [63:0] valC,
    output reg  signed [63:0] valE,
    output reg                Cnd
);
    reg sf, zf, of;

    reg signed [63:0] aluA;
    reg signed [63:0] aluB;
    
	always @(*)
	begin
   		 case(icode)
   		 	`IHALT:
   		 	begin
   		 		$stop;
   		 	end
    		`IRRMOVQ:
    		begin
    		// 根据不同的ifun做不同操作
   				aluA = valA;
  				aluB = 4'H0;
    			valE = aluA + aluB;
    			case(ifun)
					`FRRMOVQ:
					begin
						Cnd = 1;
					end
					`FCMOVLE:
					begin
						Cnd = (sf ^ of) | zf;
					end
					`FCMOVL:
					begin
						Cnd = (sf ^ of);
					end
					`FCMOVE:
					begin
						Cnd = zf;
					end
					`FCMOVNE:
					begin
						Cnd = ~zf;
					end
					`FCMOVGE:
					begin
						Cnd = ~(sf ^ of);
					end
					`FCMOVG:
					begin
						Cnd = ~(sf ^ of) & ~zf;
					end
    			endcase
    		end
    		`IIRMOVQ:
    		begin
				aluA = valC;
				aluB = 4'H0;
				valE = aluA	+ aluB;
			end
			`IRMMOVQ:
			begin
				aluA = valC;
				aluB = valB;
				valE = aluA + aluB;
    		end
			`IMRMOVQ:
			begin
				aluA = valC;
				aluB = valB;
				valE = aluA	+ aluB;
    		end
			`IJXX:
			begin
				case (ifun )
					`FJMP:
					begin
						Cnd = 1;
					end
					`FJLE:
					begin
						Cnd = (sf ^ of) | zf;
					end
					`FJL:
					begin
						Cnd = (sf ^ of);
					end
					`FJE:
					begin
						Cnd = zf;
					end
					`FJNE:
					begin
						Cnd = ~zf;
					end
					`FJGE:
					begin
						Cnd = ~(sf ^ of);
					end
					`FJG:
					begin
						Cnd = ~(sf ^ of) & ~zf;
					end
				endcase
    		end
			`ICALL:
			begin
				aluA = -4'H8;
				aluB = valB;
				valE = aluA	+ aluB;
			end
			`IRET:
			begin
				aluA = 4'H8;
				aluB = valB;
				valE = aluA	+ aluB;
			end
			`IPUSHQ:
			begin
				aluA = -4'H8;
				aluB = valB;
				valE = aluA	+ aluB;
			end
			`IPOPQ:
			begin
				aluA = 4'H8;
				aluB = valB;
				valE = aluA	+ aluB;
			end
			default:
			begin
			     ;
			end
    	endcase
	end
	
	/* 下降沿时进行可以改变标志位的运算，保证一个周期内只执行一次，避免发生错误 */
	/* SEQ需要在一个时钟周期内完成一条指令的所有操作，所以改变标志位的操作要保证只执行一次 */
	always @(negedge clk)
	begin
		case(icode)
			`IOPQ:
			begin
				aluA = valA;
				aluB = valB;
				case(ifun)
					`FADDQ:
					begin
						valE = aluA	+ aluB;
					end
					`FSUBQ:
					begin
						valE = aluB	- aluA;
					end
					`FANDQ:
					begin
						valE = aluA	& aluB;
					end
					`FXORQ:
					begin
						valE = aluA	^ aluB;
					end
					default:
					begin
					    ;
					end
				endcase
				zf = (valE == 0) ;
				sf = (aluB < aluA) ;
				of = (valA<0 == valB<0) && (valE<0 != valA<0);
			end
		endcase
	end
    
endmodule
