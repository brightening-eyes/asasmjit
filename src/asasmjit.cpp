#include "asasmjit.hpp"
#include <asmjit/asmjit.h>
#include <vector>
#include <memory>
#include "arch_x86.hpp"

using namespace asmjit;
using namespace asasmjit;

Compiler::Compiler()
{
    code.init(runtime.getCodeInfo());
    fp=fopen("debugasm.txt", "wb");
    log=new FileLogger(fp);
    code.setLogger(log);
}

ASMJIT_DEFINE_TYPE_ID(asSVMRegisters, TypeIdOf<asSVMRegisters*>::kTypeId);
ASMJIT_DEFINE_TYPE_ID(asIScriptContext, TypeIdOf<asIScriptContext*>::kTypeId);

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
Error e=    compileFunc(byteCode, end, function, code);
if(e) //errors occured, return from function
{
output=0;
return asERROR;
}
//add our function to the runtime to be able to execute it

e=runtime.add(&output, &code);
if(e)
{
output=0;
return asOUT_OF_MEMORY;
}
    fflush(fp);
//we're done, let's return 0
    return 0;
}

void Compiler::ReleaseJITFunction(asJITFunction func)
{
runtime.release(func);
    fflush(fp);
}

void Compiler::SetErrorHandler(ErrorHandler* e)
{
    code.setErrorHandler(e);
}

asmjit::ErrorHandler* Compiler::GetErrorHandler()
{
return code.getErrorHandler();
}

Compiler::~Compiler()
{
    code.resetErrorHandler();
    code.reset(true);
    if(log)
    {
        delete log;
    }
    if(fp)
    {
        fflush(fp);
        fclose(fp);
    }
}
