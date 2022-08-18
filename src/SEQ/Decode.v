`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/18/2022 10:21:40 PM
// Design Name: 
// Module Name: Decode
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
module	Decode(
    input  wire clk,
    input  wire rst,
    input  wire [3:0] icode,
    input  wire [3:0] ifun,
    input  wire [3:0] rA,
    input  wire [3:0] rB,
    output reg  [3:0] srcA,
    output reg  [3:0] srcB
);
	
	// Decode
    always @(*)
    begin
    // decode阶段比较简单，匹配icode之后只是一些赋值操作
   		case(icode)
			`INOP:
			begin
    			srcA	<= `RNONE;
    			srcB	<= `RNONE;
    		end
    		`IHALT:
    		begin
    			srcA	<= `RNONE;
    			srcB	<= `RNONE;
    		end 
    		`IRRMOVQ:
    		begin
    			srcA	<=  rA;
    			srcB	<= `RNONE;
    		end
    		`IIRMOVQ:
    		begin
    			srcA	<= `RNONE;
    			srcB	<= `RNONE;
    		end
    		`IRMMOVQ:
    		begin
    			srcA	<=  rA;
    			srcB    <=  rB;
    		end
    		`IMRMOVQ:
    		begin
				srcA	<= `RNONE;
    			srcB    <=  rB;
			end
    		`IOPQ:
    		begin
   				srcA	<=  rA;
   				srcB    <=  rB;
   			end
			`IJXX:
			begin
    	 		srcA	<= `RNONE;
    	 		srcB	<= `RNONE;
			end
			`ICALL:
			begin
    	 		srcA	<= `RNONE;
    	 		srcB    <=  `RRSP;
			end
			`IRET:
			begin
    	 		srcA	<=  `RRSP;
    	 		srcB    <=  `RRSP;
			end
			`IPUSHQ:
			begin
    	 		srcA	<=  rA;
   		 		srcB    <=  `RRSP;
			end
			`IPOPQ:
			begin
    	 		srcA	<=  `RRSP;
   		 		srcB    <=  `RRSP;
			end
		endcase
    end
	
endmodule
