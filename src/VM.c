#include <stdio.h>

#define VM_SIZE (1024 * 8)

struct VM { int PC; int regA; int regB; unsigned short data[VM_SIZE]; };

#define PUSHDS(X) vm->data[vm->data[SPP]] = X;vm->data[SPP]++
#define PUSHRS(X) vm->data[vm->data[RPP]] = X;vm->data[RPP]--
#define SP0      6 // Top of data stack
#define RP0      7 // Top of return stack
#define POPDS vm->data[--vm->data[SPP]]
#define POPRS vm->data[++vm->data[RPP]]
#define NEXT  vm->PC += 1
#define SPP     33 // Data stack pointer
#define RPP     34 // Return stack pointer
#define TRUE  0xFFFF
#define FALSE 0
int vm_run(struct VM * vm, int start)
{
    PUSHRS(VM_SIZE);
    vm->PC = start;
    while(vm->PC < VM_SIZE)
    {
        switch(vm->data[vm->PC])
        {
    case 0: // NOP
            NEXT;
            break;
        case 1: // HALT
            return -1;
        case 2: // LIT
            NEXT;
            PUSHDS(vm->data[vm->PC]);
            NEXT;
            break;
        case 3: // EXIT
            vm->PC = POPRS;
            break;
        case 4: // LOAD
            vm->regA = POPDS;
            PUSHDS(vm->data[vm->regA]);
            NEXT;
            break;
        case 5: // STORE
            vm->regA = POPDS;
            vm->regB = POPDS;
            vm->data[vm->regA] = vm->regB;
            NEXT;
            break;
        case 6: // DROP
            POPDS;
            NEXT;
            break;
        case 7: // OVER
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(vm->regB);
            PUSHDS(vm->regA);
            PUSHDS(vm->regB);
            NEXT;
            break;
        case 8: // SWAP
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(vm->regA);
            PUSHDS(vm->regB);
            NEXT;
            break;
        case 9: // DUP
            vm->regA = POPDS;
            PUSHDS(vm->regA);
            PUSHDS(vm->regA);
            NEXT;
            break;
        case 10: // UM+
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
        case 11: // NAND
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS(~(vm->regA & vm->regB));
            NEXT;
            break;
        case 12: // 0<
            vm->regA = POPDS;
            PUSHDS(vm->regA > 0x8000 ? TRUE : FALSE);
            NEXT;
            break;
        case 13: // >R
            vm->regA = POPDS;
            PUSHRS(vm->regA);
            NEXT;
            break;
        case 14: // R>
            vm->regA = POPRS;
            PUSHDS(vm->regA);
            NEXT;
            break;
        case 15: // next
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
        case 16: // BRANCH
            NEXT;
            vm->PC = vm->data[vm->PC];
            break;
        case 17: // ZBRANCH
            vm->regA = POPDS;
            NEXT;
            vm->regB = vm->data[vm->PC];
            if(vm->regA == FALSE)
                vm->PC = vm->regB;
            else
                NEXT;
            break;
        case 18: // EXECUTE
            NEXT;
            PUSHRS(vm->PC);
            vm->PC = POPDS;
            break;
        case 19: // TX!
            vm->regA = POPDS;
            fputc(vm->regA, stdout);
            NEXT;
            break;
        case 20: // ?RX
            vm->regA = fgetc(stdin);
            PUSHDS(vm->regA);
            PUSHDS(TRUE);
            NEXT;
            break;
        default: // CALL
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

void vm_init(struct VM * vm, FILE * fi, int size)
{
    int x;
    unsigned char val1  = 0;
    unsigned short val2 = 0;
    for(x = 0;x < size;x++)
    {
        val1 = fgetc(fi);
        val2 = val1 << 8;
        val1 = fgetc(fi);
        val2 |= val1;
        vm_poke(vm, x, val2);
    }
    vm_poke(vm, SPP,     vm_peek(vm, SP0));
    vm_poke(vm, RPP,     vm_peek(vm, RP0));
}

struct VM vm;
int main()
{
    FILE * f = fopen("eforth.img","rb");
    vm_init(&vm, f, VM_SIZE);
    fclose(f);
    vm_run(&vm,0);
    return 1;
}
