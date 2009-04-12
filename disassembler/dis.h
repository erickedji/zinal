/*
**
** DIS.H
**
** Well, nobody is going to write a 68000 disasembler for me
*/

typedef struct {
	char *op;	/*	pointer to assembly nemonic	*/
	unsigned op_code;	/*	base op code	*/
	unsigned mask;		/*	mask to get rid of variable data	*/
	unsigned *(*d_funct)(unsigned *c,char *s,int index);	/*	function to call	*/
}OP_DEF;

/*
** size tokens
*/

#define BYTE_SIZE	256
#define WORD_SIZE	257
#define LONG_SIZE	258

extern void *dis(void *c,char *s);

