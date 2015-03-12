using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace CompCS
{
    class VM
    {
        public const int VM_SIZE = 1024 * 8;
        const int SP0 = 6;
        const int RP0 = 7;
        const int SPP = 33;
        const int RPP = 34;
        const ushort TRUE = 0xFFFF;
        const ushort FALSE = 0;
        int PC;
        int regA;
        int regB;
        ushort[] data;

        public void poke(int position, ushort value)
        {
            data[position] = value;
        }

        public ushort peek(int position)
        {
            return data[position];
        }

        public VM()
        {
            data = new ushort[VM_SIZE];
            data[SPP] = data[SP0];
            data[RPP] = data[RP0];
        }

        void PUSHDS(ushort val)
        {
            data[data[SPP]] = val;
            data[SPP] = (ushort)(data[SPP] + 1);
        }

        void PUSHRS(ushort val)
        {
            data[data[RPP]] = val;
            data[RPP] = (ushort)(data[RPP] - 1);
        }

        ushort POPDS()
        {
            data[SPP] = (ushort)(data[SPP] - 1);
            return data[data[SPP]];
        }

        ushort POPRS()
        {
            data[RPP] = (ushort)(data[RPP] + 1);
            return data[data[RPP]];
        }

        void NEXT()
        {
            PC += 1;
        }

        public int run(int start)
        {
            PUSHRS(VM_SIZE);
            PC = start;
            while (PC < VM_SIZE)
            {
                switch (data[PC])
                {
                    case 0: //NOP
                        NEXT();
                        break;
                    case 1: //HALT
                        return -1;
                    case 2: // LIT
                        NEXT();
                        PUSHDS(data[PC]);
                        NEXT();
                        break;
                    case 3: // EXIT
                        PC = POPRS();
                        break;
                    case 4: // LOAD
                        regA = POPDS();
                        PUSHDS(data[regA]);
                        NEXT();
                        break;
                    case 5: // STORE
                        regA = POPDS();
                        regB = POPDS();
                        data[regA] = (ushort)regB;
                        NEXT();
                        break;
                    case 6: //DROP
                        POPDS();
                        NEXT();
                        break;
                    case 7: // OVER
                        regA = POPDS();
                        regB = POPDS();
                        PUSHDS((ushort)regB);
                        PUSHDS((ushort)regA);
                        PUSHDS((ushort)regB);
                        NEXT();
                        break;
                    case 8: // SWAP
                        regA = POPDS();
                        regB = POPDS();
                        PUSHDS((ushort)regA);
                        PUSHDS((ushort)regB);
                        NEXT();
                        break;
                    case 9: // DUP
                        regA = POPDS();
                        PUSHDS((ushort)regA);
                        PUSHDS((ushort)regA);
                        NEXT();
                        break;
                    case 10: // UM+
                        regA = POPDS();
                        regB = POPDS();
                        regA = regA + regB;
                        if (regA > 0xFFFF)
                        {
                            PUSHDS((ushort)(regA & 0xFFFF));
                            PUSHDS(1);
                        }
                        else
                        {
                            PUSHDS((ushort)regA);
                            PUSHDS(0);
                        }
                        NEXT();
                        break;
                    case 11: // NAND
                        regA = POPDS();
                        regB = POPDS();
                        PUSHDS((ushort)~(regA & regB));
                        NEXT();
                        break;
                    case 12: // 0<
                        regA = POPDS();
                        PUSHDS(regA > 0x8000 ? TRUE : FALSE);
                        NEXT();
                        break;
                    case 13: // >R 
                        regA = POPDS();
                        PUSHRS((ushort)regA);
                        NEXT();
                        break;
                    case 14: // R>
                        regA = POPRS();
                        PUSHDS((ushort)regA);
                        NEXT();
                        break;
                    case 15: // next
                        NEXT();
                        regA = data[PC];
                        regB = POPRS() - 1;
                        if (regB < 0)
                        {
                            NEXT();
                        }
                        else
                        {
                            PUSHRS((ushort)regB);
                            PC = regA;
                        }
                        break;
                    case 16: // BRANCH
                        NEXT();
                        PC = data[PC];
                        break;
                    case 17: // 0BRANCH
                        regA = POPDS();
                        NEXT();
                        regB = data[PC];
                        if (regA == FALSE)
                        {
                            PC = regB;
                        }
                        else
                        {
                            NEXT();
                        }
                        break;
                    case 18: // EXECUTE
                        NEXT();
                        PUSHRS((ushort)PC);
                        PC = POPDS();
                        break;
                    case 19: // TX!
                        regA = POPDS();
                        Console.Write((char)regA);
                        NEXT();
                        break;
                    case 20: // ?RX
                        regA = Console.Read();
                        PUSHDS((ushort)regA);
                        PUSHDS(TRUE);
                        NEXT();
                        break;
                    default: // Call
                        PUSHDS(data[PC]);
                        NEXT();
                        PUSHRS((ushort)PC);
                        PC = POPDS();
                        break;
                }
            }
            return 0;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            VM vm = new VM();
            FileStream fs = new FileStream("eforth.img", FileMode.Open);
            BinaryReader br = new BinaryReader(fs);
            for (int x = 0; x < VM.VM_SIZE; x++)
            {
                ushort byte1 = br.ReadByte();
                byte1 = (ushort)(byte1 << 8);
                ushort byte2 = br.ReadByte();
                vm.poke(x, (ushort)(byte1 | byte2));
            }
            fs.Close();
            vm.run(0);
        }
    }
}
