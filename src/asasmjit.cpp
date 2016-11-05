#include "asasmjit.hpp"
#include <asmjit/asmjit.h>
#include <vector>
#include <memory>
#ifdef _ASMJIT_X86_H
#include "arch_x86.hpp"
#endif
using namespace asmjit;

using namespace asasmjit;
	Compiler::Compiler()
	{
code.init(runtime.getCodeInfo());
fp=fopen("debugasm.txt", "wb");
log=new FileLogger(fp);
code.setLogger(log);
	}

ASMJIT_DEFINE_TYPE_ID(asSVMRegisters, TypeIdOf<asSVMRegisters*>::kTypeId);

	int Compiler::CompileFunction(asIScriptFunction *function, asJITFunction *output)
	{
		asUINT   length;
		asDWORD *byteCode = function->GetByteCode(&length);
		if (byteCode == 0 || length == 0)
		{
			output = 0;
			return 1;
		}
		asDWORD *end = byteCode + length;

		//Call the architecture specific compile function
		compileFunc(byteCode, end, code);

Error e=runtime.add(&byteCode, &code);
if(e)
{
return -1;
}
fflush(fp);
*output=ptr_cast<asJITFunction>(byteCode);
return 0;
	}

	void Compiler::ReleaseJITFunction(asJITFunction func)
	{
		runtime.release(reinterpret_cast<void*>(func));
fflush(fp);
	}

void Compiler::SetErrorHandler(ErrorHandler* e)
{
code.setErrorHandler(e);
}

	Compiler::~Compiler()
	{
code.resetErrorHandler();
code.reset(true);
if(log)
{
delete log;
}
if(fp)
{
fflush(fp);
fclose(fp);
}
	}
