#include "arch_x86.hpp"
#include <iostream>
using namespace asasmjit;
using namespace asmjit;

extern "C" void alive()
{
	std::cout << "alive" << std::endl;
}

int asasmjit::compileFunc(asDWORD * bc, asDWORD * end, std::shared_ptr<Assembler>& assembler, JitRuntime & runtime)
{
	Error err;

	if (assembler == nullptr)
{
CodeHolder code;
code.init(runtime.getCodeInfo());
		assembler = std::make_shared<X86Assembler>(&code);
}

	err = assembler->getLastError();
assembler->resetLastError();

	auto a = std::dynamic_pointer_cast<X86Assembler>(assembler);
	Label suspend = a->newLabel();
	Label start = a->newLabel();
	//Function prologue
	X86Gp arg0, arg1,unmut1, unmut2, unmut3, unmut4,sp;
	#if ASMJIT_ARCH_X64
	#if ASMJIT_OS_LINUX
	arg0 = x86::rdi;
	arg1 = x86::rsi;
	#elif ASMJIT_OS_WINDOWS
	arg0 = x86::rcx;
	arg1 = x86::rdx;
	#endif
	unmut1 = x86::rsi;
	unmut2 = x86::rdi;
	unmut3 = x86::rbx;
	unmut4 = x86::rbp;
	#elif ASMJIT_ARCH_X86
	arg0 = x86::eax;
	arg1 = x86::ecx;
	sp = x86::esp;
	unmut1 = x86::esi;
	unmut2 = x86::edi;
	unmut3 = x86::ebx;
	unmut4 = x86::ebp;
	#endif

	const int pushSize = sizeof(void*);
	const int prologSize = 4 * pushSize;

	//Push immutable registers
	a->push(unmut1);
	a->push(unmut2);
	a->push(unmut3);
	a->push(unmut4);

	#if ASMJIT_ARCH_X86
	//the first 4 bytes on the stack are the return adress
	a->mov(arg0, x86::ptr(sp, 4 + prologSize));
	a->mov(arg1, x86::ptr(sp, 8 + prologSize));
	#endif

	//TODO: load vm registers into real registers;
	//Jump to arg1
	a->lea(arg1, x86::ptr(start, arg1,0));
	a->jmp(arg1);
	a->bind(start);
	//start point
	auto firstEntry = a->getOffset();

	//SUSPEND:
	//TODO: save registers back into vm
	a->bind(suspend);
	//Function epilogue
	a->pop(unmut4);
	a->pop(unmut3);
	a->pop(unmut2);
	a->pop(unmut1);
	a->ret();

	while (bc < end)
	{
		// Determine the instruction
		asEBCInstr op = asEBCInstr(*(asBYTE*)bc);
		switch (op)
		{
		case asBC_SUSPEND:
			a->jmp(suspend);
			break;
		case asBC_JitEntry:
			{
			asBC_PTRARG(bc)= static_cast<asPWORD>(a->getOffset()- firstEntry);
			}
			break;

		default:
			break;
		}

		// Move to next instruction
		bc += asBCTypeSize[asBCInfo[op].type];
	}

	return 0;
}
