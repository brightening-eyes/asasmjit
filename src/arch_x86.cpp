#include "arch_x86.hpp"
#include <iostream>
#include <angelscript.h>

using namespace asasmjit;
using namespace asmjit;
using namespace x86;
using namespace DebugUtils;
using namespace std;

Error asasmjit::compileFunc(asDWORD * bc, asDWORD * end, asIScriptFunction* function, CodeHolder& code)
{
Lock l;
AutoLock lock(l);
//check if our CodeHolder was initialized
if(!code.isInitialized()) //not initialized
{
return kErrorNotInitialized;
}
    X86Assembler a(&code);
Label label=a.newLabel(); //the start of the function which start's execution
Label epilog=a.newLabel(); //the end of the function when function finishe's it's execution
Label jump=a.newLabel(); //needed for jumps
a.bind(label);
    //parameters (the first is asSVMRegisters and the second is asPWORD), these are for our Jit Function
    X86Gp par1=a.zax();
    X86Gp par2=a.zdx();
    //declaring variables for function prolog and function epilog
    FuncDetail func;
    func.init(FuncSignature2<void, asSVMRegisters*, asPWORD>(CallConv::kIdX86CDecl));
    FuncFrameInfo ffi;
ffi.enablePreservedFP();
ffi.enableCalls();
    FuncArgsMapper mapper(&func);
mapper.assignAll(par1, par2);
    mapper.updateFrameInfo(ffi);
    FuncFrameLayout layout;
    layout.init(func, ffi);
//function prolog
    FuncUtils::emitProlog(&a, layout);
    FuncUtils::allocArgs(&a, layout, mapper);

    auto firstEntry = a.getOffset(); //assembly offset
    a.finit(); //initialize FPU
//execute
    while (bc < end)
    {
        // Determine the instruction
        asEBCInstr op = asEBCInstr(*(asBYTE*)bc);
        switch (op)
        {
case asBC_PopPtr:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.lea(a.zcx(), ptr(a.zbx(), AS_PTR_SIZE));
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zcx());
a.inc(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_PshGPtr:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.lea(a.zcx(), ptr(a.zbx(), -AS_PTR_SIZE));
a.mov(ptr(a.zbx()), a.zcx());
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(ptr(a.zbx()), a.zcx());
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.add(a.zcx(), 1+AS_PTR_SIZE);
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)), a.zcx());
break;
case asBC_PshC4:
a.dec(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(dword_ptr(a.zbx()), a.zcx());
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.add(a.zcx(), 2);
break;
case asBC_PshV4:
a.dec(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.sub(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(a.zbx(), ptr(a.zcx()));
a.mov(a.zbx(), ptr(a.zbx()));
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.inc(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_PSF:
a.sub(dword_ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), -AS_PTR_SIZE);
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.sub(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(ptr(a.zbx()), a.zcx());
a.mov(a.zbx(), ptr(a.zbx()));
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.inc(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_SwapPtr:
break;
case asBC_NOT:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.add(a.zbx(), 2);
a.movzx(a.zbx(), ptr(a.zbx()));
a.cwde();
a.sal(a.zbx(), 2);
a.neg(a.zbx());
a.add(a.zcx(), a.zbx());
a.not_(a.zbx());
a.mov(ptr(a.zcx()), a.zbx());
a.inc(a.zbx());
break;
case asBC_PshG4:
break;
case asBC_LdGRdR4:
break;
case asBC_CALL:
break;
case asBC_RET:
//jump to function epilog
a.jmp(epilog);
break;
case asBC_JMP:
a.jmp(jump);
break;
case asBC_JZ:
a.jz(jump);
break;
case asBC_JNZ:
a.jnz(jump);
break;
case asBC_JS:
a.js(jump);
break;
case asBC_JNS:
a.jns(jump);
break;
case asBC_JP:
a.jp(jump);
break;
case asBC_JNP:
a.jnp(jump);
break;
case asBC_TZ:
break;
case asBC_TNZ:
break;
case asBC_TS:
break;
case asBC_TNS:
break;
case asBC_TP:
break;
case asBC_TNP:
break;
case asBC_NEGi:
break;
case asBC_NEGf:
break;
case asBC_NEGd:
break;
case asBC_INCi16:
break;
case asBC_INCi8:
break;
case asBC_DECi16:
break;
case asBC_DECi8:
break;
case asBC_INCi:
break;
case asBC_DECi:
break;
case asBC_INCf:
break;
case asBC_DECf:
break;
case asBC_INCd:
break;
case asBC_DECd:
break;
        case asBC_IncVi:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.movzx(a.zbx(), ptr(a.zbx(), 2));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.sub(a.zcx(), a.zbx());
a.inc(a.zcx());
a.inc(a.zbx());
            break;
        case asBC_DecVi:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.movzx(a.zbx(), ptr(a.zbx(), 2));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.sub(a.zcx(), a.zbx());
a.dec(a.zcx());
a.inc(a.zbx());
            break;
case asBC_BNOT:
break;
case asBC_BAND:
break;
case asBC_BOR:
break;
case asBC_BXOR:
break;
case asBC_BSLL:
break;
case asBC_BSRL:
break;
case asBC_BSRA:
break;
case asBC_COPY:
break;
case asBC_PshC8:
break;
case asBC_PshVPtr:
break;
case asBC_RDSPtr:
break;
case asBC_CMPd:
break;
case asBC_CMPu:
break;
case asBC_CMPf:
break;
case asBC_CMPi:
break;
case asBC_CMPIi:
break;
case asBC_CMPIf:
break;
case asBC_CMPIu:
break;
case asBC_JMPP:
break;
case asBC_PopRPtr:
break;
case asBC_PshRPtr:
break;
case asBC_STR:
break;
case asBC_CALLSYS:
break;
case asBC_CALLBND:
break;
        case asBC_SUSPEND:
a.nop();
            break;
case asBC_ALLOC:
break;
case asBC_FREE:
break;
case asBC_LOADOBJ:
break;
case asBC_STOREOBJ:
break;
case asBC_GETOBJ:
break;
case asBC_REFCPY:
break;
case asBC_CHKREF:
break;
case asBC_GETOBJREF:
break;
case asBC_GETREF:
break;
case asBC_PshNull:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.lea(a.zcx(), ptr(a.zbx(), -AS_PTR_SIZE));
a.mov(ptr(a.zbx()), a.zcx());
a.mov(dword_ptr(a.zbx()), 0);
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.inc(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_ClrVPtr:
break;
case asBC_OBJTYPE:
break;
case asBC_TYPEID:
a.dec(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(dword_ptr(a.zbx()), a.zcx());
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zbx());
a.add(a.zcx(), 2);
break;
case asBC_SetV4:
break;
case asBC_SetV8:
break;
case asBC_ADDSi:
break;
case asBC_CpyVtoV4:
break;
case asBC_CpyVtoV8:
break;
case asBC_CpyVtoR4:
break;
case asBC_CpyVtoR8:
break;
case asBC_CpyVtoG4:
break;
case asBC_CpyRtoV4:
break;
case asBC_CpyRtoV8:
break;
case asBC_CpyGtoV4:
break;
case asBC_WRTV1:
break;
case asBC_WRTV2:
break;
case asBC_WRTV4:
break;
case asBC_WRTV8:
break;
case asBC_RDR1:
break;
case asBC_RDR2:
break;
case asBC_RDR4:
break;
case asBC_RDR8:
break;
case asBC_LDG:
break;
case asBC_LDV:
break;
case asBC_PGA:
break;
case asBC_CmpPtr:
break;
case asBC_VAR:
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.lea(a.zcx(), ptr(a.zbx(), -AS_PTR_SIZE));
a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zcx());
a.inc(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_iTOf:
break;
case asBC_fTOi:
break;
case asBC_uTOf:
break;
case asBC_fTOu:
break;
case asBC_sbTOi:
break;
case asBC_swTOi:
break;
case asBC_ubTOi:
break;
case asBC_uwTOi:
break;
case asBC_dTOi:
break;
case asBC_dTOu:
break;
case asBC_dTOf:
break;
case asBC_iTOd:
break;
case asBC_uTOd:
break;
case asBC_fTOd:
break;
case asBC_ADDi:
break;
case asBC_SUBi:
break;
case asBC_MULi:
break;
case asBC_DIVi:
break;
case asBC_MODi:
break;
case asBC_ADDf:
break;
case asBC_SUBf:
break;
case asBC_MULf:
break;
case asBC_DIVf:
break;
case asBC_MODf:
break;
case asBC_ADDd:
break;
case asBC_SUBd:
break;
case asBC_MULd:
break;
case asBC_DIVd:
break;
case asBC_MODd:
break;
case asBC_ADDIi:
break;
case asBC_SUBIi:
break;
case asBC_MULIi:
break;
case asBC_ADDIf:
break;
case asBC_SUBIf:
break;
case asBC_MULIf:
break;
case asBC_SetG4:
break;
case asBC_ChkRefS:
break;
case asBC_ChkNullV:
break;
case asBC_CALLINTF:
break;
case asBC_iTOb:
break;
case asBC_iTOw:
break;
case asBC_SetV1:
break;
case asBC_SetV2:
break;
case asBC_Cast:
break;
case asBC_i64TOi:
break;
case asBC_uTOi64:
break;
case asBC_iTOi64:
break;
case asBC_fTOi64:
break;
case asBC_dTOi64:
break;
case asBC_fTOu64:
break;
case asBC_dTOu64:
break;
case asBC_i64TOf:
break;
case asBC_u64TOf:
break;
case asBC_i64TOd:
break;
case asBC_u64TOd:
break;
case asBC_NEGi64:
break;
case asBC_INCi64:
break;
case asBC_DECi64:
break;
case asBC_BNOT64:
break;
case asBC_ADDi64:
break;
case asBC_SUBi64:
break;
case asBC_MULi64:
break;
case asBC_DIVi64:
break;
case asBC_MODi64:
break;
case asBC_BAND64:
break;
case asBC_BOR64:
break;
case asBC_BXOR64:
break;
case asBC_BSLL64:
break;
case asBC_BSRL64:
break;
case asBC_BSRA64:
break;
case asBC_CMPi64:
break;
case asBC_CMPu64:
break;
case asBC_ChkNullS:
break;
case asBC_ClrHi:
break;
        case asBC_JitEntry:
a.mov(dword_ptr(par2), a.getOffset()-firstEntry);
            break;
case asBC_CallPtr:
break;
case asBC_FuncPtr:
break;
case asBC_LoadThisR:
break;
case asBC_PshV8:
break;
case asBC_DIVu:
break;
case asBC_MODu:
break;
case asBC_DIVu64:
break;
case asBC_MODu64:
break;
case asBC_LoadRObjR:
break;
case asBC_LoadVObjR:
break;
case asBC_RefCpyV:
break;
case asBC_JLowZ:
a.je(jump);
break;
case asBC_JLowNZ:
a.jne(jump);
break;
case asBC_AllocMem:
break;
case asBC_SetListSize:
break;
case asBC_PshListElmnt:
break;
case asBC_SetListType:
break;
case asBC_POWi:
break;
case asBC_POWu:
break;
case asBC_POWf:
break;
case asBC_POWd:
break;
case asBC_POWdi:
break;
case asBC_POWi64:
break;
case asBC_POWu64:
break;
case asBC_Thiscall1:
break;
        default:
//if this happened, give an invalid instruction error and return
a.setLastError(kErrorInvalidInstruction, errorAsString(kErrorInvalidInstruction));
            break;
        }
//return if error occured
if(a.getLastError())
{
return a.getLastError();
}
        // Move to next instruction
        bc += asBCTypeSize[asBCInfo[op].type];
    }
//when we want to jump using jmp, jz, etc, we jump here
a.bind(jump);
a.mov(a.zbx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.add(a.zcx(), 4);
a.mov(a.zcx(), ptr(a.zcx()));
a.add(a.zcx(), 2);
a.sal(a.zcx(), 2);
a.add(a.zbx(), a.zcx());
a.mov(a.zcx(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(ptr(a.zcx()), a.zbx());
//function epilog
a.bind(epilog);
a.nop();
//emit the function epilog
    FuncUtils::emitEpilog(&a, layout);
//no error, return from function
    return kErrorOk;
}
