#pragma once
#include <angelscript.h>

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
	private:
		priv* m_private;
	};
}