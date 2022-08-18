`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/18/2022 11:01:38 PM
// Design Name: 
// Module Name: Update
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

module Update(
    input  wire         clk,
    input  wire         rst,
    input  wire [3:0]   icode,
    input  wire         Cnd,
    input  wire [`WORD] valP,
    input  wire [`WORD] valM,
    input  wire [`WORD] valC,
    output reg  [`WORD] pc
);
    
    always @(posedge clk)
    begin
    	case(icode)
			`ICALL: pc = valC;
			`IJXX:	pc = Cnd == 1 ? valC : valP;
			`IRET:	pc = valM;
			default:
			begin
                if(valP > 0) 
                    pc = valP;
                else	
                    pc = 0;
			end
    	endcase
    end
   	
endmodule
