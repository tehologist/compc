
from struct import unpack
vals = []
f = open('eforth.img','rb')
for a in range(1024 * 8):
    vals.append(unpack(">H", f.read(2))[0])
f.close()

forth = "const PROGMEM uint16_t image[] = {\n"
for b in range(512):
    forth += ((" " * 4) + ("%i, " * 16) + "\n") % tuple(vals[16 * b:b * 16 + 16])
forth += "};"

head = """
#include <avr/pgmspace.h>

#define VM_SIZE (1024 * 8)

struct VM { int PC; int regA; int regB; unsigned int data[512]; };

"""

tail = """

#define SP0      6 // Top of data stack
#define RP0      7 // Top of return stack
#define POPDS vm->data[--vm->data[SPP]]
#define POPRS vm->data[++vm->data[RPP]]
#define NEXT  vm->PC += 1
#define SPP     33 // Data stack pointer
#define RPP     34 // Return stack pointer
#define TRUE  0xFFFF
#define FALSE 0
#define PUSHDS(X) vm->data[vm->data[SPP]] = X;vm->data[SPP]++
#define PUSHRS(X) vm->data[vm->data[RPP]] = X;vm->data[RPP]--
int vm_run(struct VM * vm, int start)
{
    PUSHRS(VM_SIZE);
    vm->PC = start;
    unsigned short opcode = 0;
    unsigned long x;
    unsigned long y;
    while(vm->PC < VM_SIZE)
    {

        if(vm->PC > 512)
        {
          opcode = pgm_read_word_near(image + vm->PC); 
        }
        else
        {
          opcode = vm->data[vm->PC];
        }
        switch(opcode)
        {
        case 0: // NOP
            NEXT;
            break;
        case 1: // HALT
            return -1;
        case 2: // LIT
            NEXT;
            if(vm->PC > 511)
            {
              PUSHDS(pgm_read_word_near(image + vm->PC)); 
            }
            else
            {
              PUSHDS(vm->data[vm->PC]);
            }
            NEXT;
            break;
        case 3: // EXIT
            vm->PC = POPRS;
            break;
        case 4: // LOAD
            vm->regA = POPDS;
            if(vm->regA > 511)
            {
              PUSHDS(pgm_read_word_near(image + vm->regA));
            }
            else
            {
                PUSHDS(vm->data[vm->regA]);
            }
            NEXT;
            break;
        case 5: // STORE
            vm->regA = POPDS;
            vm->regB = POPDS;
            if(vm->regA < 512)
            {
                vm->data[vm->regA] = vm->regB;
            }
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
            x = POPDS;
            y = POPDS;
            x = x + y;
            if(x > 0xFFFF) {
                PUSHDS(x & 0xFFFF);
                PUSHDS(1);
            }
            else {
                PUSHDS(x);
                PUSHDS(0);
            }
            NEXT;
            break;
        case 11: // NAND
            vm->regA = POPDS;
            vm->regB = POPDS;
            PUSHDS((~(vm->regA & vm->regB)) & 0xFFFF);
            NEXT;
            break;
        case 12: // 0<
            PUSHDS(POPDS > 0x8000 ? TRUE : FALSE);
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
            if(vm->PC > 511)
            {
              vm->regA = pgm_read_word_near(image + vm->PC);
            }
            else
            {
                vm->regA = vm->data[vm->PC];
            }
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
            if(vm->PC > 511)
            {
              vm->PC = pgm_read_word_near(image + vm->PC);
            }
            else
            {
                vm->PC = vm->data[vm->PC];
            }
            break;
        case 17: // 0BRANCH
            vm->regA = POPDS;
            NEXT;
            if(vm->PC > 511)
            {
              vm->regB = pgm_read_word_near(image + vm->PC);
            }
            else
            {
                vm->regB = vm->data[vm->PC];
            }
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
            Serial.write((char)vm->regA);
            NEXT;
            break;
        case 20: // ?RX
            vm->regA = Serial.read();
            if(vm->regA == -1)
            {
                PUSHDS(FALSE);
            }
            else
            {
              PUSHDS(vm->regA);
              PUSHDS(TRUE);
            }
            NEXT;
            break;
        default: // CALL
            if(vm->PC > 511)
            {
              PUSHDS(pgm_read_word_near(image + vm->PC)); 
            }
            else
            {
              PUSHDS(vm->data[vm->PC]);
            }
            NEXT;
            PUSHRS(vm->PC);
            vm->PC = POPDS;
            break;
        }
    }
    return 0;
};

void vm_init(struct VM * vm)
{
  int k;
  for(k=0;k < 512;k++)
  {
    vm->data[k] = pgm_read_word_near(image + k);  
  }
}

struct VM vm;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vm_init(&vm);
}

void loop() {
  // put your main code here, to run repeatedly:
  vm_run(&vm, 0);
  Serial.println(\"\\nRestart\\n\");
}
"""
print head + forth + tail 
