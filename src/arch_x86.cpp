#include "arch_x86.hpp"
#include <iostream>
#include <angelscript.h>

using namespace asasmjit;
using namespace asmjit;
using namespace x86;
using namespace std;

int asasmjit::compileFunc(asDWORD * bc, asDWORD * end, CodeHolder& code)
{
	Lock l;
	AutoLock lock(l);
	X86Assembler a(&code);

	Label suspend = a.newLabel();
	Label start = a.newLabel();
	int stackSize=offsetof(asSVMRegisters, stackPointer);
	//parameters
	X86Gp retval=a.zax();
	X86Gp par1=a.zbx();
	//declaring variables for function prolog and function epilog
	FuncDetail func;
	func.init(FuncSignature2<void, asSVMRegisters*, asUINT>(CallConv::kIdX86StdCall));
	FuncFrameInfo ffi;
	ffi.setAllDirty();
	FuncArgsMapper mapper(&func);
	mapper.assignAll(retval, par1);
	mapper.updateFrameInfo(ffi);
	FuncFrameLayout layout;
	layout.init(func, ffi);
	//function prolog
	FuncUtils::emitProlog(&a, layout);
	FuncUtils::allocArgs(&a, layout, mapper);
	
	a.jmp(start);
	//suspend:
	a.bind(suspend);
	a.pause();
	
	//start:
	a.bind(start);
	//start point
	auto firstEntry = a.getOffset();
	//load vm registers into real registers(under development)
	cout<<bc;
	
	
	while (bc < end)
	{
		// Determine the instruction
		asEBCInstr op = asEBCInstr(*(asBYTE*)bc);
		switch (bc)
		{
			case asBC_SUSPEND:
			a.jmp(suspend);
			break;
			case asBC_JitEntry:
			a.jmp(start);
			asBC_PTRARG(bc)= static_cast<asPWORD>(a.getOffset()- firstEntry);
				break;
			case asBC_NEGi:
			a.neg(ptr(asBC_INTARG(bc)));
			break;
			case asBC_NEGf:
			a.neg(ptr(asBC_FLOATARG(bc)));
			break;
			case asBC_NEGd:
			a.neg(ptr(asBC_FLOATARG(bc)));
			break;
			case asBC_NEGi64:
			a.neg(ptr(asBC_QWORDARG(bc)));
			break;
			case asBC_NOT:
			a.not_(ptr(asBC_PTRARG(bc)));
			break;
			case asBC_ADDi:
			a.mov(a.zax(), ptr(asBC_SWORDARG0(bc)));
			a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
			a.mov( asBC_SWORDARG0(bc), a.zax());
			break;
			case asBC_ADDf:
			a.mov(a.zax(), asBC_SWORDARG0(bc));
			a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
			a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
			break;
			case asBC_ADDd:
			a.mov(a.zax(), ptr(asBC_SWORDARG0(bc)));
			a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
			a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
			break;
			case asBC_ADDi64:
			a.mov(a.zax(), ptr(asBC_SWORDARG0(bc)));
			a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
			a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
			break;
			case asBC_INCi8:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_INCi16:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_INCi:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_INCi64:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_INCf:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_FLOATARG(bc)));
			break;
			case asBC_INCd:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_FLOATARG(bc)));
			break;
			case asBC_DECi8:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_DECi16:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_DECi:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_DECi64:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_INTARG(bc)));
			break;
			case asBC_DECf:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_FLOATARG(bc)));
			break;
			case asBC_DECd:
			a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)-asBC_FLOATARG(bc)));
			break;
			case asBC_IncVi:
			a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackPointer)-asBC_SWORDARG0(bc)));
			break;
			case asBC_DecVi:
cout<<"\n";
cout<<asBC_SWORDARG0(bc);
			a.dec(ptr(asBC_SWORDARG0(bc)));
			break;

		default:
			break;
		}

		// Move to next instruction
		bc += asBCTypeSize[asBCInfo[op].type];
	}
//function epilog
FuncUtils::emitEpilog(&a, layout);

	return 0;
}
