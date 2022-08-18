`define	INSTBUS	79:0
`define	ICODE	79:76
`define	IFUN	75:72
`define	RA	    71:68
`define	RB	    67:64
`define	V	    63:0
`define	D	    63:0
`define	Dest	75:8
`define	WORD	63:0

/* icode */
`define     IHALT           4'H0 // halt
`define     INOP            4'H1 // nop
`define     IRRMOVQ         4'H2 // rrmovq
`define     IIRMOVQ         4'H3 // irmovq
`define     IRMMOVQ         4'H4 // rmmovq
`define     IMRMOVQ         4'H5 // mrmovq
`define     IOPQ            4'H6 // op
`define     IJXX            4'H7 // jxx
`define     ICALL           4'H8 // call
`define     IRET            4'H9 // ret
`define     IPUSHQ          4'HA // pushq
`define     IPOPQ           4'HB // popq
//`define	 	IIADDQ			4'HC


/* ifun */
/* op */
//`define     FNONE           4'H0
`define     FADDQ           4'H0 // add
`define     FSUBQ           4'H1 // sub
`define     FANDQ           4'H2 // and
`define     FXORQ           4'H3 // xor

/* jxx */
`define     FJMP            4'H0 // jmp
`define     FJLE            4'H1 // jle
`define     FJL             4'H2 // jl
`define     FJE             4'H3 // je
`define     FJNE            4'H4 // jne
`define     FJGE            4'H5 // jge
`define     FJG             4'H6 // jg
`define     FRRMOVQ         4'H0 // rrmovq

/* cmovxx */
`define     FCMOVLE         4'H1 // cmovle
`define     FCMOVL          4'H2 // cmovl
`define     FCMOVE          4'H3 // cmove
`define     FCMOVNE         4'H4 // cmovne
`define     FCMOVGE         4'H5 // cmovge
`define     FCMOVG          4'H6 // cmovg

/* Register Codes */
`define     RRSP            4'H4 // %rsp
`define     RNONE           4'HF

/* Status Codes */
`define     SAOK            4'H1
`define     SADR            4'H2
`define     SINS            4'H3
`define     SHLT            4'H4

/* Other Codes */
`define     ENABLE          1'b1
`define     DISABLE         1'b0
`define     ALUADD          4'H0
