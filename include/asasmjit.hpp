#pragma once
#include <angelscript.h>
#include <asmjit/asmjit.h>

namespace asasmjit
{
	class Compiler : public asIJITCompiler
	{
	private:
typedef 		struct priv;
typedef bool (*ErrorHandlerCallback) (asmjit::Error e, const char* msg, asmjit::CodeEmitter* c);
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
