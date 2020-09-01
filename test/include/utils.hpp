#pragma once

#include <llvm/IR/Module.h>
#include <sc/context.hpp>

namespace sc
{
    llvm::Module *empty_module()
    {
        assert( sc::context_ptr() );
        return new llvm::Module( "empty", sc::context() );
    }
} // namespace sc
