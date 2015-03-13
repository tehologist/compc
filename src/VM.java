
import java.io.*;

class VM {
	private int data[];
	
	public static final int VM_SIZE = 1024 * 8;
	final int SP0 = 6;
	final int RP0 = 7;
	final int SPP = 33;
	final int RPP = 34;
	final int TRUE = 0xFFFF;
	final int FALSE = 0;
	int PC;
	int regA;
	int regB;
	
    public void poke(int position, int value)
    {
        data[position] = (value & 0xFFFF);
    }

    public int peek(int position)
    {
        return (data[position] & 0xFFFF);
    }
    
    public VM()
    {
    	data = new int[VM_SIZE];
    	poke(SPP,peek(SP0));
    	poke(RPP,peek(RP0)); 
    }
	
    void PUSHDS(int val)
    {
    	poke(peek(SPP), val);
    	poke(SPP,peek(SPP) + 1);	
    }
    
    void PUSHRS(int val)
    {
    	poke(peek(RPP), val);
    	poke(RPP,peek(RPP) - 1);	
    }
    
    int POPDS()
    {
    	poke(SPP,peek(SPP) - 1);
    	return peek(peek(SPP));
    }
    
    int POPRS()
    {
    	poke(RPP,peek(RPP) + 1);
    	return peek(peek(RPP));
    }
	
    void NEXT()
    {
    	PC += 1;
    }
    
    public int run(int start)
    {
    	PUSHRS(VM_SIZE);
    	PC = start;
    	while(PC < VM_SIZE)
    	{
    		switch(peek(PC))
    		{
    		case 0: // NOP
    			NEXT();
    			break;
    		case 1: // HALT
    			return -1;
    		case 2: // LIT
    			NEXT();
    			PUSHDS(peek(PC));
    			NEXT();
    			break;
    		case 3: // EXIT 
    			PC = POPRS();
    			break;
    		case 4: // LOAD
    			regA = POPDS();
    			PUSHDS(peek(regA));
    			NEXT();
    			break;
    		case 5: // STORE
    			regA = POPDS();
    			regB = POPDS();
    			poke(regA, regB);
    			NEXT();
    			break;
    		case 6: // DROP
    			POPDS();
    			NEXT();
    			break;
    		case 7: // OVER 
    			regA = POPDS();
    			regB = POPDS();
    			PUSHDS(regB);
    			PUSHDS(regA);
    			PUSHDS(regB);
    			NEXT();
    			break;
    		case 8: // SWAP
    			regA = POPDS();
    			regB = POPDS();
    			PUSHDS(regA);
    			PUSHDS(regB);
    			NEXT();
    			break;
    		case 9: // DUP
    			regA = POPDS();
    			PUSHDS(regA);
    			PUSHDS(regA);
    			NEXT();
    			break;
    		case 10: // UM+
    			regA = POPDS();
    			regB = POPDS();
    			regA = (regA + regB);
    			if(regA > 0xFFFF)
    			{
    				PUSHDS(regA & 0xFFFF);
    				PUSHDS(1);
    			}
    			else
    			{
    				PUSHDS(regA);
        			PUSHDS(0);
    			}
    			NEXT();
    			break;
    		case 11: // NAND
    			regA = POPDS();
    			regB = POPDS();
    			PUSHDS((~(regA & regB)) & 0xFFFF);
    			NEXT();
    			break;
    		case 12: // 0<
    			regA = POPDS();
    			PUSHDS((regA & 0xFFFF) > 0x8000 ? TRUE : FALSE);
    			NEXT();
    			break;
    		case 13: // >R
    			PUSHRS(POPDS());
    			NEXT();
    			break;
    		case 14: // R>
    			PUSHDS(POPRS());
    			NEXT();
    			break;
    		case 15: // next
    			NEXT();
    			regA = peek(PC);
    			regB = POPRS() - 1;
    			if(regB < 0)
    			{
    				NEXT();
    			}
    			else
    			{
    				PUSHRS(regB);
    				PC = regA;
    			}
    			break;
    		case 16: // BRANCH
    			NEXT();
    			PC = peek(PC);
    			break;
    		case 17: // 0BRANCH
    			regA = POPDS();
    			NEXT();
    			regB = peek(PC);
    			if(regA == FALSE)
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
    			PUSHRS(PC);
    			PC = POPDS();
    			break;
    		case 19: // TX!
    			regA = POPDS();
    			System.out.print((char)regA);
    			NEXT();
    			break;
    		case 20:
    			try {
					regA = System.in.read();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
    			PUSHDS(regA);
    			PUSHDS(TRUE);
    			NEXT();
    			break;
    		default:
    			PUSHDS(peek(PC));
    			NEXT();
    			PUSHRS(PC);
    			PC = POPDS();
    			break;
    		}
    	}
    	return 0;
    }
	public static void main(String[] args) {
		// TODO Auto-generated method stub

		VM vm = new VM();
		FileInputStream f = null;
		try {
			f = new FileInputStream("eforth.img");
			for(int x = 0; x < VM.VM_SIZE;x++)
			{
				int val1 = (f.read() << 8);
				int val2 = (f.read());
				val1 = val1 | val2;
				vm.poke(x, (val1 & 0XFFFF));
			}
			f.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		vm.run(0);
	}

}

