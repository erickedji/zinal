
/**************************************************************************\

  e68k

  An environment for programming a MC68000

  Copyright (C) 1992-1996  Miguel Filgueiras, Universidade do Porto

----------------------------------------------------------------------------
  File:  e68k.h    Header file
----------------------------------------------------------------------------

   Permission is granted to anyone to make or distribute copies
   of this document provided that the copyright notice and this
   permission notice are preserved, thus giving the recipient
   permission to redistribute in turn.

   Permission is granted to distribute modified versions
   of this document, or of portions of it, under the above
   conditions, provided also that they carry prominent notices
   stating who last changed them.

\**************************************************************************/

/* FILE is used in this file so stdio must be included
 * added by Eric KEDJI */
#include <stdio.h>

/* OS type (added by pbv, 11/1994) */
#ifdef dos
#define DOS 1
#endif

/* Host machine architecture (added by pbv, 11/1994) 
   changed leftvalue() function to work correctly on big-endians;
   still doesn't implement a 68000 behaviour 
   (little-endian) correctly on that machines

   Changed by mig (11/1996); implementing BIGENDIAN MC68000 behaviour.
*/
#ifdef linux
   /* assumes a Intel processor */
#define LITTLEENDIAN 1
#define UNIX 1
#endif
#ifdef LINUX
#define LITTLEENDIAN 1
#define UNIX 1
#endif
#ifdef DOS
#define LITTLEENDIAN 1
#endif
#ifdef i386
#define LITTLEENDIAN 1
#endif
#ifdef i486
#define LITTLEENDIAN 1
#endif

#ifdef sparc
#define BIGENDIAN 1
#define UNIX 1
#endif
#ifdef SPARC
#define BIGENDIAN 1
#define UNIX 1
#endif
#ifdef mips
  /* probably a BIGENDIAN... */
#define BIGENDIAN 1
#define UNIX 1
#endif
#ifdef MIPS
#define BIGENDIAN 1
#define UNIX 1
#endif

/* otherwise, architecture should be defined
   like "-D...ENDIAN" in compile line */

/* Interactive commands */

char *Prompt = "e68k > ";

typedef enum { ASS,  /* must be the first one */
		 RUN, DEBUG, LIST, CLEAR, PFILE, EDIT, SETED, HELP,
		 BYE /* must be the last one  */
	       } CMMD;

char *CMDName[] = {
  "assemble", "run", "debug", "list", "clear", "file",
  "edit", "seted", "help", "quit" };

char *CMDExpl[] = {
  "Assemble a program in a file and load it into memory",
  "Run an assembled program in memory",
  "Run a program step by step and inspect values in memory",
  "Obtain a listing of the program in memory",
  "Clear the memory, destroying all its contents",
  "Define new program source file, and clear memory",
  "Edit program source file",
  "Define editor",
  "Get information on commands",
  "Exit from the emulator" };

/* Debugger stuff */

#define MAXBRK 32

int Debug, Step, BrkInx, Break[MAXBRK];

char *DPrompt = ":Deb68k:: ";

typedef enum { STEP,  /* must be the first one, see next defines */
		 CONT, NODEBUG, BREAK, DEL,
		      /* all the following may be used after program end */
		 PRREG,  /* must be the first of these, see next defines */
		 SETREG, PRMEM, SETMEM,
		 SHLN, DLIST, DHELP, RESTART,
		 QUIT /* must be the last one  */
	       } DCMMD;

#define RUNNING STEP
#define POSTMORTEM PRREG

char *DCMDName[] = {
  "step", "cont", "nodebug", "break", "delete",
  "regs", "setreg", "mem", "setmem",
  "line", "list", "help", "restart", "abort" };

char *DCMDExpl[] = {
  "Execute a single instruction",
  "Continue execution until next breakpoint",
  "Continue execution with no debugger control",
  "Set a breakpoint at a program line",
  "Delete a breakpoint",
  "Display contents of all registers",
  "Set value of register",
  "Display n memory bytes from given address",
  "Set values of n memory bytes from given address",
  "Show current line",
  "Obtain a listing of the program in memory",
  "Get information on debugger commands",
  "Restart program from the beginning",
  "Stop execution; debugging is turned off" };

/* Instruction attributes */

typedef enum { UNSIZED=0, B=1, W=2, BW=3, L=4, BL=5, WL=6, BWL=7,
		 BADATTR
	       } IATTR;

/* Argument types */

typedef enum {
   /* used when generating code: */
  NONE, /* no argument */
  D,  /* data register */
  A,  /* address register */
  I,  /* immediate */
  PrD, /* pre-decrement A */
  PtI, /* post-increment A */
  CR, /* condition register */
  LBL,  /* label (or absolute) */
  REG,  /* register */
  REGL,  /* register list */
  DISPL,  /* d(A) */
  DISPLREG,  /* d(A,X) */
  IND,    /* (A) */
  DISPLPC,  /* d(PC) */
  DISPLPCREG,  /* d(PC,X) */
   /* used in typing instruction arguments: */
  QI3, /* quick immediate 0-7 */
  QI8, /* quick immediate -128..127 */
  EA, /* any effective address */
  EAA, /* EA except A */
  EAADL,  /* EA except A, and D only with L */
  EAAIDL,  /* EA except A, except I, and D only with L */
  ALT, /* alterable = (A), (A)+ -(A) d(A) d(A,X) Lbl */
  ALTD, /* ALT + D */
  ALTDA,  /* ALT + D + A */
  ALTDL,  /* ALT + ( D only with L ) */
  ALTPtI,  /* ALT except PtI */
  PADDR, /* prog address = (A) d(A) d(A,X) Lbl d(PC) d(PC,X) */
  PADDRPtI  /* PADDR + PtI */
  } IARG;

/* Syntax */

#define CMTCHR ';'   /* comment delimiter */
#define MAXCLIST  128

typedef enum {
  ADD=0,  ADDA,  ADDI,  ADDQ,  ADDX,  AND,  ANDI,  ASL,  ASR,
  BCC,  BCS,  BEQ,  BGE,  BGT,  BHI,  BLE,  BLS,  BLT,  BMI,
  BNE,  BPL,  BVC,  BVS,  BCHG,  BCLR,  BRA,  BSET,  BSR,  BTST,
  CLR,  CMP,  CMPA,  CMPI,  CMPM,  DBCC,  DBCS,  DBEQ,  DBF,  DBGE,  DBGT,
  DBHI,  DBLE,  DBLS,  DBLT,  DBMI,  DBNE,  DBPL,  DBT,  DBVC,  DBVS,  DIVS,
  DIVU,  EOR,  EORI,  EXG,  EXT,  ILLEGAL,  JMP,  JSR,  LEA,  LINK,  LSL,
  LSR,  MOVE,  MOVEA,  MOVEM,  MOVEQ,  MULS,  MULU,
  NEG,  NEGX,  NOP,  NOT,  OR,  ORI,  PEA,  ROL,  ROR,  ROXL,
  ROXR,  RTR,  RTS,  SCC,  SCS,  SEQ,  SF,  SGE,  SGT,  SHI,  SLE,
  SLS,  SLT,  SMI,  SNE,  SPL,  ST,  SVC,  SVS,  SUB,  SUBA,  SUBI,
  SUBQ,  SUBX,  SWAP,  TAS,  TST,  UNLK,
		 ASSDIR,  /* must be before the assembler directives */
  GETC,  PUTC,  OPNF,  CLSF,  DS,  DC,  EQU,  END,  EXIT,
		 UNIMPL,  /* must be before unimplemented ones */
  ABCD,  CHK,  MOVEP,  NBCD,  SBCD,  TRAP,  TRAPV,
		 BADMNM  /* must be the last */
  } MNM;

typedef struct {
  IARG  lhs, rhs;
} ARGPAIR;

typedef struct {
  char mnem[8];
  IATTR attr;
  int ary;
  ARGPAIR  argpatt[4];
} IDEF;

IDEF  Instr[] = {
  {"ADD",BWL,2,{{EA,D},{D,ALT},{NONE}}},
  {"ADDA",WL,2,{{EA,A},{NONE}}},
  {"ADDI",BWL,2,{{I,ALTD},{NONE}}},
  {"ADDQ",BWL,2,{{QI3,ALTDA},{NONE}}},
  {"ADDX",BWL,2,{{D,D},{PrD,PrD},{NONE}}},
  {"AND",BWL,2,{{EAA,D},{D,ALT},{NONE}}},
  {"ANDI",BWL,2,{{I,ALTD},{I,CR},{NONE}}},
  {"ASL",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"ASR",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"BCC",UNSIZED,1,{{LBL},{NONE}}},
  {"BCS",UNSIZED,1,{{LBL},{NONE}}},
  {"BEQ",UNSIZED,1,{{LBL},{NONE}}},
  {"BGE",UNSIZED,1,{{LBL},{NONE}}},
  {"BGT",UNSIZED,1,{{LBL},{NONE}}},
  {"BHI",UNSIZED,1,{{LBL},{NONE}}},
  {"BLE",UNSIZED,1,{{LBL},{NONE}}},
  {"BLS",UNSIZED,1,{{LBL},{NONE}}},
  {"BLT",UNSIZED,1,{{LBL},{NONE}}},
  {"BMI",UNSIZED,1,{{LBL},{NONE}}},
  {"BNE",UNSIZED,1,{{LBL},{NONE}}},
  {"BPL",UNSIZED,1,{{LBL},{NONE}}},
  {"BVC",UNSIZED,1,{{LBL},{NONE}}},
  {"BVS",UNSIZED,1,{{LBL},{NONE}}},
  {"BCHG",BL,2,{{D,ALTDL},{I,ALTDL},{NONE}}},
  {"BCLR",BL,2,{{D,ALTDL},{I,ALTDL},{NONE}}},
  {"BRA",UNSIZED,1,{{LBL},{NONE}}},
  {"BSET",BL,2,{{D,ALTDL},{I,ALTDL},{NONE}}},
  {"BSR",UNSIZED,1,{{LBL},{NONE}}},
  {"BTST",BL,2,{{D,EAADL},{I,EAAIDL},{NONE}}},
  {"CLR",BWL,1,{{ALTD},{NONE}}},
  {"CMP",BWL,2,{{EA,D},{D,ALT},{NONE}}},
  {"CMPA",WL,2,{{EA,A},{NONE}}},
  {"CMPI",BWL,2,{{I,ALTD},{NONE}}},
  {"CMPM",BWL,2,{{PtI,PtI},{NONE}}},
  {"DBCC",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBCS",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBEQ",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBF",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBGE",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBGT",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBHI",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBLE",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBLS",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBLT",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBMI",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBNE",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBPL",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBT",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBVC",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DBVS",UNSIZED,2,{{D,LBL},{NONE}}},
  {"DIVS",UNSIZED,2,{{EAA,D},{NONE}}},
  {"DIVU",UNSIZED,2,{{EAA,D},{NONE}}},
  {"EOR",BWL,2,{{EAA,D},{D,ALT},{NONE}}},
  {"EORI",BWL,2,{{I,ALTD},{I,CR},{NONE}}},
  {"EXG",UNSIZED,2,{{REG,REG},{NONE}}},
  {"EXT",WL,1,{{D},{NONE}}},
  {"ILLEGAL",UNSIZED,0},
  {"JMP",UNSIZED,1,{{PADDR},{NONE}}},
  {"JSR",UNSIZED,1,{{PADDR},{NONE}}},
  {"LEA",UNSIZED,2,{{PADDR,A},{NONE}}},
  {"LINK",UNSIZED,2,{{A,I},{NONE}}},
  {"LSL",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"LSR",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"MOVE",BWL,2,{{EA,ALTD},{EA,CR},{NONE}}},
  {"MOVEA",WL,2,{{EA,A},{NONE}}},
  {"MOVEM",WL,2,{{REGL,ALTPtI},{PADDRPtI,REGL},{NONE}}},
  {"MOVEQ",UNSIZED,2,{{QI8,D},{NONE}}},
  {"MULS",UNSIZED,2,{{EAA,D},{NONE}}},
  {"MULU",UNSIZED,2,{{EAA,D},{NONE}}},
  {"NEG",BWL,1,{{ALTD},{NONE}}},
  {"NEGX",BWL,1,{{ALTD},{NONE}}},
  {"NOP",UNSIZED,0},
  {"NOT",BWL,1,{{ALTD},{NONE}}},
  {"OR",BWL,2,{{EAA,D},{D,ALT},{NONE}}},
  {"ORI",BWL,2,{{I,ALTD},{I,CR},{NONE}}},
  {"PEA",UNSIZED,1,{{PADDR},{NONE}}},
  {"ROL",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"ROR",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"ROXL",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"ROXR",BWL,2,{{D,D},{QI3,D},{ALT,NONE},{NONE}}},
  {"RTR",UNSIZED,0},
  {"RTS",UNSIZED,0},
  {"SCC",UNSIZED,1,{{ALTD},{NONE}}},
  {"SCS",UNSIZED,1,{{ALTD},{NONE}}},
  {"SEQ",UNSIZED,1,{{ALTD},{NONE}}},
  {"SF",UNSIZED,1,{{ALTD},{NONE}}},
  {"SGE",UNSIZED,1,{{ALTD},{NONE}}},
  {"SGT",UNSIZED,1,{{ALTD},{NONE}}},
  {"SHI",UNSIZED,1,{{ALTD},{NONE}}},
  {"SLE",UNSIZED,1,{{ALTD},{NONE}}},
  {"SLS",UNSIZED,1,{{ALTD},{NONE}}},
  {"SLT",UNSIZED,1,{{ALTD},{NONE}}},
  {"SMI",UNSIZED,1,{{ALTD},{NONE}}},
  {"SNE",UNSIZED,1,{{ALTD},{NONE}}},
  {"SPL",UNSIZED,1,{{ALTD},{NONE}}},
  {"ST",UNSIZED,1,{{ALTD},{NONE}}},
  {"SVC",UNSIZED,1,{{ALTD},{NONE}}},
  {"SVS",UNSIZED,1,{{ALTD},{NONE}}},
  {"SUB",BWL,2,{{EA,D},{D,ALT},{NONE}}},
  {"SUBA",WL,2,{{EA,A},{NONE}}},
  {"SUBI",BWL,2,{{I,ALTD},{NONE}}},
  {"SUBQ",BWL,2,{{QI3,ALTDA},{NONE}}},
  {"SUBX",BWL,2,{{D,D},{PrD,PrD},{NONE}}},
  {"SWAP",UNSIZED,1,{{D},{NONE}}},
  {"TAS",UNSIZED,1,{{ALTD},{NONE}}},
  {"TST",BWL,1,{{ALTD},{NONE}}},
  {"UNLK",UNSIZED,1,{{A},{NONE}}},
  {""},   /* directives after this one */
     /* arity>0 for directive implies treatment as normal instruction */
  {"GETC",UNSIZED,2,{{D,D},{D,NONE},{NONE}}},
  {"PUTC",UNSIZED,2,{{D,D},{D,NONE},{NONE}}},
  {"OPNF",UNSIZED,2,{{A,D},{NONE}}},
  {"CLSF",UNSIZED,1,{{D},{NONE}}},
  {"DS",BWL,-1},
  {"DC",BWL,-1},
  {"EQU",UNSIZED,-1},
  {"END",UNSIZED,0},
  {"EXIT",UNSIZED,0},
  {""},   /* unimplemented after this one */
  {"ABCD"},
  {"CHK"},
  {"MOVEP",WL,2,{{D,DISPL},{DISPL,D},{NONE}}},
  {"NBCD"},
  {"SBCD"},
  {"TRAP"},
  {"TRAPV"}
};

#define MAXERR 10

/* "Code" representation */

#define MAXCODE 5000
#define MAXLABL 1000
#define MAXLBLLGTH  9
/*
 * Eric KEDJI: mimic S.E.R.I.E Electronics
 * #define UNDEF  (0xAAAFBAAA)
 * #define ORIGIN  0x1000
 */
#define UNDEF (0x00000000)
#define ORIGIN (0x0)

int  CInx, Origin;

typedef signed long int ADDR;          /* must be 32-bits long */
typedef unsigned long int ULONG;       /* same */
typedef signed short int WORD;         /* must be 16-bits long */
typedef signed char BYTE;              /* must be signed 8-bits long */

ADDR InstrLgth;

typedef struct {
    ADDR caddri;  MNM mnmi;  IATTR attri;
    IARG targi[2];  ADDR valargi[2][3];
} MCHINSTR;

MCHINSTR Code[MAXCODE];

/* Arguments of instructions:

     type             #values       which values

     CR, REG,
     IND, PtI, PrD    1            RGT
     REGL             1            mask (A0=lower,...,A7,D0,...,D7=higher)
     LBL              2            index in LblDef, IATTR (default L)
     DISPL, DISPLPC   2            index in LblDef, RGT
     DISPL(PC)REG     3            index in LblDef, RGT, (RGT or -RGT if L)
     I                2            (value, I) or (index in LblDef, LBL)
*/

/* Labels */

typedef enum { PROGLAB, DATALAB, CONSTLAB }  LABTYPE;

typedef struct {
    char lname[MAXLBLLGTH];
    LABTYPE ltype;
    ADDR lcinx;
} LBLDEF;

LBLDEF LblDef[MAXLABL];

int LblInx;

/* Data segment */
/*
#define DATAORIGIN  (0x0)
#define DATATOP     (0x4000)    must be multiple of 4 ! */

/* Above 2 define commented out by Eric KEDJI on 20-02-2007
 * to have memory adresses exactly like those of the mc68000
 * used in lab
 */
#define DATAORIGIN  (0xFE0000) /* start RAM1 */
#define DATATOP     (0xFFFFFC)   /* must be multiple of 4 ! */ /* end RAM2 */

BYTE Data[DATATOP-DATAORIGIN+1];

int DInx;

/* Text */

#define NOINFO  (-DATATOP-1)

int Txtln[MAXCODE], TxtInx;

/* Registers */

typedef enum { PC=0, A0, A1, A2, A3, A4, A5, A6, A7, SP,
		 D0, D1, D2, D3, D4, D5, D6, D7, CCR,
		 BADREG
		 }  RGT;

char *RegName[] = { "PC", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
		     "SP",
		     "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "CCR" };


/* Simulation */

typedef enum { NOERR=0, ADDRERR, BUSERR, ILLIERR, UNIMPLERR, DEBUGQUIT,
		 SEGFAULT, DIVBY0,
		 PROGEXIT /* must be the last */
		 } EXECERR;

char *ExErrMess[] = {
  "",  "Address error",  "Bus error", "Illegal instruction",
  "Unimplemented instruction", "User quit", "Segmentation fault",
  "Division by zero",
  "Exit from program"};

EXECERR ExErr;

ADDR Reg[1+9+8+1],
  BShift[] = { 0, 24, 16, 0, 0 },
  MaskA[] = { 0, 0xFF, 0xFFFF, 0, 0xFFFFFFFF },
#ifdef LITTLEENDIAN
  MaskLE[] = { 0, 0xFFFFFF, 0xFFFF, 0, 0 },
#endif
  MaskH[] = { 0, 0x80, 0x8000, 0, 0x80000000 },
  MaskB[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
		0x100, 0x200, 0x400, 0x800,
		0x1000, 0x2000, 0x4000, 0x8000,
		0x10000, 0x20000, 0x40000, 0x80000,
		0x100000, 0x200000, 0x400000, 0x800000,
		0x1000000, 0x2000000, 0x4000000, 0x8000000,
		0x10000000, 0x20000000, 0x40000000, 0x80000000 },
  MaskCB[] = { 0x1, 0x2, 0x4, 0x8, 0x10 };

typedef enum {C, V, Z, N, X} CONDBIT;

typedef enum {CC, CS, EQ, GE, GT, HI, LE, LS, LT, MI, NE, PL, VC, VS,
	      XS}  COND;

#define SIZE  (Code[ExCInx].attri)
#define ARG1  (Code[ExCInx].valargi[0][0])
#define ARG2  (Code[ExCInx].valargi[1][0])
#define PARG1  (&(Code[ExCInx].valargi[0][0]))
#define PARG2  (&(Code[ExCInx].valargi[1][0]))
#define TARG1  (Code[ExCInx].targi[0])
#define TARG2  (Code[ExCInx].targi[1])

int ExCInx;

#define TRUE(x) (x ? 0xFF : 0)
#define RMBITSET(x,a) (x&MaskH[a])
#define ISZERO(x,a) (!(x&MaskA[a]))

/* Varia */

#define BLANK(c)  (c && c <= ' ')
#define ALPHA(c)  (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
#define ALPHANUM(c)  (('a'<=c && c<='z')||('A'<=c && c<='Z')||('0'<=c && c<='9')||c=='_')
#define DIGIT(c)  ('0'<=c && c<='9')
#define NUMIN(c)  (('0'<=c && c<='9') || c=='@' || c=='%' || c=='$')
#define QUOTE  39     /* because of strange error of Sun cc */

#define TTY_ROWS 22   /* listing screen size */

/* OS-dependent stuff */
/* files, times etc. --- UNIX only */
#ifdef UNIX
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

/* Default editor (added by pbv, 11/1994) */
#ifndef DEFAULT_EDITOR
#ifdef UNIX
#define DEFAULT_EDITOR "vi"
#endif
#ifdef DOS
#define DEFAULT_EDITOR "edit"
#endif
#endif

#define MAXUFILE 5

FILE *ProgF, *UserF[MAXUFILE];

int MaxUF;

char ProgFName[120], EditorName[120];

#ifdef UNIX
time_t ProgVersion;
#endif

/**************************************************************************
  end of include file
  Last revision: 11/1994, by pbv.
  Last revision: 11/1996, by mig, nam, pbv, rvr
  Last revision: 02/2007, by KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
  *************************************************************************/
