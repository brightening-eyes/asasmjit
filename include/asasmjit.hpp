#pragma once
#include <angelscript.h>
#include <asmjit/asmjit.h>

namespace asasmjit
{
	class Compiler : public asIJITCompiler
	{
	private:
struct priv;
	public:
		Compiler();
		~Compiler();
		int  CompileFunction(asIScriptFunction *function, asJITFunction *output);
		void ReleaseJITFunction(asJITFunction func);
void SetErrorHandler(asmjit::ErrorHandler*);
	private:
		priv* m_private;
	};
}
