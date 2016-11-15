#pragma once
#include <angelscript.h>
#include <memory>
#include "asmjit/asmjit.h"

namespace asasmjit
{
int compileFunc(asDWORD* bc, asDWORD* end,asmjit::CodeHolder& code);
}
