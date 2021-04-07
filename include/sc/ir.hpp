/*
 * (c) 2020 Henrich Lauko <xlauko@mail.muni.cz>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <llvm/IR/Module.h>
#include <llvm/Pass.h>

namespace sc
{
    using module_t   = llvm::Module;
    using module_ref = llvm::Module &;
    using module_ptr = llvm::Module *;

    using instruction = llvm::Instruction;

    inline llvm::Function* get_function( llvm::Argument *a )
    {
        return a->getParent();
    }

    inline llvm::Function* get_function( llvm::Instruction *i )
    {
        return i->getParent()->getParent();
    }

    inline llvm::Function* get_function( llvm::Value *v )
    {
        if ( auto arg = llvm::dyn_cast< llvm::Argument >( v ) )
            return get_function( arg );
        return get_function( llvm::cast< llvm::Instruction >( v ) );
    }

    inline llvm::Module* get_module( llvm::Value *v )
    {
        return get_function(v)->getParent();
    }

    inline const llvm::DataLayout& data_layout( llvm::Module *m )
    {
        return m->getDataLayout();
    }

    inline const llvm::DataLayout& data_layout( llvm::Value *v )
    {
        return get_module( v )->getDataLayout();
    }

} // namespace sc
