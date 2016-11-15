#include "arch_x86.hpp"
#include <iostream>
#include <angelscript.h>

using namespace asasmjit;
using namespace asmjit;
using namespace x86;
using namespace std;

int asasmjit::compileFunc(asDWORD * bc, asDWORD * end, CodeHolder& code)
{
Lock l;
AutoLock lock(l);
    X86Assembler a(&code);

    Label suspend = a.newLabel();
    Label start = a.newLabel();
    //parameters (the first is asSVMRegisters and the second is asPWORD), these are for our Jit Function
    X86Gp par1=a.zax();
    X86Gp par2=a.zdx();
    //declaring variables for function prolog and function epilog
    FuncDetail func;
    func.init(FuncSignature2<void, asSVMRegisters*, asPWORD>(CallConv::kIdX86CDecl));
    FuncFrameInfo ffi;
ffi.setAllDirty();
    FuncArgsMapper mapper(&func);
mapper.assign(1, par1);
mapper.assign(2, par2, TypeId::kIntPtr);
    mapper.updateFrameInfo(ffi);
    FuncFrameLayout layout;
    layout.init(func, ffi);
//function prolog
    FuncUtils::emitProlog(&a, layout);
    FuncUtils::allocArgs(&a, layout, mapper);
//jump to start of the function
a.jmp(start);

//vm registers
asSVMRegisters* regs;
//jit argument
asPWORD jitArg;

    //suspend:
    a.bind(suspend);
//pause the execution
    a.pause();
//move the regs into the first parameter
a.mov(par1, ptr(regs));
//set the jit instruction parameter in par2
a.mov(par2, dword_ptr(jitArg));


    //start:
    a.bind(start);
    //start point
    auto firstEntry = a.getOffset(); //assembly offset
    a.finit(); //initialize FPU
    //load par1 into asSVMRegisters
a.mov(ptr(regs), par1);

//execute
    while (bc < end)
    {
        // Determine the instruction
        asEBCInstr op = asEBCInstr(*(asBYTE*)bc);
        switch (op)
        {
case asBC_PopPtr:
a.add(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), AS_PTR_SIZE);
a.inc(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_PshGPtr:
a.sub(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), AS_PTR_SIZE);
a.push(a.zax());
a.mov(a.zax(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.movzx(a.zax(), ptr(a.zax(), 1));
a.cwde();
a.mov(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zax());
a.add(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)), 1+AS_PTR_SIZE);
a.pop(a.zax());
break;
case asBC_PshC4:
a.dec(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.push(a.zax());
a.mov(a.zax(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.mov(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zax());
a.add(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)), 2);
a.pop(a.zax());
break;
case asBC_PshV4:
a.dec(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.push(a.zax());
a.push(a.zdx());
a.mov(a.zax(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.mov(a.zdx(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.movzx(a.zdx(), dword_ptr(a.zdx(), -asBC_SWORDARG0(bc)));
a.sub(dword_ptr(a.zax()), a.zdx());
a.mov(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)), a.zax());
a.pop(a.zdx());
a.pop(a.zax());
a.inc(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
break;
case asBC_PSF:
a.sub(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), AS_PTR_SIZE);
a.push(a.zax());
a.mov(a.zax(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.movzx(a.zax(), -asBC_SWORDARG0(bc));
a.mov(a.zax(), dword_ptr(a.zax()));
a.mov(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zax());
a.inc(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.pop(a.zax());
break;
case asBC_SwapPtr:
a.push(a.zax());
a.push(a.zdx());
a.mov(a.zax(), ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(a.zax(), dword_ptr(a.zax()));
a.mov(a.zdx(), dword_ptr(a.zax(), AS_PTR_SIZE));
a.mov(dword_ptr(a.zdx()), a.zdx());
a.mov(a.zdx(), dword_ptr(a.zax(), AS_PTR_SIZE));
a.mov(a.zax(), a.zdx());
a.mov(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)), a.zax());
a.inc(ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.pop(a.zdx());
a.pop(a.zax());
break;
        case asBC_SUSPEND:
            a.jmp(suspend);
            break;
        case asBC_JitEntry:
jitArg=a.getOffset()-firstEntry;
a.mov(par2, dword_ptr(jitArg));
            break;
        case asBC_NEGi:
            a.neg(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            break;
        case asBC_NEGf:
            a.neg(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            break;
        case asBC_NEGd:
            a.neg(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            break;
        case asBC_NEGi64:
            a.neg(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            break;
        case asBC_NOT:
            a.not_(ptr(ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            break;
        case asBC_ADDi:
            a.mov(a.zax(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)));
            a.add(a.zax(), ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG1(bc)));
            a.mov(ptr(par1, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)+asBC_SWORDARG0(bc)), a.zax());
            break;
        case asBC_ADDf:
            a.mov(a.zax(), asBC_SWORDARG0(bc));
            a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
            a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
            break;
        case asBC_ADDd:
            a.mov(a.zax(), ptr(asBC_SWORDARG0(bc)));
            a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
            a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
            break;
        case asBC_ADDi64:
            a.mov(a.zax(), ptr(asBC_SWORDARG0(bc)));
            a.add(a.zax(), ptr(asBC_SWORDARG1(bc)));
            a.mov(ptr(asBC_SWORDARG0(bc)), a.zax());
            break;
        case asBC_INCi8:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_INCi16:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_INCi:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_INCi64:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_INCf:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_FLOATARG(bc)));
}
            break;
        case asBC_INCd:
{
            a.inc(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_FLOATARG(bc)));
}
            break;
        case asBC_DECi8:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_DECi16:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_DECi:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_DECi64:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_INTARG(bc)));
}
            break;
        case asBC_DECf:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_FLOATARG(bc)));
}
            break;
        case asBC_DECd:
{
            a.dec(ptr(a.zbp(), offsetof(asSVMRegisters, stackFramePointer)+asBC_FLOATARG(bc)));
}
            break;
        case asBC_IncVi:
{
X86Gp tmp=a.zax();
a.mov(tmp, ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
X86Gp stack=a.zdx();
a.mov(stack, ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackPointer)));
a.mov(stack, word_ptr(stack, asBC_SWORDARG0(bc)));
a.sub(stack, tmp);
a.inc(dword_ptr(stack));
a.inc(tmp);
}
            break;
        case asBC_DecVi:
{
X86Gp tmp=a.zax();
a.mov(tmp, ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, programPointer)));
a.movzx(tmp, word_ptr(tmp, -asBC_SWORDARG0(bc)));
X86Gp stack=a.zdx();
a.mov(stack, ptr(regs, ASMJIT_OFFSET_OF(asSVMRegisters, stackFramePointer)));
a.sub(stack, tmp);
a.dec(dword_ptr(stack));
a.inc(tmp);
}
            break;

        default:
            break;
        }

        // Move to next instruction
        bc += asBCTypeSize[asBCInfo[op].type];
    }
//move the regs into the first parameter
a.mov(par1, ptr(regs));
a.nop();
//function epilog
    FuncUtils::emitEpilog(&a, layout);

    return 0;
}
