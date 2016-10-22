#include "arch_x86.hpp"
#include <iostream>
#include <angelscript.h>
using namespace asasmjit;
using namespace asmjit;

int asasmjit::compileFunc(asDWORD * bc, asDWORD * end, CodeHolder& code)
{
	X86Assembler a(&code);

	Label suspend = a.newLabel();
	Label start = a.newLabel();
	//Function prologue
	X86Gp arg0, arg1,unmut1, unmut2, unmut3, unmut4,sp;
	#if ASMJIT_OS_LINUX
	arg0 = a.zdi();
	arg1 = a.zsi();
	#elif ASMJIT_OS_WINDOWS
	arg0 = a.zcx();
	arg1 = a.zdx();
	#endif
	arg0 = a.zax();
	arg1 = a.zcx();
	sp = a.zsp();
	unmut1 = a.zsi();
	unmut2 = a.zdi();
	unmut3 = a.zbx();
	unmut4 = a.zbp();

	const int pushSize = sizeof(void*);
	const int prologSize = 4 * pushSize;

	//Push immutable registers
	a.push(unmut1);
	a.push(unmut2);
	a.push(unmut3);
	a.push(unmut4);

	#if ASMJIT_ARCH_X86
	//the first 4 bytes on the stack are the return adress
	a.mov(arg0, x86::ptr(sp, 4 + prologSize));
	a.mov(arg1, x86::ptr(sp, 8 + prologSize));
	#endif

	//Jump to arg1
	a.lea(arg1, x86::ptr(start, arg1,0));
a.jmp(start);
	a.bind(start);
	//start point
	auto firstEntry = a.getOffset();
	//load vm registers into real registers(under development)
a.mov(a.zsi(), x86::ptr(a.zbp(), offsetof(asSVMRegisters, stackPointer)));
a.mov(a.zdi(), x86::ptr(a.zbp(), offsetof(asSVMRegisters, valueRegister)));
a.mov(a.zdx(), x86::ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)));

	//SUSPEND:
	a.bind(suspend);
//suspend:
a.pause();
	while (bc < end)
	{
		// Determine the instruction
		asEBCInstr op = asEBCInstr(*(asBYTE*)bc);
		switch (op)
		{
		case asBC_SUSPEND:
			a.jmp(suspend);
			break;
		case asBC_JitEntry:
					asBC_PTRARG(bc)= static_cast<asPWORD>(a.getOffset()- firstEntry);
			break;
case asBC_NEGi:
a.neg(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_NEGf:
a.neg(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_NEGd:
a.neg(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_NEGi64:
a.neg(x86::ptr(a.zdx(), -asBC_QWORDARG(bc)));
break;
case asBC_NOT:
a.not_(x86::ptr(a.zdx(), -asBC_PTRARG(bc)));
break;
case asBC_ADDi:
a.mov(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
a.add(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG1(bc)));
break;
case asBC_ADDf:
a.mov(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
a.add(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG1(bc)));
break;
case asBC_ADDd:
a.mov(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
a.add(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG1(bc)));
break;
case asBC_ADDi64:
a.mov(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
a.add(a.zax(), x86::ptr(a.zdx(), -asBC_SWORDARG1(bc)));
break;
case asBC_INCi8:
a.inc(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_INCi16:
a.inc(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_INCi:
a.inc(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_INCi64:
a.inc(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_INCf:
a.inc(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_INCd:
a.inc(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_DECi8:
a.dec(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_DECi16:
a.dec(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_DECi:
a.dec(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_DECi64:
a.dec(x86::ptr(a.zdx(), -asBC_INTARG(bc)));
break;
case asBC_DECf:
a.dec(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_DECd:
a.dec(x86::ptr(a.zdx(), -asBC_FLOATARG(bc)));
break;
case asBC_IncVi:
a.inc(x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
break;
case asBC_DecVi:
a.dec(x86::ptr(a.zdx(), -asBC_SWORDARG0(bc)));
break;

		default:
			break;
		}

		// Move to next instruction
		bc += asBCTypeSize[asBCInfo[op].type];
	}
	//save registers back into vm(under development)

a.mov(x86::ptr(a.zbp(), offsetof(asSVMRegisters, stackPointer)), a.zsi());
a.mov(x86::ptr(a.zbp(), offsetof(asSVMRegisters, valueRegister)), a.zdi());
a.mov(x86::ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)), a.zdx());

	//Function epilogue
	a.pop(unmut4);
	a.pop(unmut3);
	a.pop(unmut2);
	a.pop(unmut1);
	a.ret();

	return 0;
}
