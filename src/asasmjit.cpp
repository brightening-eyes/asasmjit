#include "asasmjit.hpp"
#include <asmjit/asmjit.h>
#include <vector>
#include <memory>
#ifdef _ASMJIT_X86_H
#include "arch_x86.hpp"
#endif
using namespace asmjit;

namespace asasmjit
{
	struct Compiler::priv
	{
		JitRuntime runtime;
CodeHolder code;
FileLogger* log;
	};

	Compiler::Compiler() : m_private(nullptr)
	{
		m_private = new priv();
m_private->code.init(m_private->runtime.getCodeInfo());
m_private->log=new FileLogger(stdout);
m_private->code.setLogger(m_private->log);
	}

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
		compileFunc(byteCode, end, m_private->code);

Error e=m_private->runtime.add(&byteCode, &m_private->code);
if(e)
{
return -1;
}
*output=ptr_cast<asJITFunction>(byteCode);
return 0;
	}

	void Compiler::ReleaseJITFunction(asJITFunction func)
	{
		m_private->runtime.release(reinterpret_cast<void*>(func));
	}

void Compiler::SetErrorHandler(ErrorHandler* e)
{
m_private->code.setErrorHandler(e);
}

	Compiler::~Compiler()
	{
		if (m_private)
{
m_private->code.resetErrorHandler();
m_private->code.reset(true);
if(m_private->log)
{
delete m_private->log;
}
			delete m_private;
}
	}
}
