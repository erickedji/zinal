
/**************************************************************************\

  e68k

  An environment for programming a MC68000

  Copyright (C) 1992-1996  Miguel Filgueiras, Universidade do Porto

----------------------------------------------------------------------------
  File:  e68k.c    Main program file
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> /* Eric KEDJI: I'am replacing the character handling
                      macros with calls to the function defined in
                      ctype.h : the good old days are over ! */
#include "e68k.h"

#define VERSION "2.0.4"
/* 
************ Version Reports ****************
Version 2.0.4 mig
Fixed bad usage of setreg() and setregc()
Fixed bug in BCLR

Version 2.0.3 mig, nam
Adds new command to debugger: restart
Fixed bug in setting bits N, Z in CCR

Version 2.0.2 mig
Fixed bug in dealing with PC in jumpmaddr()

Version 2.0.1 mig
Fixed bug in dealing with registers through rightvalue() and setatlv().

Version 2.0 mig nam pbv rvr
Fixed LITTLE/BIG-ENDIAN problems in accessing memory and
provides emulation of BIG-ENDIAN behaviour of MC68000
if compiled with LITTLEENDIAN or BIGENDIAN options correctly
set for the target architecture.

Version 1.9.1 nam
Fixed MULU bug : arg1 must be .W

*********************************************
*/

/** Utilities */

#ifdef LITTLEENDIAN
ADDR chgrepr(v,a)      /* changes representation from/to BIGENDIAN */
ADDR v;  IATTR a;
{
  switch (a) {
  case B:  return(v&0xFF);
    break;
  case W:  return(((v&0xFF)<<8) + ((v&0xFF00)/256));
    break;
  case L:  return(((v&0xFF)<<24) + ((v&0xFF00)<<8) +
               ((v&0xFF0000)/256) + (((v&0xFF000000)>>24)&0xFF));
  }
}
#endif

ADDR length(v,a)    /* length in bytes of v cells attribute a */
ADDR v;  IATTR a;
{
  switch (a) {
  case B:  return(v);
  case W:  return(v+v);
  case L:  return(v<<2);
  }
}

ADDR myabs(v)   /* absolute value */
ADDR v;
{
  if (v < 0)  return(-v);
  return(v);
}

void blccpy(p,v,a,d)  /* copy block from p, v cells attribute a to d */
ADDR *p, v;  IATTR a;  BYTE *d;
{ WORD *wp;  ADDR *lp;

  switch (a) {
  case B:
    while (v--)  *d++ = (BYTE) *p++;
    return;
  case W:
    wp = (WORD *) d;
#ifdef LITTLEENDIAN
    while (v--)  *wp++ = chgrepr((WORD) *p++,W);
#else /* BIGENDIAN */
    while (v--)  *wp++ = (WORD) *p++;
#endif
    return;
  case L:
    lp = (ADDR *) d;
#ifdef LITTLEENDIAN
    while (v--)  *lp++ = chgrepr(*p++,L);
#else /* BIGENDIAN */
    while (v--)  *lp++ = *p++;
#endif
    return;
  }
}

/** String utilities **/

char *skipblanks(s)  /* skip over blanks */
char *s;
{
  while (BLANK(*s))  s++;
  return(s);
}

char *afteralnum(s1,s2)  /* s1=conc(s2,result), *result is not alphanumeric */
char *s1, *s2;
{
  while (ALPHANUM(*s1))  *s2++ = *s1++;
  *s2 = 0;
  return(skipblanks(s1));
}

/** I/O  and OS-dependent stuff */

void getline(f,s)    /* strips blanks at the end */
FILE *f;  char *s;
{ int c;
  char *i = s;

  if ((c = getc(f))!=EOF && c!='\n') {
    do *s++ = c;
    while ((c = getc(f))!=EOF && c!='\n');
    s--;
    while (BLANK(*s))
      if (s == i) {
	*s = 0;
	return;
      } else  s--;
    s++;
  }
  *s = 0;
}

void skiptoeof(f)   /* skip to eof */
FILE *f;
{
  while (! feof(f))  getc(f);
}

void getanswer(m,n)  /* get answer in string */
char *m, *n;
{
  printf("%s: ",m);  getline(stdin,n);
}

#ifdef UNIX              /* UNIX-only */
void setprogversion()    /* remember which version of program is in memory */
{ struct stat buff;

  if (stat(ProgFName,&buff)) {
    printf("Warning: cannot access modification date of file %s\n",ProgFName);
    ProgVersion = 0;
  } else  ProgVersion = buff.st_mtime;
}

void checkprogversion()    /* check version of program against memory */
{ struct stat buff;

  if (ProgVersion && !stat(ProgFName,&buff) && ProgVersion!=buff.st_mtime)
    printf("Warning: file may be different from program in memory!\n");
}
#else
/* sorry, no file-time interfaces on non-UNIX OS */
#define setprogversion() 
#define checkprogversion() 
#endif

int noprogfile()   /* open program file for input */
{
  if ((ProgF=fopen(ProgFName,"r")) == NULL || feof(ProgF)) {
    printf("Cannot open %s for input...\n",ProgFName);
    return(1);
  }
  checkprogversion();
  return(0);
}

void seteditor()     /* get info on default editor from environment */
{ char *s;

#ifdef UNIX
  if ((s=getenv("EDITOR")) != NULL)  strncpy(EditorName,s,20);
  else  strncpy(EditorName,DEFAULT_EDITOR,20);
#else
  strncpy(EditorName, DEFAULT_EDITOR);
#endif
}

void edit()         /* invoke editor */
{ char *s = EditorName;

  while (*s++);
  *--s = ' ';
  strncpy(s+1,ProgFName,60);
  system(EditorName);
  *s = 0;
#ifdef UNIX
  system("reset");  system("clear");
#endif
}

/** Error handling */

int error(m,s)
char *m, *s;
{
  printf("Error: %s in\n\tLine %d:%s\n",m,TxtInx,s);
  return(1);
}

/** Clearing memory */

void clear()
{
#ifdef UNIX
  ProgVersion=0;
#endif
  CInx = LblInx = DInx = TxtInx = Debug = 0;
  Code[0].caddri = Origin = ORIGIN;
}

/** Label handling */

int lkuplabel(s)   /* lookup in table */
char *s;
{ int l;

  for (l=LblInx; l; l--)
    if (!strcmp(LblDef[l].lname,s))  return(l);
  return(0);
}

int newlabel(s,a,t)   /* add label, init to a, t */
char *s;  ADDR a;  LABTYPE t;
{
  strncpy(LblDef[++LblInx].lname,s,MAXLBLLGTH);
  LblDef[LblInx].lcinx = a;  LblDef[LblInx].ltype = t;
  return(LblInx);
}

int lkdeflabel(s,a,t)  /* lookup and if new define label */
char *s;  ADDR a;  LABTYPE t;
{ int l;

  if ((l = lkuplabel(s)))  return(l);
  return(newlabel(s,a,t));
}

int wronglabel(s,sp)  /* check label definition */
char *s, **sp;
{ char buff[120];  int l;

  if (! ALPHA(*s))  return(1);
  *sp = afteralnum(s,buff);
  if ((l=lkuplabel(buff)))
    if (LblDef[l].lcinx==UNDEF && LblDef[l].ltype==PROGLAB)
      LblDef[l].lcinx = CInx;
    else  return(1);
  else  newlabel(buff,CInx,PROGLAB);
  return(0);
}

int labelserr()   /* check all labels defined */
{ int l, err=0;

  for (l=LblInx; l; l--)
    if (LblDef[l].lcinx==UNDEF && LblDef[l].ltype==PROGLAB) {
      printf("Label %s undefined\n",LblDef[l].lname);
      err++;
    }
  return(err);
}

int makelbl(v)    /* make label for value v */
ADDR v;
{ int l;

  for (l=LblInx; l; l--)
    if (LblDef[l].lcinx==v && LblDef[l].ltype==CONSTLAB)  return(l);
  return(newlabel("",v,CONSTLAB));
}

/** Mnemonic handling */

MNM lkupmnm(s)  /* lookup mnemonic */
char *s;
{ MNM m;

  for (m=0; m<BADMNM; m++)
    if (!strcmp(Instr[m].mnem,s))  return(m);
  return(BADMNM);
}

MNM mnemonic(s,sp)  /* check mnemonic */
char *s, **sp;
{ char buff[120];

  if (! ALPHA(*s))  return(BADMNM);
  *sp = afteralnum(s,buff);
  return(lkupmnm(buff));
}

/** Attribute handling */

IATTR compattr(a,m)   /* check compatibility between mnemonic and attribute */
IATTR a;  MNM m;
{
  if (Instr[m].attr & a)  return(a);
  return(BADATTR);
}

IATTR attribute(s,m,sp)   /* parse attribute for mnemonic m */
char *s, **sp;  MNM m;
{ 
  if (*s != '.') {
    *sp = s;
    switch (m) {   /* set defaults, or W if not used */
    case EXG: case LEA: case MOVEQ: case PEA:
      return(L);
    case SCC: case SCS: case SEQ: case SF: case SGE: case SGT: case SHI:
    case SLE: case  SLS: case SLT: case SMI: case SNE: case SPL: case ST:
    case SVC: case SVS: case TAS:
      return(B);
    default:       /* use W for jumps and those where no attribute is used */
      return(W);
    }
  }
  s++;
  *sp = skipblanks(s+1);
  switch (*s) {
  case 'B':  return(compattr(B,m));
  case 'W':  return(compattr(W,m));
  case 'L':  return(compattr(L,m));
  default:  return(BADATTR);
  }
}

/** Arguments handling */

int badreg(s,pv,sp)     /* parse register name */
char *s, **sp;  ADDR *pv;
{ RGT r;  char buff[20];

  *sp = afteralnum(s,buff);
  for (r=PC; r<BADREG; r++)
    if (!strcmp(buff,RegName[r])) {
      *pv = (r==SP ? A7 : r);
      return(0);
    }
  return(1);
}

int badstr(s,p,c,sp)  /* string of up to 4 chars, delimiter c, to int */
char *s, c, **sp;  ADDR *p;
{ int max = 4;

  *p = 0;
  while (max-- && *s) {
    if (*s==c && *++s!=c) {
      *sp = s;
      return(0);
    }
    *p = (*p<<8) + *s++;
  }
  if (!*s || *s++ != c)  return(1);
  *sp = s;
  return(0);
}

int convdig(c)    /* convert digit to decimal */
char c;
{
  if (DIGIT(c))  return(c-'0');
  /* Eric KEDJI: I added the ability to use lower case
   * letters in hex numbers, so this instruction needs
   * a tweek.
   */
  //return(c-'A'+10);
  return(toupper(c) - 'A' + 10);
}

int digitb(c,b)    /* check if good digit for base */
char c;  int b;
{
  if (b != 16)  return('0'<=c && c<'0'+b);
  
  /*
   * Eric KEDJI: Changed the following line to
   * enter hex numbers with lower case letters
   */
  //return(DIGIT(c) || ('A'<=c && c<='F'));
  return(DIGIT(c) || ('A'<=c && c<='F') || ('a' <= c && c <= 'f'));
}


int badnum(s,p,sp)  /* parse non-negative number */
char *s, **sp;  ADDR *p;
{
    
    int b = 10;

  if (! DIGIT(*s))
    switch (*s++) {
    case '@':  b = 8;  break;
    case '%':  b = 2;  break;
    case '$':  b = 16;  break;
    case '\'':  return(badstr(s,p,QUOTE,sp));
    case '"':  return(badstr(s,p,'"',sp));
    default:  return(1);
    }
  if (! digitb(*s,b))  return(1);
  *p = convdig(*s++);
  while (digitb(*s,b))  *p = *p*b+convdig(*s++);
  *sp = s;
  return(0);
}

int badint(s,pv,sp)       /* parse integer */
char *s, **sp;  ADDR *pv;
{ int  k = 0;

  if (*s == '-') {
    s++;  k = 1;
    if (! DIGIT(*s))  return(1);
  }
  if (badnum(s,pv,sp))  return(1);
  if (k)  *pv = -*pv;
  return(0);
}

int badeol(s)       /* anything other than comments at end of line? */
char *s;
{
  while (*s && BLANK(*s))  s++;
  return(*s && *s!=CMTCHR);
}

int badattrdisplreg(s,pa,pv,sp)   /* parse after label or number */
char *s, **sp;  IARG *pa;  ADDR *pv;
{
  *pv = L;  *sp = s;
  if (*s == '.') {
    if (*++s == 'W')  *pv = W;
    else if (*s != 'L')  return(1);
    *sp = s+1;
    return(0);
  }
  if (*s != '(')  return(0);
  if (badreg(s+1,pv,&s) || *pv>SP)  return(1);
  if (*s == ')') {
    *pa = (*pv==PC ? DISPLPC : DISPL);  *sp = s+1;
    return(0);
  }
  if (*s != ',')  return(1);
  *pa = (*pv==PC ? DISPLPCREG : DISPLREG);
  if (badreg(s+1,++pv,&s) || *pv<A0 || *pv>D7)  return(1);
  if (*s == '.') {
    if (*++s == 'L')  *pv = -*pv;
    else if (*s != 'W')  return(1);
    ++s;
  }
  if (*s != ')')  return(1);
  *sp = s+1;
  return(0);
}

ADDR maskreg(r)   /* register to its bit-mask representation */
RGT r;
{
  if (r > A7)  return((1<<(r-D0+8)));
  return((1<<(r-A0)));
}

int badrange(r1,r2)   /* check register range */
RGT r1, r2;
{
  return(r1>r2 || (r1<D0 && r2>SP) || (r1>SP && r2>D7));
}

int badreglist(s,r,pv,sp)  /* parse rest of register list */
char *s, **sp;  RGT r;  ADDR *pv;
{ char  c = *s;  ADDR n;

  if (badreg(s+1,&n,&s))  return(1);
  if (c == '-') {
    if (badrange(r,n))  return(1);
    while (n > r)  *pv |= maskreg(n--);
    if (*s == '/')  return(badreglist(s,1000,pv,sp));
  } else {  /* c == '/' */
    if (n<A0 || n>D7)  return(1);
    *pv |= maskreg(n);
    if (*s=='/' || *s=='-')  return(badreglist(s,n,pv,sp));
  }
  *sp = s;
  return(0);
}

int badarg(s,n,sp)   /* parse argument number n+1 */
char *s, **sp;  int n;
{ char buff[120];
  ADDR *pv = &(Code[CInx].valargi[n][0]), r;
  IARG *pa = &(Code[CInx].targi[n]);

  if (ALPHA(*s)) {
    if (! badreg(s,pv,sp)) {
      s = *sp;
      if (*s=='/' || *s=='-') {
	if (*pv == CCR)  return(1);
	*pa = REGL;  *pv = maskreg((r = *pv));
	return(badreglist(s,r,pv,sp));
      }
      if (*pv == CCR)  *pa = CR;
      else if (Code[CInx].attri==B && *pv>=A0 && *pv<=SP)  return(1);
      else  *pa = REG;
      return(0);
    }
    *sp = s = afteralnum(s,buff);
    *pa = LBL;
    *pv = lkdeflabel(buff,UNDEF,PROGLAB);
    return(badattrdisplreg(s,pa,++pv,sp));
  }
  if (NUMIN(*s)) {
    if (badnum(s,pv,&s))  return(1);
    *pa = LBL;  *pv = makelbl(*pv);
    return(badattrdisplreg(s,pa,++pv,sp));
  }
  switch (*s++) {
  case '-':
    if (DIGIT(*s)) {
      if (badnum(s,pv,&s))  return(1);
      *pa = LBL;  *pv = makelbl(-*pv);
      if (badattrdisplreg(s,pa,++pv,sp) || *pa==LBL)  return(1);
      return(0);
    }
    if (*s != '(')  return(1);
    if (badreg(s+1,pv,&s) || *pv<A0 || *pv>SP || *s!=')')  return(1);
    *pa = PrD;  *sp = s+1;
    return(0);
  case '(':
    if (badreg(s,pv,&s) || *pv<A0 || *pv>SP || *s++ != ')')  return(1);
    if (*s == '+') {
      *pa = PtI;  *sp = s+1;
    } else {
      *pa = IND;  *sp = s;
    }
    return(0);
  case '#':
    *pa = I;
    if (ALPHA(*s)) {
      if (! badreg(s,pv,sp))  return(1);
      *sp = s = afteralnum(s,buff);
      *pv = lkdeflabel(buff,UNDEF,PROGLAB);  *(pv+1) = LBL;
    } else if (badint(s,pv,sp))  return(1);
    else  *(pv+1) = I;
    return(0);
  }
}

int notargcompat(m,a,g,pa,pv)  /* check arg type compatibility */
MNM m;  IATTR a;  IARG g, *pa;  ADDR *pv;
{ ADDR rigthvalue();

  switch (m) {  /* deal with exceptions first... */
  case ANDI: case EORI: case ORI:
    if (g==CR && *pa==CR)  return(a==B);
    break;
  case ASL: case ASR: case LSL: case LSR: case ROL: case ROR:
  case ROXL: case ROXR:
    if (g==ALT && a!=W)  return(1);
    if (g == QI3)
      if (*pa != I)  return(1);
      else  return(0);   /* cannot always check length */
    break;
  case MOVE:
    if (g==CR && *pa==CR  &&  a!=W)  return(1);
    break;
  }
  if (g == *pa)  return(0);
  switch (g) {
  case NONE: case I: case PrD: case PtI: case LBL: case DISPL: case DISPLREG:
  case DISPLPC: case DISPLPCREG: case REG: case CR:
    return(1);
  case A:  return(*pa!=REG || *pv<A0 || *pv>SP);
  case D:  return(*pa!=REG || *pv<D0 || *pv>D7);
  case REGL:
    if (*pa != REG)  return(1);
    *pa = REGL;  *pv = maskreg(*pv);
    return(0);
  case QI3: case QI8:
    return(*pa != I);    /* cannot always check length */
  case EA:  return(0);
  case EAAIDL:
    if (*pa == I)  return(1);
  case EAADL:
    if (*pa==REG && *pv>=D0 && *pv<=D7)  return(Code[CInx].attri != L);
  case EAA:
    return(*pa==REG && *pv>=A0 && *pv<=SP);
  case ALTDA:
    if (*pa==REG && *pv>=A0 && *pv<=SP)  return(0);
  case ALTD:
    if (*pa==REG && *pv>=D0 && *pv<=D7)  return(0);
  case ALT:
    switch (*pa) {
    case IND: case PtI: case PrD: case LBL:
      return(0);
    case DISPL: case DISPLREG:
      return(*(pv+1)<A0 || *(pv+1)>SP);
    default:  return(1);
    }
  case ALTDL:
    if (*pa==REG && *pv>=D0 && *pv<=D7)  return(Code[CInx].attri != L);
    switch (*pa) {
    case IND: case PtI: case PrD: case LBL:
      return(0);
    case DISPL: case DISPLREG:
      return(*(pv+1)<A0 || *(pv+1)>SP);
    default:  return(1);
    }
  case ALTPtI:
    switch (*pa) {
    case IND: case PrD: case LBL:
      return(0);
    case DISPL: case DISPLREG:
      return(*(pv+1)<A0 || *(pv+1)>SP);
    default:  return(1);
    }
  case PADDRPtI:
    if (*pa == PtI)  return(0);
  case PADDR:
    switch (*pa) {
    case IND: case PtI: case PrD: case LBL: case DISPLPC: case DISPLPCREG:
      return(0);
    case DISPL: case DISPLREG:
      return(*(pv+1)<A0 || *(pv+1)>SP);
    default:  return(1);
    }
  default:  return(1);  /* unlikely to happen... */
  }
}

ADDR arglgth(m,ta,a)    /* compute length in bytes of argument coding */
MNM m;  IARG ta;  IATTR a;
{
  switch (m) {
  case ADDQ: case MOVEQ: case SUBQ:
    return(0);
  case ASL: case ASR: case LSL: case LSR: case ROL: case ROR:
  case ROXR: case ROXL:
    if (ta == LBL)  return(4);
    if (ta==DISPL || ta==DISPLREG)  return(2);
    return(0);
  case BRA: case BSR:
  case BCC: case BCS: case BEQ: case BGE: case BGT: case BHI:
  case BLE: case BLS: case BLT: case BMI: case BNE: case BPL:
  case BVC: case BVS:
  case DBCC: case DBCS: case DBEQ: case DBF: case DBGE: case DBGT: case DBHI:
  case DBLE: case DBLS: case DBLT: case DBMI: case DBNE: case DBPL:
  case DBT: case DBVC: case DBVS:
    return(2);
  case BCHG: case BCLR: case BSET: case BTST:
  case LINK:
    if (ta == I)  return(2);
    return(0);
  }
  switch (ta) {
  case REG: case PtI: case PrD: case IND:
    return(0);
  case DISPL: case DISPLPC: case DISPLREG: case DISPLPCREG: case REGL:
    return(2);
  case LBL:
    return(4);
  case I:
    if (a == L)  return(4);
    return(2);
  }
}

int badsinglearg(m,a,ar)  /* check compatibility of single argument */
MNM m;  IATTR a;  int ar;
{ IARG  *pa = &(Code[CInx].targi[0]);
  ARGPAIR  *app = &(Instr[m].argpatt[0]);
  ADDR  *vp = &(Code[CInx].valargi[0][0]);

  if (ar == 1)
    while (app->lhs != NONE && notargcompat(m,a,app->lhs,pa,vp))  app++;
  else { /* ar == 2 */
    *(pa+1) = NONE;
    while (app->lhs != NONE &&
	   (app->rhs != NONE || notargcompat(m,a,app->lhs,pa,vp)))  app++;
  }
  InstrLgth += arglgth(m,*pa,Code[CInx].attri);
  return(app->lhs == NONE);
}

int badargpair(m,a)    /* check compatibility of argument pair */
MNM m;  IATTR a;
{ IARG  *pa = &(Code[CInx].targi[0]);
  ARGPAIR  *app = &(Instr[m].argpatt[0]);
  ADDR *vp1 = &(Code[CInx].valargi[0][0]), *vp2 = &(Code[CInx].valargi[1][0]);

  while (app->lhs!=NONE && (notargcompat(m,a,app->lhs,pa,vp1)
			    || notargcompat(m,a,app->rhs,pa+1,vp2)))
    app++;
  if (app->lhs == NONE)  return(1);
  switch (m) {    /* some more exceptions here... */
  case BCHG: case BCLR: case BSET: case BTST:
    switch (Code[CInx].attri) {
    case B:
      if (*(pa+1) == REG)  return(1);
      break;
    case W:  /* means no attribute was given */
      Code[CInx].attri = (*(pa+1)==REG ? L : B);
      break;
    case L:
      if (*(pa+1) != REG)  return(1);
    }
  }
  InstrLgth += arglgth(m,*pa,Code[CInx].attri)+arglgth(m,*(pa+1),W);
                         /* attribute only for Imm, not needed for 2nd arg */
  return(0);
}

int badargs(s,m,a)  /* parse arguments for m.a */
char *s;  MNM m;  IATTR a;
{ int badassdir();

  if (! Instr[m].ary)  return(badeol(s));
  if (badarg(s,0,&s))  return(1);
  if (Instr[m].ary==1 || !(*s) || BLANK(*s) || *s==CMTCHR)
    return(badeol(s) || badsinglearg(m,a,Instr[m].ary));
  if (*s++ != ',')  return(1);
  if (badarg(s,1,&s) || badeol(s))  return(1);
  return(badargpair(m,a));
}

/** Assembler directives, not to be treated as normal instructions */

int redeflbl(t,v)   /* redefine label(s) of current position, count them */
LABTYPE t;  ADDR v;
{ int l, k=0;

  for (l=1; l<=LblInx; l++)
    if (LblDef[l].lcinx==CInx && LblDef[l].ltype==PROGLAB) {
      LblDef[l].ltype = t;  LblDef[l].lcinx = v;  k++;
    }
  return(k);
}

int badlength(v,a)   /* check compatibility of value with attribute */
ADDR v;  IATTR a;
{
  return(a!=L && (v=(v>>(32-BShift[a]))) && v != -1);
}

int badclist(s,l,a,pv,sp)  /* parse constant list */
char *s, **sp;  IATTR a;  ADDR *l, *pv;
{ char c;

  *pv = 0;
  if (a==B && ((c = *s)==QUOTE || c=='"')) {
    while (*++s && *pv<MAXCLIST) {
      if (*s==c && *++s!=c) {
	*sp = s;
	return(0);
      }
      *l++ = *s;  (*pv)++;
    }
    return(1);
  }
  do {
    if (badint(s,l,&s) || ++(*pv)>MAXCLIST || badlength(*l,a))  return(1);
    l++;
  } while (*s++ == ',');
  *sp = s-1;
  return(0);
}

int align(i,a)        /* align index into data segment for attribute */
int i;  IATTR a;
{ BYTE *p = &(Data[i]);

  if (a==W && (((ADDR) p)&1))  return(i+1);
  else if (a==L && (((ADDR) p)&3))  return(i+4-(((ADDR) p)&3));
  return(i);
}

int badassdir(s,m,a)  /* parse arguments and obey assembler directive */
char *s;  MNM m;  IATTR a;
{ ADDR l, v, clist[MAXCLIST];

  switch (m) {
  case END:
    if (badeol(s))  return(1);
    skiptoeof(ProgF);
    if (redeflbl(PROGLAB,UNDEF)) {
      printf("??? Cannot accept label(s) at non-executable directive\n");
      return(1);
    }
    return(0);
  case EXIT:
    if (badeol(s))  return(1);
    Txtln[TxtInx] = CInx++;
    Code[CInx].caddri = Code[CInx-1].caddri + 2;
    return(0);
  case EQU:
    if (badint(s,&v,&s) || badeol(s))  return(1);
    if (! redeflbl(CONSTLAB,v)) {
      printf("??? Cannot obey EQU without a label\n");
      return(1);
    }
    return(0);
  case DS:
    if (badnum(s,&v,&s) || badeol(s))  return(1);
    DInx = align(DInx,a);
    redeflbl(DATALAB,DInx);
    Txtln[TxtInx] = -DInx-1;
    DInx += length(v,a);
    return(0);
  case DC:
    if (badclist(s,clist,a,&v,&s) || (l=length(v,a))>MAXCLIST || badeol(s))
      return(1);
    DInx = align(DInx,a);
    redeflbl(DATALAB,DInx);
    blccpy(clist,v,a,&(Data[DInx]));
    Txtln[TxtInx] = -DInx-1;
    DInx += length(v,a);
    return(0);
  }
}

/** Assembling */

int asslineerr(s)  /* assemble single line */
char *s;
{ char c, *cp = s;  MNM m;  IATTR a;

  InstrLgth = 0;
  if ((c = *s)==CMTCHR || c=='*')  return(0);
  if (c && !BLANK(c) && wronglabel(s,&s))
    return(error("wrong label",cp));
  s = skipblanks(s);
  if (!(c = *s) || c==CMTCHR)  return(0);
  if ((m=mnemonic(s,&s)) == BADMNM) return(error("bad mnemonic",cp));
  if (m > UNIMPL)  return(error("unimplemented instruction",cp));
  if ((a = attribute(s,m,&s)) == BADATTR)
    return(error("bad attribute",cp));
  InstrLgth = 2;
  Code[CInx].mnmi = m;  Code[CInx].attri = a;
  s = skipblanks(s);
  if (m>ASSDIR && Instr[m].ary<1)
    if (badassdir(s,m,a))  return(error("bad argument(s)",cp));
    else  return(0);
  if (!(c = *s) || c==CMTCHR) {
    if (Instr[m].ary)  return(error("argument(s) expected",cp));
  } else if (badargs(s,m,a))  return(error("bad argument(s)",cp));
  Txtln[TxtInx] = CInx++;
  Code[CInx].caddri = Code[CInx-1].caddri + InstrLgth;
  return(0);
}

void ass()        /* assemble entire program */
{ char buff[120];  int errs=0;

  if (! (*ProgFName))  getanswer("Filename",ProgFName);
  if (noprogfile())  return;
  do {
    getline(ProgF,buff);  Txtln[++TxtInx] = NOINFO;
    errs += asslineerr(buff);
  } while (errs<MAXERR && !feof(ProgF));
  if (! errs)  errs = labelserr();
  if (errs) {
    if (errs >= MAXERR)  printf("Too many errors; aborting...\n");
    clear();
  } else {
    /* just make sure there is an "EXIT" directive at the end */
    Txtln[++TxtInx] = NOINFO;
    asslineerr("\t EXIT");
  }
  fclose(ProgF);
}

/** Listing */

void printline(ln,s)    /* print text line */
int ln;  char *s;
{
  if (Txtln[ln] == NOINFO)
    printf("%3d        \t%s\n",ln,s);
  else if (Txtln[ln] < 0)
    printf("%3d %06Xv\t%s\n",ln,DATAORIGIN-1-Txtln[ln],s);
  else  printf("%3d %06Xp\t%s\n",ln,Code[Txtln[ln]].caddri,s);
}

void list()             /* list complete program */
{ int l;  char buff[120];

  if (noprogfile())  return;
  printf("\n>\t\t\tListing of %s\n\n",ProgFName);
  for (l=1; l<=TxtInx; l++) {
    getline(ProgF,buff);  printline(l,buff);
    if (l%TTY_ROWS == 0) {
      getanswer("(return to continue or any key to quit)", buff);
      if (buff[0]) break;
    }
  }
  if (l>TxtInx)
    printf("\n>\t\t\tend of listing\n\n");
  else
    printf("\n\n");
  fclose(ProgF);
}

/** Execution */

/* Data conversion */

ADDR convert(v,a)    /* convert long according to attribute */
ADDR v;  IATTR a;
{
  if (a!=L && (v&MaskH[a]))  return((v<<BShift[a])>>BShift[a]);
  return(v&MaskA[a]);
}

/* Accessing registers */

ADDR setcond(c,f)    /* set condition bit in CCR to f */
CONDBIT c;  int f;
{
  if (f)  f = MaskCB[c];
  Reg[CCR] = (Reg[CCR] & ~MaskCB[c]) + f;
  return(f);
}

void setreg(r,a,v)    /* set register, attribute a, CCR not affected */
int r;  IATTR a;  ADDR v;
{
  Reg[r] = (Reg[r] & ~MaskA[a]) + (v & MaskA[a]);
}

void setregc(r,a,v)    /* set register, attribute a, set CCR */
int r;  IATTR a;  ADDR v;
{
  Reg[r] = (Reg[r] & ~MaskA[a]) + (v & MaskA[a]);
  if (r != CCR) {
    setcond(C,0);  setcond(V,0);
    setcond(Z,ISZERO(v,a));  setcond(N,RMBITSET(v,a));
  }
}

ADDR getreg(r,a)    /* get register contents, attribute a */
RGT r;  IATTR a;
{ ADDR  v = Reg[r];

  return(convert(v,a));
}

ADDR decrreg(r,a)   /* decrement register contents, attribute a */
RGT r;  IATTR a;
{ ADDR  v = getreg(r,a)-1;

  setreg(r,a,v);
  return(v);
}

/* Memory access */

BYTE *locateaddr(a)  /* get pointer to cell with memory address a */
ADDR a;
{
  if ((a>DATATOP || (a-=DATAORIGIN)<0))  return(NULL);
  return(&(Data[a]));
}

void setmem(ad,at,v)    /* set memory at address ad, attribute at */
ADDR ad;  ULONG v;  IATTR at;
{ BYTE *p;  WORD *q;

  if (at!=B && (ad&1))  ExErr = ADDRERR;
  else if ((p=locateaddr(ad)) == NULL)  ExErr = SEGFAULT;
  else {
#ifdef LITTLEENDIAN
    v = chgrepr(v,at);
#endif
    switch (at) {
    case B:  *p = v;  break;
    case W:
      * (WORD *)p = v;
      break;
    case L:
      if ((int)p % 4) {     /* avoiding alignment problems... */
	q = (WORD *) p;
	*q++ = v & MaskA[W];  *q = v >> BShift[W];
      } else  * (ADDR *)p = v;
    }
  }
}

ADDR memcont(p,at)      /* get memory contents pointed to by p */
BYTE *p;  IATTR at;
{ ADDR v;  WORD *q;

  switch (at) {
  case B:  return(convert(*p,B));
  case W:
#ifdef LITTLEENDIAN
    v = chgrepr(* (ADDR *)p,W);
#else /* BIGENDIAN */
    v = *(WORD *)p;
#endif
    return(convert(v,W));
  case L:
#ifdef LITTLEENDIAN
    return(chgrepr(* (ADDR *)p,L));
#else /* BIGENDIAN */
    if ((int)p % 4) {     /* avoiding alignment problems... */
      q = (WORD *) p;
      return((*(q+1)<<BShift[W])+*q);
    } else  return(* (ADDR *)p);
#endif
  }
}

ADDR getmem(ad,at)      /* get memory contents address ad, attribute at */
ADDR ad;  IATTR at;
{ BYTE *p;

  if (at!=B && (ad&1))  ExErr = ADDRERR;
  else if ((p=locateaddr(ad)) == NULL)  ExErr = SEGFAULT;
  else  return(memcont(p,at));
  return(UNDEF);
}

int goodlv(lv)       /* check pointer against data segment bounds */
BYTE *lv;
{
  if (ExErr!=NOERR && ExErr!=PROGEXIT)  return(0);
  if (lv>(BYTE *)&(Data[DATATOP-DATAORIGIN+1]) || lv<(BYTE *)&(Data[0])) {
    ExErr = SEGFAULT;
    return(0);
  }
  return(1);
}

/* Using labels */

ADDR lblval(l)   /* get value associated with label */
int l;
{
  switch (LblDef[l].ltype) {
  case PROGLAB:  return(Code[LblDef[l].lcinx].caddri);
  case CONSTLAB:  return(LblDef[l].lcinx);
  case DATALAB:  return(LblDef[l].lcinx+DATAORIGIN);
  }
}

/* Control addresses */

ADDR getctrladdr(t,p)    /* compute control address */
IARG t;  ADDR *p;
{
  switch (t) {
  case LBL:  return(lblval(*p));
  case IND:  return(Reg[*p]);
  case DISPL:  return(lblval(*p)+Reg[*(p+1)]);
  case DISPLREG:  return(lblval(*p)+Reg[*(p+1)]+
		    getreg(myabs(*(p+2)),(*(p+2)<0 ? L : W)));
  }
}

/* Effective addresses */

BYTE *leftvalue(t,p,at)   /* compute pointer to location of argument */
IARG t;  ADDR *p;  IATTR at;
{ BYTE *x;

  switch (t) {
  case REG:  case CR:
    switch (at) {
#ifdef LITTLEENDIAN
    case B:
    case W:
      return(((BYTE *) &(Reg[*p])));
#else /* BIGENDIAN */
    case B:  return(((BYTE *) &(Reg[*p]))+3);
    case W:  return(((BYTE *) &(Reg[*p]))+2);
#endif
    case L:  return((BYTE *) &(Reg[*p]));
    }
  case IND:  return(locateaddr(Reg[*p]));
  case PtI:
    x = locateaddr(Reg[*p]);  Reg[*p] += length(1,at);
    return(x);    
  case PrD:
    Reg[*p] -= length(1,at);
    return(locateaddr(Reg[*p]));
  case LBL:  return(locateaddr(lblval(*p)));
  case DISPL: case DISPLPC:
    return(locateaddr(lblval(*p)+Reg[*(p+1)]));
  case DISPLREG: case DISPLPCREG:
    return(locateaddr(lblval(*p)+Reg[*(p+1)]+
		      getreg(myabs(*(p+2)),(*(p+2)<0 ? L : W))));
  default:
    return(NULL);
  }
}

ADDR rightvalue(t,p,at,plv)   /* compute value of argument, attribute at */
IARG t;  ADDR *p;  IATTR at;  BYTE **plv;
{
  if (t == I)
    if (*(p+1) == I)  return(convert(*p,at));
    else  return(convert(lblval(*p),at));    
  if ((*plv=leftvalue(t,p,at)) == NULL)  ExErr = SEGFAULT;
  else if ((((ADDR) *plv)&1) && at!=B)  ExErr = ADDRERR;
  else if (t==REG || t==CR)  return(getreg(*p,at));
  return(memcont(*plv,at));
  return(UNDEF);
}

/* Alterable addresses */

void setatlv(lv,t,at,v)    /* set memory at address to value */
BYTE *lv;  IARG t;  IATTR at;  ADDR v;
{ WORD *q;

  if (ExErr != NOERR)  return;
  switch (at) {
  case B:  *lv = v;  break;
  case W:
#ifdef LITTLEENDIAN
    if (t!=REG && t!=CCR)  v = chgrepr(v,W);
#endif
    * (WORD *)lv = v;
    break;
  case L:
#ifdef LITTLEENDIAN
    if (t!=REG && t!=CCR)  v = chgrepr(v,L);
#endif
    if ((int)lv % 4) {     /* avoiding alignment problems... */
      q = (WORD *) lv;
      *q++ = v & MaskA[W];  *q = v >> BShift[W];
    } else  * (ADDR *)lv = v;
  }
}

void setatlvc(lv,t,at,v)    /* set memory at address to value, set CCR */
BYTE *lv;  IARG t;  IATTR at;  ADDR v;
{
  if (ExErr != NOERR)  return;
  setatlv(lv,t,at,v);
  setcond(C,0);  setcond(V,0);
  setcond(Z,ISZERO(v,at));  setcond(N,RMBITSET(v,at));
}

void setalt(t,p,at,v)    /* set alterable memory to value */
IARG t;  ADDR *p, v;  IATTR at;
{ BYTE *lv;

  if (t==REG)  setreg(*p,at,v);
  else if ((lv=leftvalue(t,p,at)) == NULL)  ExErr = SEGFAULT;
  else if ((((ADDR) lv)&1) && at!=B)  ExErr = ADDRERR;
  else setatlv(lv,t,at,v);
}

void setaltc(t,p,at,v)    /* set alterable memory to value, set CCR */
IARG t;  ADDR *p, v;  IATTR at;
{
  setalt(t,p,at,v);
  setcond(C,0);  setcond(V,0);
  setcond(Z,ISZERO(v,at));  setcond(N,RMBITSET(v,at));
}

/* Program addresses */

int locatepaddr(a)  /* get index in Code for program address a */
ADDR a;
{ int  mn=0, mx=CInx, i;  ADDR x;

  if (!(a&1) && a>=Code[0].caddri && a<=Code[CInx-1].caddri)
    while (mn <= mx)
      if ((x = Code[(i=(mx+mn)/2)].caddri) == a)  return(i);
      else if (x < a)  mn = i+1;
      else  mx = i-1;
  return(-1);
}

ADDR getpaddr(t,p)  /* get PADDR from argument */
IARG t;  int *p;
{
  switch (t) {
  case IND:  return(Reg[*p]);
  case DISPL: case DISPLPC:
    return(lblval(*p)+Reg[*(p+1)]);
  case DISPLREG: case DISPLPCREG:
    return(lblval(*p)+Reg[*(p+1)]+getreg(myabs(*(p+2)),(*(p+2)<0 ? L : W)));
  case LBL:  return(lblval(*p));
  }
}

/* Jumping */

void jumpmaddr(a)  /* jump to memory address */
ADDR a;
{
  if ((ExCInx = locatepaddr(a)) < 0) {
    ExErr = ADDRERR;  ExCInx = 0;  Reg[PC] = 0;
  } else  Reg[PC] = a;
}

void jumpaddr(t,p)  /* jump using PADDR */
IARG t;  int *p;
{
  jumpmaddr(getpaddr(t,p));
}
  
/* Dealing with conditions */

int cond(c)         /* evaluate a condition */
COND c;
{
  switch (c) {
  case CC:  return(!(Reg[CCR]&MaskCB[C]));
  case CS:  return((Reg[CCR]&MaskCB[C])&&1);
  case EQ:  return((Reg[CCR]&MaskCB[Z])&&1);
  case GE:  return((cond(MI)&&cond(VS))||(cond(PL)&&cond(VC)));
  case GT:  return(cond(NE)&&cond(GE));
  case HI:  return(cond(CC)&&cond(NE));
  case LE:  return(cond(EQ)||cond(LT));
  case LS:  return(cond(CS)||cond(EQ));
  case LT:  return((cond(MI)&&cond(VC))||(cond(PL)&&cond(VS)));
  case MI:  return((Reg[CCR]&MaskCB[N])&&1);
  case NE:  return(!(Reg[CCR]&MaskCB[Z]));
  case PL:  return(!(Reg[CCR]&MaskCB[N]));
  case VC:  return(!(Reg[CCR]&MaskCB[V]));
  case VS:  return((Reg[CCR]&MaskCB[V])&&1);
  case XS:  return((Reg[CCR]&MaskCB[X])&&1);    /* not canonical ! */
  }
}

int divoflow(x)      /* test division overflow */
ADDR x;
{ WORD  u = (x>>16)&MaskA[W];

  if (! u)  return(0);          /* no, if upper word is all 0s */
  if (u != -1)  return(1);      /* yes, if neither all 0s nor all 1s */
  return(!(x&MaskH[W]));        /* yes, if all 1s and lower is positive */
}

/* Arithmetics */

ADDR subtrc(d,s,at)    /* subtract and set CCR */
ADDR d, s;  IATTR at;
{ ADDR r = d-s;  int sm, dm, rm;

  sm = s&MaskH[at];  dm = d&MaskH[at];  rm = RMBITSET(r,at);
  setcond(N,rm);  setcond(Z,ISZERO(r,at));
  setcond(V,(dm&&!(sm||rm))||(sm&&rm&&!dm));
  setcond(X,setcond(C,(sm&&rm)||(sm&&!dm)||(rm&&!dm)));
  return(r);
}

ADDR addc(d,s,at)    /* add and set CCR */
ADDR d, s;  IATTR at;
{ ADDR r = d+s;  int sm, dm, rm;

  sm = s&MaskH[at];  dm = d&MaskH[at];  rm = RMBITSET(r,at);
  setcond(N,rm);  setcond(Z,ISZERO(r,at));
  setcond(V,(sm&&dm&&!rm)||(rm&&!sm&&!dm));
  setcond(X,setcond(C,(sm&&dm)||(!rm&&(sm||dm))));
  return(r);
}

/* Bit operations */

ADDR bitchg(v,b)    /* change bit b on value v, set Z condition */
ADDR v;  int b;
{ ADDR  f = v&MaskB[b];

  setcond(Z,!f);
  return((f ? v&~MaskB[b] : v|MaskB[b]));
}

ADDR bitclr(v,b)    /* clear bit b on value v, set Z condition */
ADDR v;  int b;
{
  setcond(Z,!(v&MaskB[b]));
  return(v&~MaskB[b]);
}

ADDR bitset(v,b)    /* set bit b on value v, set Z condition */
ADDR v;  int b;
{
  setcond(Z,!(v&MaskB[b]));
  return(v|MaskB[b]);
}

/* Shifting */

ADDR shift(m,at,c,v)         /* shift v c bits, set CCR */
MNM m;  IATTR at;  int c;  ADDR v;
{ ADDR b;

  setcond(V,0);
  switch (m) {
  case ROXR:
    if (! c)  setcond(C,cond(XS));
    else
      do {
	b = setcond(C,v&1);
	v = (v>>1)&(cond(XS) ? MaskA[at] : ~MaskH[at]);
	setcond(X,b);
      } while (--c);
    break;
  case ROXL:
    if (! c)  setcond(C,cond(XS));
    else
      do {
	b = setcond(C,v&MaskH[at]);
	v = (v<<1)|cond(XS);  setcond(X,b);
      } while (--c);
    break;
  case ROR:
    if (! c)  setcond(C,0);
    else
      do {
	b = setcond(C,v&1);
	v = (v>>1)&(b ? MaskA[at] : ~MaskH[at]);
      } while (--c);
    break;
  case ROL:
    if (! c)  setcond(C,0);
    else
      do {
	b = setcond(C,v&MaskH[at]);
	v = (v<<1)|b;
      } while (--c);
    break;
  case LSR:
    if (! c)  setcond(C,0);
    else
      do {
	setcond(C,setcond(X,v&1));
	v = (v>>1)&~MaskH[at];
      } while (--c);
    break;
  case LSL:
    if (! c)  setcond(C,0);
    else
      do {
	setcond(C,setcond(X,RMBITSET(v,at)));
	v = v<<1;
      } while (--c);
    break;
  case ASR:
    if (! c)  setcond(C,0);
    else {
      b = v&MaskH[at];
      do {
	setcond(C,setcond(X,v&1));
	v = (v>>1)|b;
      } while (--c);
    }
    break;
  case ASL:
    if (! c)  setcond(C,0);
    else
      do {
	b = setcond(C,setcond(X,RMBITSET(v,at)));
	v = v<<1;
	setcond(V,cond(VS)||(b== !RMBITSET(v,at)));
      } while (--c);
    break;
  }
  v = convert(v,at);
  setcond(N,RMBITSET(v,at));  setcond(Z,ISZERO(v,at));
  return(v);
}

/* Register list */

int in(r,m)           /* check if register is in register list mask */
RGT r;  ADDR m;
{
  if (r > A7)  return(m&(1<<(r-D0+8)));
  return(m&(1<<(r-A0)));
}

/* Main control */

MNM fetch()     /* fetch mnemonic, check it and program address */
{ MNM m;

  if (Reg[PC] != Code[ExCInx].caddri) {
    ExErr = BUSERR;
    return(ASSDIR);
  }
  if ((m=Code[ExCInx].mnmi) > UNIMPL) {
    ExErr = UNIMPLERR;
    return(ASSDIR);
  }
  if (ExCInx+1 == CInx)  Reg[PC] = 0;
  else  Reg[PC] = Code[ExCInx+1].caddri;
  return(m);
}

EXECERR execinstr()   /* execute instruction at (PC), ExCInx */
{ ADDR x, y;  ULONG ux, uy;  BYTE *lv, *px;  MNM m;  int debugit();

  if (Debug && debugit(RUNNING))  return(ExErr);
  switch ((m=fetch())) {
  case MOVEM:
    if (TARG1 == REGL)
      if (TARG2 == PrD) {
	for (x=A7; x>=A0 && ExErr==NOERR; x--)
	  if (in(x,ARG1))  setalt(PrD,PARG2,SIZE,getreg(x,SIZE));
	for (x=D7; x>=D0 && ExErr==NOERR; x--)
	  if (in(x,ARG1))  setalt(PrD,PARG2,SIZE,getreg(x,SIZE));
      } else {
	lv = leftvalue(TARG2,PARG2,SIZE);  y = length(1,SIZE);
	for (x=D0; x<=D7 && goodlv(lv); x++, lv+=y)
	  if (in(x,ARG1))  setatlv(lv,TARG2,SIZE,getreg(x,SIZE));
	for (x=A0; x<=A7 && goodlv(lv); x++, lv+=y)
	  if (in(x,ARG1))  setatlv(lv,TARG2,SIZE,getreg(x,SIZE));
      }
    else  /* TARG2 == REGL */
      if (TARG1 == PtI) {
	for (x=D0; x<=D7 && ExErr==NOERR; x++)
	  if (in(x,ARG2))  setreg(x,SIZE,rightvalue(PtI,PARG1,SIZE,&px));
	for (x=A0; x<=A7 && ExErr==NOERR; x++)
	  if (in(x,ARG2))  setreg(x,SIZE,rightvalue(PtI,PARG1,SIZE,&px));
      } else {
	lv = leftvalue(TARG2,PARG2,SIZE);  y = length(1,SIZE);
	for (x=D0; x<=D7 && goodlv(lv); x++, lv+=y)
	  if (in(x,ARG2))  setreg(x,SIZE,memcont(lv,SIZE));
	for (x=A0; x<=A7 && goodlv(lv); x++, lv+=y)
	  if (in(x,ARG2))  setreg(x,SIZE,memcont(lv,SIZE));
      }
    break;
  case ASL: case ASR: case LSL: case LSR: case ROL: case ROR:
  case ROXR: case ROXL:
    if (TARG2 == NONE) {
      x = shift(m,W,1,rightvalue(TARG1,PARG1,W,&lv));
      setatlv(lv,TARG1,W,x);
    } else if (TARG1 == REG)
      setreg(ARG2,SIZE,shift(m,SIZE,Reg[ARG1]&0x3F,getreg(ARG2,SIZE)));
    else  /* TARG1 == I */
      setreg(ARG2,SIZE,shift(m,SIZE,rightvalue(TARG1,PARG1,B,NULL),
			     getreg(ARG2,SIZE)));
    break;
  case DIVU:
    if (!(ux=(rightvalue(TARG1,PARG1,L,&px)&MaskA[W])))  ExErr = DIVBY0;
    else if (ExErr == NOERR) {
      uy = ((ULONG) Reg[ARG2])/ux;
      if (! setcond(V,divoflow(uy))) {
	setreg(ARG2,L,((Reg[ARG2]%ux)<<16)+(uy&MaskA[W]));
	setcond(C,0);  setcond(Z,ISZERO(uy,W));  setcond(N,RMBITSET(uy,W));
      }
    }
    break;
  case DIVS:
    if (!(x=rightvalue(TARG1,PARG1,W,&px)))  ExErr = DIVBY0;
    else if (ExErr == NOERR) {
      y = Reg[ARG2]/x;
      if (! setcond(V,divoflow(y))) {
	setreg(ARG2,L,((Reg[ARG2]%x)<<16)+(y&MaskA[W]));
	setcond(C,0);  setcond(Z,ISZERO(y,W));  setcond(N,RMBITSET(y,W));
      }
    }
    break;
  case MULU:
    setregc(ARG2,L,
	    getreg(ARG2,W)*(rightvalue(TARG1,PARG1,W,&px)&MaskA[W])); 
    break;
  case MULS:
    setregc(ARG2,L,getreg(ARG2,W)*rightvalue(TARG1,PARG1,W,&px));
    break;
  case SCC:
    setalt(TARG1,PARG1,B,TRUE(cond(CC)));
    break;
  case SCS:
    setalt(TARG1,PARG1,B,TRUE(cond(CS)));
    break;
  case SEQ:
    setalt(TARG1,PARG1,B,TRUE(cond(EQ)));
    break;
  case SF:
    setalt(TARG1,PARG1,B,0);
    break;
  case SGE:
    setalt(TARG1,PARG1,B,TRUE(cond(GE)));
    break;
  case SGT:
    setalt(TARG1,PARG1,B,TRUE(cond(GT)));
    break;
  case SHI:
    setalt(TARG1,PARG1,B,TRUE(cond(HI)));
    break;
  case SLE:
    setalt(TARG1,PARG1,B,TRUE(cond(LE)));
    break;
  case SLS:
    setalt(TARG1,PARG1,B,TRUE(cond(LS)));
    break;
  case SLT:
    setalt(TARG1,PARG1,B,TRUE(cond(LT)));
    break;
  case SMI:
    setalt(TARG1,PARG1,B,TRUE(cond(MI)));
    break;
  case SNE:
    setalt(TARG1,PARG1,B,TRUE(cond(NE)));
    break;
  case SPL:
    setalt(TARG1,PARG1,B,TRUE(cond(PL)));
    break;
  case ST:
    setalt(TARG1,PARG1,B,TRUE(1));
    break;
  case SVC:
    setalt(TARG1,PARG1,B,TRUE(cond(VC)));
    break;
  case SVS:
    setalt(TARG1,PARG1,B,TRUE(cond(VS)));
    break;
  case BTST:
    if (SIZE == L)
      setcond(Z,!(Reg[ARG2]&MaskB[rightvalue(TARG1,PARG1,L,&px)%32]));
    else
      setcond(Z,!(rightvalue(TARG2,PARG2,B,&px)
		  & MaskB[rightvalue(TARG1,PARG1,L,&px)%8]));
    break;
  case BSET:
    if (SIZE == L)
      setreg(ARG2,L,bitset(Reg[ARG2],rightvalue(TARG1,PARG1,L,&px)%32));
    else {
      x = rightvalue(TARG2,PARG2,B,&lv);
      setatlv(lv,TARG2,B,bitset(x,rightvalue(TARG1,PARG1,L,&px)%8));
    }
    break;
  case BCLR:
    if (SIZE == L)
      setreg(ARG2,L,bitclr(Reg[ARG2],rightvalue(TARG1,PARG1,L,&px)%32));
    else {
      x = rightvalue(TARG2,PARG2,B,&lv);
      setatlv(lv,TARG2,B,bitclr(x,rightvalue(TARG1,PARG1,L,&px)%8));
    }
    break;
  case BCHG:
    if (SIZE == L)
      setreg(ARG2,L,bitchg(Reg[ARG2],rightvalue(TARG1,PARG1,L,&px)%32));
    else {
      x = rightvalue(TARG2,PARG2,B,&lv);
      setatlv(lv,TARG2,B,bitchg(x,rightvalue(TARG1,PARG1,L,&px)%8));
    }
    break;
  case TST:
    x = rightvalue(TARG1,PARG1,SIZE,&px);
    if (ExErr == NOERR) {
      setcond(N,RMBITSET(x,SIZE));  setcond(Z,ISZERO(x,SIZE));
      setcond(V,0);  setcond(C,0);
    }
    break;
  case TAS:
    x = rightvalue(TARG1,PARG1,B,&lv);
    if (ExErr == NOERR) {
      setcond(N,RMBITSET(x,B));  setcond(Z,ISZERO(x,B));
      setcond(V,0);  setcond(C,0);
      setatlv(lv,TARG1,B,(x&~MaskH[B])|MaskH[B]);
    }
    break;
  case SWAP:
    x = (Reg[ARG1]>>16)&MaskA[W];
    setregc(ARG1,L,(Reg[ARG1]<<16)+x);
    break;
  case NOT:
    setaltc(TARG1,PARG1,SIZE,~rightvalue(TARG2,PARG2,SIZE,&px));
    break;
    case NEG:
    x = rightvalue(TARG1,PARG1,SIZE,&lv);
    setatlv(lv,TARG1,SIZE,subtrc(0,x,SIZE));
    break;
  case NEGX:
    x = cond(EQ);  y = rightvalue(TARG1,PARG1,SIZE,&lv);
    setatlv(lv,TARG1,SIZE,subtrc(cond(XS),y,SIZE));
    setcond(Z,x&&cond(EQ));
    break;
  case UNLK:
    Reg[A7] = Reg[ARG1];  setreg(ARG1,L,getmem(Reg[A7],L));
    if (ExErr == NOERR)  Reg[A7] += 4;
    break;
  case LINK:
    Reg[A7] -= 4;  setmem(Reg[A7],L,Reg[ARG1]);
    if (ExErr == NOERR)  Reg[A7] += rightvalue(TARG2,PARG2,W,&px);
    break;
  case PEA:
    Reg[A7] -= 4;  setmem(Reg[A7],L,getctrladdr(TARG1,PARG1));
    break;
  case LEA:
    setreg(ARG2,L,getctrladdr(TARG1,PARG1));
    break;
  case EXG:
    x = Reg[ARG1];  Reg[ARG1] = Reg[ARG2];  Reg[ARG2] = x;
    break;
  case EXT:
    if (SIZE == W)  setregc(ARG1,W,getreg(ARG1,B));
    else  setregc(ARG1,L,getreg(ARG1,W));
    break;
  case OR: case ORI:
    if (TARG2==REG && ARG2==CCR)
      setreg(CCR,B,Reg[CCR]|rightvalue(TARG1,PARG1,W,&px));
    else {
      x = rightvalue(TARG2,PARG2,SIZE,&lv);
      setatlvc(lv,TARG2,SIZE,rightvalue(TARG1,PARG1,SIZE,&px)|x);
    }
    break;
  case EOR: case EORI:
    if (TARG2==REG && ARG2==CCR)
      setreg(CCR,B,Reg[CCR]^rightvalue(TARG1,PARG1,W,&px));
    else {
      x = rightvalue(TARG2,PARG2,SIZE,&lv);
      setatlvc(lv,TARG2,SIZE,rightvalue(TARG1,PARG1,SIZE,&px)^x);
    }
    break;
  case AND: case ANDI:
    if (TARG2==REG && ARG2==CCR)
      setreg(CCR,B,Reg[CCR]&rightvalue(TARG1,PARG1,W,&px));
    else {
      x = rightvalue(TARG2,PARG2,SIZE,&lv);
      setatlvc(lv,TARG2,SIZE,rightvalue(TARG1,PARG1,SIZE,&px)&x);
    }
    break;
  case SUB: case SUBI:
    x = rightvalue(TARG2,PARG2,SIZE,&lv);
    setatlv(lv,TARG2,SIZE,subtrc(x,rightvalue(TARG1,PARG1,SIZE,&px),SIZE));
    break;
  case SUBA:
    Reg[ARG2] -= rightvalue(TARG1,PARG1,SIZE,&px);
    break;
  case SUBQ:
    if (TARG2==REG && ARG2>=A0 && ARG2<=SP)
      Reg[ARG2] -= rightvalue(TARG1,PARG1,SIZE,&px);
    else {
      x = rightvalue(TARG2,PARG2,SIZE,&lv);
      setatlv(lv,TARG2,SIZE,subtrc(x,rightvalue(TARG1,PARG1,SIZE,&px),SIZE));
    }
    break;
  case SUBX:
    x = cond(EQ);  y = rightvalue(TARG2,PARG2,SIZE,&lv);
    setatlv(lv,TARG2,SIZE,
	    subtrc(y-cond(XS),rightvalue(TARG1,PARG1,SIZE,&px),SIZE));
    setcond(Z,x&&cond(EQ));
    break;
  case ADD: case ADDI:
    x = rightvalue(TARG2,PARG2,SIZE,&lv);
    setatlv(lv,TARG2,SIZE,addc(rightvalue(TARG1,PARG1,SIZE,&px),x,SIZE));
    break;
  case ADDA:
    Reg[ARG2] += rightvalue(TARG1,PARG1,SIZE,&px);
    break;
  case ADDQ:
    if (TARG2==REG && ARG2>=A0 && ARG2<=SP)
      Reg[ARG2] += rightvalue(TARG1,PARG1,SIZE,&px);
    else {
      x = rightvalue(TARG2,PARG2,SIZE,&lv);
      setatlv(lv,TARG2,SIZE,addc(rightvalue(TARG1,PARG1,SIZE,&px),x,SIZE));
    }
    break;
  case ADDX:
    x = cond(EQ);  y = rightvalue(TARG2,PARG2,SIZE,&lv);
    setatlv(lv,TARG2,SIZE,
	    addc(rightvalue(TARG1,PARG1,SIZE,&px),y+cond(XS),SIZE));
    setcond(Z,x&&cond(EQ));
    break;
  case CMP:
    x = cond(XS);
    subtrc(getreg(ARG2,SIZE),rightvalue(TARG1,PARG1,SIZE,&px),SIZE);
    setcond(X,x);
    break;
  case CMPA:
    x = cond(XS);
    subtrc(Reg[ARG2],rightvalue(TARG1,PARG1,SIZE,&px),L);
    setcond(X,x);
    break;
  case CMPI: case CMPM:
    x = cond(XS);
    subtrc(rightvalue(TARG2,PARG2,SIZE,&px),
	   rightvalue(TARG1,PARG1,SIZE,&px),SIZE);
    setcond(X,x);
    break;
  case CLR:
    setaltc(TARG1,PARG1,SIZE,0);
    break;
  case MOVE:
    if (TARG2==REG && ARG2==CCR)
      setreg(CCR,B,rightvalue(TARG1,PARG1,W,&px));
    else  setaltc(TARG2,PARG2,SIZE,rightvalue(TARG1,PARG1,SIZE,&px));
    break;
  case MOVEQ:
    x = rightvalue(TARG1,PARG1,W,&px);
    if ((x & 128))  x -= 256;        /* sign extend */
    setaltc(TARG2,PARG2,SIZE,x);
    break;
  case MOVEA:
    setreg(ARG2,L,rightvalue(TARG1,PARG1,SIZE,&px));
    break;
     /* jumps */
  case BRA: case JMP:  jumpaddr(TARG1,PARG1);  break;
  case BSR: case JSR:
    Reg[A7] -= 4;  setmem(Reg[A7],L,Reg[PC]);
    if (ExErr == NOERR)  jumpaddr(TARG1,PARG1);
    break;
  case RTR:
    setreg(CCR,B,getmem(Reg[A7],B));  setreg(A7,L,Reg[A7]+2);
    if (ExErr != NOERR)  break;
  case RTS:
    jumpmaddr(getmem(Reg[A7],L));  setreg(A7,L,Reg[A7]+4);
    break;
  case BCC:
    if (cond(CC))  jumpaddr(TARG1,PARG1);
    break;
  case BCS:
    if (cond(CS))  jumpaddr(TARG1,PARG1);
    break;
  case BEQ:
    if (cond(EQ))  jumpaddr(TARG1,PARG1);
    break;
  case BGE:
    if (cond(GE))  jumpaddr(TARG1,PARG1);
    break;
  case BGT:
    if (cond(GT))  jumpaddr(TARG1,PARG1);
    break;
  case BHI:
    if (cond(HI))  jumpaddr(TARG1,PARG1);
    break;
  case BLE:
    if (cond(LE))  jumpaddr(TARG1,PARG1);
    break;
  case BLS:
    if (cond(LS))  jumpaddr(TARG1,PARG1);
    break;
  case BLT:
    if (cond(LT))  jumpaddr(TARG1,PARG1);
    break;
  case BMI:
    if (cond(MI))  jumpaddr(TARG1,PARG1);
    break;
  case BNE:
    if (cond(NE))  jumpaddr(TARG1,PARG1);
    break;
  case BPL:
    if (cond(PL))  jumpaddr(TARG1,PARG1);
    break;
  case BVC:
    if (cond(VC))  jumpaddr(TARG1,PARG1);
    break;
  case BVS:
    if (cond(VS))  jumpaddr(TARG1,PARG1);
    break;
  case DBCC:
    if (!cond(CC) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBCS:
    if (!cond(CS) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBEQ:
    if (!cond(EQ) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBF:
    if (decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBGE:
    if (!cond(GE) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBGT:
    if (!cond(GT) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBHI:
    if (!cond(HI) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBLE:
    if (!cond(LE) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBLS:
    if (!cond(LS) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBLT:
    if (!cond(LT) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBMI:
    if (!cond(MI) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBNE:
    if (!cond(NE) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBPL:
    if (!cond(PL) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBVC:
    if (!cond(VC) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
  case DBVS:
    if (!cond(VS) && decrreg(ARG1,W) != -1)  jumpaddr(TARG2,PARG2);
    break;
      /* nops */
  case DBT: case END: case NOP:
    break;
      /* special effects */
  case ASSDIR:  break;  /* error during fetch-phase */
  case EXIT:  ExErr = PROGEXIT;  break;
  case GETC:
    if (TARG2 == NONE)  setregc((RGT) ARG1,B,getchar());
    else {
      x = getreg((RGT) ARG1,B);
      if (x<0 || x>=MAXUFILE || UserF[x]==NULL)  setreg((RGT) ARG2,B,-1);
      else  setreg((RGT) ARG2,B,getc(UserF[x]));
    }
    break;
  case PUTC:
    if (TARG2 == NONE)  putchar(getreg((RGT) ARG1,B));
    else {
      x = getreg((RGT) ARG1,B);
      if (x<0 || x>=MAXUFILE || UserF[x]==NULL)  setreg((RGT) ARG2,B,-1);
      else  putc(getreg((RGT) ARG2,B),UserF[x]);
    }
    break;
  case OPNF:
    lv = locateaddr(Reg[ARG1]);
    if (lv == NULL) {
      ExErr = SEGFAULT;
      break;
    }
    if ((x=getreg((RGT) ARG2,B))<0 || x>2)  setreg((RGT) ARG2,B,-1);
    else {
      for (y=0; y<MaxUF; y++)
	if (UserF[y] == NULL)  break;
      if (y == MaxUF)
	if (MaxUF+1 < MAXUFILE)  MaxUF++;
	else {
	  setreg((RGT) ARG2,B,-1);
	  break;
	}
      switch (x) {
      case 0:  UserF[y] = fopen(lv,"r");  break;
      case 1:  UserF[y] = fopen(lv,"w");  break;
      case 2:  UserF[y] = fopen(lv,"a");
      }
      setreg((RGT) ARG2,B,(UserF[y] == NULL ? -1 : y));
    }
    break;
  case CLSF:
    x = getreg((RGT) ARG1,B);
    if (x>=0 && x<MAXUFILE && UserF[x]!=NULL) {
      fclose(UserF[x]);
      if (x == MaxUF-1)  MaxUF--;
      else  UserF[x] = NULL;
    }
    break;
  default:  ExErr = ILLIERR;  break;
  }
  if (ExErr==NOERR && ((ExCInx=locatepaddr(Reg[PC]))<0 || ExCInx>=CInx)) {
    ExCInx = 0;  Reg[PC] = 0;
  }
  return(ExErr);
}

void initrun()    /* initialize before running program */
{ int i;

  for (i=PC+1; i<BADREG; i++)  Reg[i] = UNDEF;
  Reg[A7] = DATATOP;  Reg[CCR] &= 0xF;
  Reg[PC] = Code[(ExCInx=0)].caddri;
  for (i=0; i<MAXUFILE; i++)  UserF[i] = NULL;
  ExErr = NOERR;  MaxUF = 0;
}

void run()    /* execute program in memory */
{
  initrun();
  while (execinstr() == NOERR);
  printf("\n\nExecution terminated: %s\n\n",ExErrMess[ExErr]);
}

/** Interactive command handling */

int getcommand(prompt,c0,cn,ns)    /* get command in range c0-cn */
char *prompt, **ns;  int c0, cn;
{ char buff[120];  int c;
    /*
     * Addition by Eric KEDJI on 18 Feb 2007
     * Added the ability to abbreviate commands as long
     * as they are not ambiguous
     */
    int * flags = (int *) malloc((cn - c0 + 1) * sizeof(int));
    
    if(flags)
    {
        int i,j,nb_candidates,
            cmd; // saves the number of the last matching command

        while (1)
        {
            // reset the flags, a flag is raised if the command is no more a candidate
            for(i = c0; i <= cn; i++)
                *(flags + i) = 0;
            
            printf("%s ",prompt);
            getline(stdin,buff);

            // readily ask for a new command if the previous one is empty
            if(*buff == '\0')
                continue;

            for(j = 0; *(buff + j) != '\0'; j++) // iterate over characters in given command
            {
                for(i = c0, nb_candidates = 0; i <= cn; i++) // iterate over available commands
                {
                    if(*(flags + i) == 1) // command is no more a candidate, continue
                        continue;
                    
                    if(*(*(ns + i) + j) == '\0') // end of command *(ns + i), don't check anymore
                    {
                        *(flags + i) = 1;
                        continue;
                    }
                    
                    if(*(*(ns + i) + j) == *(buff + j)) // character j matches
                    {
                        cmd = i; // save the matching command number
                        nb_candidates ++;
                    } else { // character j does not match
                        *(flags + i) = 1; // no more a candidate
                    }
                } // finished scanning available commands for position j
                
                if(nb_candidates == 1) //only one candidate, we are done
                {
                    /*
                     * free(flags);
                     * the call to free causes the program to exit mysteriously
                     * with the message
                     * *** glibc detected *** free(): invalid next size (fast): 0x0809b008 ***
                     * It happens only when you enter any valid command (or abbreviation)
                     * after a program being debugged's execution has terminated with the
                     * message: Exit from program: execution terminated
                     */
                    return (cmd);
                }

                if(!nb_candidates) // no candidate, command no found
                    break; // get out of the j loop (characters in given command)
            } // finished scanning given command

            if(nb_candidates > 1) // ambiguous command
            {
                printf("%s: command ambiguous; try \"help\" for a list"
                        " of valid commands\n",buff);
            } else { // command not found , nb_candidates == 0
                printf("%s: not an interactive command; try \"help\"\n",buff);
            }
                
        } // end of while(1): if command no found, fetch another one.
    } else { // memory allocation for flags failed, do it Miguel's way ...
      while (1) {
        printf("%s ",prompt);  getline(stdin,buff);
        for (c=c0; c<=cn; c++)
          if (!strcmp(buff,*(ns+c)))  return(c);
        printf("%s: not an interactive command; try \"help\"\n",buff);
      }
    }
}

/** Help facility */

void help(c0,cn,ns,es)
int c0, cn;  char **ns, **es;
{
  printf("\t\t--- You may use the following commands:\n");

  // Next line added by Eric KEDJI for the command abbreviation feature
  printf("\t\t--- Commands may be abbreviated as long as they are not ambiguous\n\n");

  for (; c0<=cn; c0++)
    printf("\t%s\t: %s\n",*(ns+c0),*(es+c0));
  putchar('\n');
}

/** Debugging */

/* Dealing with lines */

int currentline()    /* get line for current instruction */
{ int i;

  for (i=1; i<=TxtInx; i++)
    if (ExCInx == Txtln[i])  return(i);
  return(0);  /* cannot happen... */
}

void show(ln)        /* get and print text line */
int ln;
{ int l;  char buff[120];

  if (noprogfile())  return;
  for (l=1; l<=TxtInx; l++) {
    getline(ProgF,buff);
    if (l == ln) {
      printline(l,buff);
      fclose(ProgF);
      return;
    }
  }
}

/* Breakpoints */

int badbreak(s)      /* check argument of setting breakpoint */
char *s;
{ int l;

  s = skipblanks(s);
  if (badint(s,&l,&s) || l<1 || l>=TxtInx || Txtln[l]<=0)  return(1);
  if (BrkInx > MAXBRK) {
    printf("Too many breakpoints set!\n");
    return(1);
  }
  Break[BrkInx++] = l;
  return(0);
}

int baddelete(s)      /* check argument of deleting breakpoint */
char *s;
{ int l, i;

  s = skipblanks(s);
  if (badint(s,&l,&s) || l<0)  return(1);
  for (i=0; i<BrkInx; i++)
    if (l == Break[i]) {
      while (++i < BrkInx)  Break[i-1] = Break[i];
      BrkInx--;
      return(0);
    }
  return(1);
}

/* Printing information */

void printregs()      /* display registers */
{
  printf("\tPC = %06X\t\t\tX = %d N = %d Z = %d V = %d C = %d\n\n",Reg[PC],
	 cond(XS),cond(MI),cond(EQ),cond(VS),cond(CS));
  printf("\tA0 = %08X\tA1 = %08X\tA2 = %08X\tA3 = %08X\n",
	 Reg[A0],Reg[A1],Reg[A2],Reg[A3]);
  printf("\tA4 = %08X\tA5 = %08X\tA6 = %08X\tA7 = %08X\n\n",
	 Reg[A4],Reg[A5],Reg[A6],Reg[A7]);
  printf("\tD0 = %08X\tD1 = %08X\tD2 = %08X\tD3 = %08X\n",
	 Reg[D0],Reg[D1],Reg[D2],Reg[D3]);
  printf("\tD4 = %08X\tD5 = %08X\tD6 = %08X\tD7 = %08X\n\n",
	 Reg[D4],Reg[D5],Reg[D6],Reg[D7]);
}

void printmem16(n,lv)  /* print up to 16 first bytes pointed to by lv */
int n;  BYTE *lv;
{ int k, r=16-n;

  if (n > 16) {
    n = 16;  r = 0;
  }
  printf("%06Xv  ",(ADDR) lv-(ADDR) Data+DATAORIGIN);
  for (k=0; k<n; k++)  printf("%02X ",(*lv++)&0xFF);
  while (r--)  printf("   ");
  lv -= n;
  printf("    ");
  for (k=0; k<n; k++, lv++)  printf("%c",(*lv<32 ? '?' : *lv));
  putchar('\n');
}

int badprintmem(s)   /* check arguments and print memory */
char *s;
{ int n;  ADDR a;  BYTE *lv;  EXECERR e=ExErr;

  s = skipblanks(s);
  if (badint(s,&n,&s) || n<1)  return(1);
  s = skipblanks(s);
  if (*s == ',')  s = skipblanks(s+1);
  if (badint(s,&a,&s) || (lv=locateaddr(a))==NULL || !goodlv(lv+n)) {
    ExErr = e;
    return(1);
  }
  do {
    printmem16(n,lv);  lv += 16;  n -= 16;
  } while (n > 0);
  return(0);
}

/* Setting registers and memory */

int badvalforreg(s)       /* parse and get value for setting a register */
char *s;
{ ADDR r, v;  IATTR a = L;

  s = skipblanks(s);
  if (badreg(s,&r,&s))  return(1);
  if (*s == '.') {
    switch (*++s) {
    case 'B':
      if (r==PC || (r>=A0 && r<=SP))  return(1);
      a = B;
      break;
    case 'W':
      if (r==CCR || r==PC)  return(1);
      a = W;
      break;
    case 'L':
      if (r == CCR)  return(1);
      break;
    default:  return(1);
    }
    s++;
  } else if (r==CCR)  a = B;
  if (*s == ',')  s++;
  s = skipblanks(s);
  if (badint(s,&v,&s) || badeol(s))  return(1);
  setreg(r,a,v);
  return(0);
}

int badvalformem(s)       /* parse and get values for setting memory */
char *s;
{ ADDR a, n, clist[MAXCLIST], *p;  EXECERR e=ExErr;  char buff[500];
  BYTE *lv;

  s = skipblanks(s);
  if (badint(s,&a,&s) || badeol(s) || (lv=locateaddr(a))==NULL) {
    ExErr = e;
    return(1);
  }
  getanswer("Constant list (.B)",buff);
  s = skipblanks(buff);
  if (badclist(s,clist,B,&n,&s) || n>MAXCLIST || badeol(s) ||
      !goodlv(lv+n)) {
    ExErr = e;
    return(1);
  }
  p = clist;
  while (n--)  *lv++ = *p++;
  return(0);
}

/* Main control */

int notatbreak(ln)    /* check whether debugger must stop */
int ln;
{ int i;

  if (Step)  return(0);
  for (i=0; i<BrkInx; i++)
    if (ln == Break[i])  return(0);
  return(1);
}

int debugit(c)      /* debug before instruction execution or after exit */
DCMMD c;
{ char buff[120];  int cline = currentline();

  if (ExErr==DEBUGQUIT || (c==RUNNING && notatbreak(cline)))  return(0);
  printregs(); // Added by Eric KEDJI to mimic S.E.R.I Electronics' debugger
  show(cline);
  while (1)
    switch (getcommand(DPrompt,c,QUIT,DCMDName)) {
    case STEP:  Step = 1;  return(0);
    case CONT:  return((Step=0));
    case NODEBUG:  return((Debug=0));
    case BREAK:
      getanswer("Break at line number",buff);
      if (badbreak(buff))  printf("Cannot set breakpoint there...\n");
      break;
    case DEL:
      getanswer("Delete break at line number",buff);
      if (baddelete(buff))  printf("No breakpoint there...\n");
      break;
    case PRREG: printregs();  break;
    case SETREG:
      getanswer("Register[.attribute], value",buff);
      if (badvalforreg(buff))  printf("Wrong arguments...\n");
      break;
    case PRMEM:
      getanswer("Number of bytes, memory address",buff);
      if (badprintmem(buff))  printf("Wrong arguments...\n");
      break;
    case SETMEM:
      getanswer("Memory address",buff);
      if (badvalformem(buff))  printf("Wrong arguments...\n");
      break;
    case SHLN:  show(cline);  break;
    case DLIST:  list();  break;
    case DHELP:  help(c,QUIT,DCMDName,DCMDExpl);  break;
    case RESTART:
      Step = 1;  
      if (c == POSTMORTEM) {
	ExErr = NOERR;
	return(1);
      }
      initrun();
      cline = currentline();
      show(cline);
      break;
    case QUIT:  ExErr = DEBUGQUIT;  return(1);
    default:
      printf("... Sorry, not implemented\n");
    }
}

/** Main commands */

int docommand()
{
  switch (getcommand(Prompt,ASS,BYE,CMDName)) {
  case HELP:  help(ASS,BYE,CMDName,CMDExpl);  break;
  case CLEAR: clear(); break;
  case PFILE:
    getanswer("Filename",ProgFName);  clear();
    break;
  case ASS: 
    if (CInx)
      printf("You already have a program in memory - use \"clear\" first\n");
    else {
      ass();
      if (CInx)  setprogversion();
    }
    break;
  case LIST:
    if (! CInx)
      printf("There is no program in memory - use \"assemble\" first\n");
    else list();
    break;
  case DEBUG:
    Debug = Step = 1;  BrkInx = 0;
  case RUN:
    if (! CInx)
      printf("There is no program in memory - use \"assemble\" first\n");
    else
      do {
	run();
	if (Debug)  debugit(POSTMORTEM);
      } while (ExErr == NOERR);   /* debugger restart */
    Debug = 0;
    break;
  case EDIT:
    if (!*ProgFName)  getanswer("Filename",ProgFName);
    edit();
    break;
  case SETED:
    getanswer("Editor",EditorName);
    break;
  case BYE:  return(0);
  }
  return(1);
}

int main(int argc, char ** argv)
{
  printf("\n\n\t*** Environment for programming a MC68000 (version %s) ***\n\n",
	 VERSION);
  seteditor();

  // Eric KEDJI: Added the ability to specify filename on the command line
  if(argc != 2)
      getanswer("Filename",ProgFName);
  else
      strcpy(ProgFName,argv[1]);
  
  clear();
  while (docommand());
  return EXIT_SUCCESS;
}
  
