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

#include <numeric>
#include <sc/annotation.hpp>

namespace sc
{
    const std::string &annotation::back() const
    {
        return _parts.back();
    }

    std::string annotation::str() const
    {
        auto delim = []( const auto &a, const auto &b ) {
            return a + ( a.length() > 0 ? "." : "" ) + b;
        };
        return std::accumulate( _parts.begin(), _parts.end(), std::string(), delim );
    }

    size_t annotation::size() const
    {
        return _parts.size();
    }

    std::string_view annotation::name() const
    {
        return _parts.back();
    }

    annotation annotation::get_namespace() const
    {
        return { _parts.begin(), std::prev( _parts.end() ) };
    }

    bool annotation::in_namespace( const annotation &ns ) const
    {
        return ns.size() < size()
               && std::equal( ns._parts.begin(), ns._parts.end(), _parts.begin() );
    }

} // namespace sc
