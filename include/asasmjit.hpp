#pragma once
#include <angelscript.h>
#include <stdio.h>
#include <asmjit/asmjit.h>

namespace asasmjit
{
	class Compiler : public asIJITCompiler
	{
	private:
	asmjit::JitRuntime runtime;
	asmjit::CodeHolder code;
	asmjit::FileLogger* log;
FILE* fp;
	public:
		Compiler();
		~Compiler();
		int  CompileFunction(asIScriptFunction *function, asJITFunction *output);
		void ReleaseJITFunction(asJITFunction func);
void SetErrorHandler(asmjit::ErrorHandler*);
	};
}
