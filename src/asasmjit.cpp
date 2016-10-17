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
		std::shared_ptr<Assembler> assembler;
	};

	Compiler::Compiler() : m_private(nullptr)
	{
		m_private = new priv();
m_private->code.init(m_private->runtime.getCodeInfo());
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
		compileFunc(byteCode, end,m_private->assembler,m_private->runtime);

Error e=m_private->runtime.add(&output, &m_private->code);
		return 0;
	}

	void Compiler::ReleaseJITFunction(asJITFunction func)
	{
		m_private->runtime.release(reinterpret_cast<void*>(func));
	}

	Compiler::~Compiler()
	{
		if (m_private)
{
m_private->code.reset(true);
			delete m_private;
}
	}
}
