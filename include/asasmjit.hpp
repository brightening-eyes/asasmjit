#pragma once
#include <angelscript.h>
#include <asmjit/asmjit.h>

namespace asasmjit
{
typedef bool (*ErrorHandlerCallback) (asmjit::Error e, const char* msg, asmjit::CodeEmitter* c);
	class Compiler : public asIJITCompiler
	{
	private:
struct priv;
	public:
		Compiler();
		~Compiler();
		int  CompileFunction(asIScriptFunction *function, asJITFunction *output);
		void ReleaseJITFunction(asJITFunction func);
void SetErrorCallback(ErrorHandlerCallback cb);
	private:
		priv* m_private;
	};
}
