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
class err: public ErrorHandler
{
ErrorHandlerCallback cb;
public:
bool handleError(Error err, const char* message, CodeEmitter* code)
{
return cb(err, message, code);
}
void SetCallback(ErrorHandlerCallback c)
{
cb=c;
}
}err;
		std::shared_ptr<Assembler> assembler;
	};

	Compiler::Compiler() : m_private(nullptr)
	{
		m_private = new priv();
m_private->code.init(m_private->runtime.getCodeInfo());
m_private->code.setErrorHandler(&m_private->err);
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

Error e=m_private->runtime.add(&byteCode, &m_private->code);
*output=ptr_cast<asJITFunction>(byteCode);
return e;
	}

	void Compiler::ReleaseJITFunction(asJITFunction func)
	{
		m_private->runtime.release(reinterpret_cast<void*>(func));
	}

void Compiler::SetErrorCallback(ErrorHandlerCallback cb)
{
m_private->err.SetCallback(cb);
}

	Compiler::~Compiler()
	{
		if (m_private)
{
m_private->code.resetErrorHandler();
m_private->code.reset(true);
			delete m_private;
}
	}
}
