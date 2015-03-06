#include <stdio.h>

#ifdef _DEBUG
#define debug_printf(...) printf(__VA_ARGS__);
#else
#define debug_printf(...) 
#endif

#define VM_SIZE (1024 * 8)
#define SP0      6 // Top of data stack
#define RP0      7 // Top of return stack
#define SPP     33 // Data stack pointer
#define RPP     34 // Return stack pointer
#define TO_IN   17 // >IN
#define NUM_TIB 18 // #TIB
#define TIB     19 // Text input buffer
#define CURRENT 27 // Current vocabulary
#define CP      29 // Code Pointer
#define NP      30 // Next cell in name dictionary
#define LAST    31 // Last name in dictionary
#define STATE   32 // State of compiler
#define TRUE  0xFFFF
#define FALSE 0

struct VM { int PC; int regA; int regB; unsigned short data[VM_SIZE]; };

typedef enum {
    OP_NOP, 
    OP_HALT, 
    OP_LIT, 
    OP_EXIT, 
    OP_LOAD, 
    OP_STORE, 
    OP_DROP, 
    OP_OVER, 
    OP_SWAP,
    OP_DUP,
    OP_UMADD,
    OP_NAND,
    OP_LTZ,
    OP_TO_R,
    OP_FROM_R,
    OP_NEXT,
    OP_BRANCH,
    OP_ZBRANCH,
    OP_EXECUTE,
    OP_PUTC,
    OP_GETC,
    LAST_OP,
} VM_OPCODE;

typedef enum
{
    INT_TICK = LAST_OP,
    INT_COLON,
    INT_END_COLON,
} INT_OPCODE;

typedef enum 
{
    PRIMITIVE,
    INTERPRETER,
    WORDLIST,
    IMMEDIATE,
} INT_TYPE ;

#define PUSHDS(X) vm->data[vm->data[SPP]] = X;vm->data[SPP]++
#define PUSHRS(X) vm->data[vm->data[RPP]] = X;vm->data[RPP]--
#define POPDS vm->data[--vm->data[SPP]]
#define POPRS vm->data[++vm->data[RPP]]
#define NEXT  vm->PC += 1
int vm_run(struct VM * vm, int start)
{
	PUSHRS(VM_SIZE);
    vm->PC = start;
    while(vm->PC < VM_SIZE)
    {
        switch(vm->data[vm->PC])
        {
	    case OP_NOP:
            NEXT;
            break;
        case OP_HALT:
            return -1;
        case OP_LIT:
            NEXT;
            PUSHDS(vm->data[vm->PC]);
            NEXT;
            break;
        case OP_EXIT: // Return
            vm->PC = POPRS;
            break;
        case OP_LOAD:
            vm->regA = POPDS;
            PUSHDS(vm->data[vm->regA]);
            NEXT;
            break;
        case OP_STORE:
            vm->regA = POPDS;
            vm->regB = POPDS;
            vm->data[vm->regA] = vm->regB;
            NEXT;
            break;
        case OP_DROP:
            POPDS;
            NEXT;
            break;
        case OP_OVER:
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(vm->regB);
            PUSHDS(vm->regA);
            PUSHDS(vm->regB);
            NEXT;
            break;
        case OP_SWAP:
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(vm->regA);
            PUSHDS(vm->regB);
            NEXT;
            break;
        case OP_DUP:
            vm->regA = POPDS;
            PUSHDS(vm->regA);
            PUSHDS(vm->regA);
            NEXT;
            break;
        case OP_UMADD:
            vm->regA = POPDS;
            vm->regB = POPDS;
            vm->regA = vm->regA + vm->regB;
            if(vm->regA > 0xFFFF) {
                PUSHDS(vm->regA & 0xFFFF);
                PUSHDS(1);
            }
            else {
                PUSHDS(vm->regA);
                PUSHDS(0);
            }
            NEXT;
            break;
        case OP_NAND:
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(~(vm->regA & vm->regB));
            NEXT;
            break;
        case OP_LTZ: // Less than 0
            vm->regA = POPDS;
            PUSHDS(vm->regA > 0x8000 ? TRUE : FALSE);
            NEXT;
            break;
        case OP_TO_R:
            vm->regA = POPDS;
            PUSHRS(vm->regA);
            NEXT;
            break;
        case OP_FROM_R:
            vm->regA = POPRS;
            PUSHDS(vm->regA);
            NEXT;
            break;
        case OP_NEXT:
            NEXT;
            vm->regA = vm->data[vm->PC];
            vm->regB = POPRS - 1;
            if(vm->regB < 0)
                NEXT;
            else {
                PUSHRS(vm->regB);
                vm->PC = vm->regA;
            }
            break;
        case OP_BRANCH:
            NEXT;
            vm->PC = vm->data[vm->PC];
            break;
        case OP_ZBRANCH:
            vm->regA = POPDS;
            NEXT;
            vm->regB = vm->data[vm->PC];
            if(vm->regA == FALSE)
                vm->PC = vm->regB;
            else
                NEXT;
            break;
        case OP_EXECUTE:
            NEXT;
            PUSHRS(vm->PC);
            vm->PC = POPDS;
            break;
        case OP_PUTC:
            vm->regA = POPDS;
            fputc(vm->regA, stdout);
            NEXT;
            break;
        case OP_GETC:
            vm->regA = fgetc(stdin);
            PUSHDS(vm->regA);
            PUSHDS(TRUE);
            NEXT;
            break;
        default: // Call
            PUSHDS(vm->data[vm->PC]);
            NEXT;
            PUSHRS(vm->PC);
            vm->PC = POPDS;
            break;
        }
    }
    return 0;
}

void vm_poke(struct VM * vm, int position, unsigned short value)
{
    vm->data[position] = value;
}

unsigned short vm_peek(struct VM * vm, int position)
{
    return vm->data[position] ;
}

void vm_init(struct VM * vm)
{
    int x;
    for(x = 0;x < VM_SIZE;x++)
    {
        vm->data[x] = 0;
    }
    vm_poke(vm, SP0,     35);
    vm_poke(vm, SPP,     35);
    vm_poke(vm, RP0,     99);
    vm_poke(vm, RPP,     99);
}

int int_getCompiler(struct VM * vm) { return vm_peek(vm, STATE); }
void int_setcompiler(struct VM * vm, int val) { vm_poke(vm, STATE, val); }

int int_readline(struct VM * vm, char * txt)
{
    int size = 0;
        
    for(;;)
    {
        if(txt[size] == '\0')
            return - 1;
        if(txt[size] != '\n')
            vm_poke(vm, vm_peek(vm, TIB) + size, txt[size]);
        else
            break;
        size++;
    }
    vm_poke(vm,vm_peek(vm, NUM_TIB),size);
    vm_poke(vm, TO_IN, 0);
    return size + 1;
}

#define HERE vm_peek(vm, CP)

void int_create(struct VM * vm, char * word, int size,unsigned short type, unsigned short data)
{
    int x;
    int charstart;
    int position = vm_peek(vm, NP);
    position -= (size);
    charstart = (position + 1);
    vm_poke(vm, position, size);
    vm_poke(vm, position - 3, vm_peek(vm, LAST));
    vm_poke(vm, LAST, position);
    for(x=0;x < size; x++)
    {
        vm_poke(vm, charstart + x, word[x]);
    }
    vm_poke(vm, position - 1, type);
    vm_poke(vm, position - 2, data);
    vm_poke(vm, NP, position - 4);
}

void int_init(struct VM *vm)
{
    vm_poke(vm, NP,      VM_SIZE - 1);
    vm_poke(vm, LAST,    0);
    int_create(vm, "@",       1, PRIMITIVE, OP_LOAD);
    int_create(vm, "!",       1, PRIMITIVE, OP_STORE);
    int_create(vm, "DROP",    4, PRIMITIVE, OP_DROP);
    int_create(vm, "OVER",    4, PRIMITIVE, OP_OVER);
    int_create(vm, "SWAP",    4, PRIMITIVE, OP_SWAP);
    int_create(vm, "UM+",     3, PRIMITIVE, OP_UMADD);
    int_create(vm, ":",       1, INTERPRETER, INT_COLON);
    int_create(vm, ";",       1, INTERPRETER, INT_END_COLON);
    int_create(vm, "[\']",    3, INTERPRETER, INT_TICK);
    vm_poke(vm, CURRENT, vm_peek(vm,LAST));
	vm_poke(vm, CP,      181);
    vm_poke(vm, TO_IN,   0);
    vm_poke(vm, NUM_TIB, 100);
    vm_poke(vm, TIB,     101);
}

int int_word(struct VM * vm, char * word)
{
    int numIn = vm_peek(vm,TO_IN);
    int length = vm_peek(vm,vm_peek(vm, NUM_TIB));
    int start = numIn;
    int count = 0;
    while(numIn < length)
    {
        char c = (char)vm_peek(vm, vm_peek(vm,TIB) + numIn);
        numIn++;
        if(c == ' ')
        {
            word[count] = '\0';
            break;
        }
        else
        {
            word[count] = c;
        }
        count++;
    }
    vm_poke(vm, TO_IN, numIn);
    return numIn - start;
}

int int_find(struct VM *vm, char * word, int size)
{
    int start = vm_peek(vm, CURRENT);
    int len = vm_peek(vm, start);
    while(start != 0)
    {
        if(len == size)
        {
            int match = TRUE;
            int x;
            for(x = 0;x < len;x++)
            {
                match &= (word[x] == vm_peek(vm, start + 1 + x));  
            }
            if(match > FALSE)
                return start;
        }
        start = vm_peek(vm, start - 3);
        len = vm_peek(vm, start);
    }
    return 0;
}

int int_number(int * val, char * word, int size)
{
    int ndx = 0;
    int negative = 0;
    if(size == 0)
        return 0;
    if(word[ndx] == '-')
    {
        negative = 1;
        ndx++;
    }
    while(ndx < size)
    {
        if(word[ndx] >= '0' && word[ndx] <= '9')
            *val = ((*val * 10) + (word[ndx] - '0'));
        else
            return 0;
        ndx++;
    }
    if(negative == 1)
        *val = ~*val + 1;
    return 1;
}

void int_comma(struct VM * vm, unsigned short val)
{
    vm_poke(vm, HERE, val);
    vm_poke(vm, CP, HERE + 1);
}

int int_interpret(struct VM * vm)
{
    int sz;
    int found;
    char word[128];
    int num;
    int type;
    int data;
    while(vm_peek(vm,TO_IN) < vm_peek(vm,vm_peek(vm,NUM_TIB)))
    {
        sz = int_word(vm, word);
        found = int_find(vm, word, sz-1);
        if(found != FALSE)
        {
            type = vm_peek(vm, found - 1);
            data = vm_peek(vm, found - 2);
            if(type == INTERPRETER)
            {
                char word[128];
                int sz;
                switch(data)
                {
                case INT_TICK:
                    sz = int_word(vm, word);
                    if((found = int_find(vm, word, sz - 1)) != 0)
					{
                        //PUSHDS(found = vm_peek(vm, found - 2));
						found = vm_peek(vm, found - 2);         // Get XT 
						vm_poke(vm, vm_peek(vm,SPP),found);     // Push XT to top of stack
						vm_poke(vm, SPP, vm_peek(vm, SPP) + 1); // Update stack pointer
					}
                    debug_printf("TICK: %s Data: %i\n", word,found);
                    break;
                case INT_COLON:
                    sz = int_word(vm, word);                       // Get next word
                    debug_printf("Defining: %s\n",word);
                    int_create(vm, word, sz - 1, WORDLIST, HERE); // Create new dictionary entry
                    int_setcompiler(vm, TRUE);                    // Turn compiler on 
                    break;
                case INT_END_COLON:
                    debug_printf("End Definition\n");
                    int_comma(vm, OP_EXIT);
                    int_setcompiler(vm, FALSE);
                    vm_poke(vm, CURRENT, vm_peek(vm, LAST));
                    break;
                default:
                    break;
                }
            }
            else if(type == IMMEDIATE || 
                (type == WORDLIST && int_getCompiler(vm) == FALSE))
            {
                debug_printf("Executing %s\n",word);
                if(vm_run(vm, data) == -1)
                    return -1;
            }
            else
            {
                debug_printf("Compiling %s\n",word);
                int_comma(vm, data);
            }
        }
        else
        {
            num = 0;
            if(int_number(&num,word,sz - 1))
            {
                if(int_getCompiler(vm) == FALSE)
                {
                    debug_printf("Pushing Number: %i\n", num);
                    //PUSHDS(num);
					vm_poke(vm, vm_peek(vm,SPP), num);      // Push number to top of stack
					vm_poke(vm, SPP, vm_peek(vm, SPP) + 1); // Update stack pointer
                }
                else
                {
                    debug_printf("Compiling Number: %i\n", num);
                    int_comma(vm, OP_LIT);
                    int_comma(vm, num);
                }
            }
            else
            {
                if(word[0] != 0)
                {
                    word[sz] = '\0';
                    debug_printf("? %s\n",word);
                }
            }
        }
    }
    return 1;
}

int int_readfile(FILE * fp, char * stream)
{
    char c = 1;
    int i;
    for(;;)
    {
        i = 0;
        while(c != '\n')
        {
            c = fgetc(fp);
            stream[i] = c;
            i++;
            if(c == EOF)
                return -1;
        }
        stream[i] = '\0';
        return i - 1;
    }
}

int interpret(struct VM * vm, FILE * fi, char * strm, int ok)
{
    while(int_readfile(fi, strm) != -1)
    {
        int_readline(vm, strm);
        if(int_interpret(vm) == -1)
            return -1;
        if(ok)
            printf("\nOK. ");
    }
    return 0;
}

void saveVM(struct VM * vm, FILE * fi, int size)
{
    int x;
    for(x = 0; x < size; x++)
    {
        fputc(((vm->data[x] >> 8) & 0xFF),fi);
        fputc((vm->data[x] & 0xFF),fi);
    }
}

struct VM vm;

int main()
{
    FILE * f = fopen("eforth.cmp","r");
    char stream[128];
    vm_init(&vm);
    int_init(&vm);
    interpret(&vm, f, stream,0);
    fclose(f);
    f = fopen("eforth.img","wb");
    saveVM(&vm, f, VM_SIZE);
    fclose(f);
    interpret(&vm, stdin, stream,1);
    return 1;
}
