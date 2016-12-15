#pragma once
#include <angelscript.h>
#include <memory>
#include "asmjit/asmjit.h"

namespace asasmjit
{
asmjit::Error compileFunc(asDWORD* bc, asDWORD* end, asIScriptFunction* function, asmjit::CodeHolder& code);
}
