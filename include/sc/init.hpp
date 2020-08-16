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

#include "ir.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
namespace sc
{
    using context_t   = llvm::LLVMContext;
    using context_ref = context_t &;
    using context_ptr = context_t *;

    void init( context_ref ctx );

    context_ref context();

    struct with_context
    {
        explicit with_context( module_ref m )
        {
            init( m.getContext() );
        }

        explicit with_context( context_ref ctx )
        {
            init( ctx );
        }
    };

} // namespace sc
