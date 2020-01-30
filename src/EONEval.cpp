/* 
 * EONEval.cpp
 *
 * This file is part of SciVi (https://github.com/scivi-tools).
 * Copyright (c) 2019 Konstantin Ryabinin.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "EONEval.h"


using namespace EON;

void Eval::turn()
{
    m_func.begin();

    m_stack.clear();
    m_ringBuf.clear();
    m_linkIndex = 0;

    Offset attrIndex = m_blob.attrStart();
    Offset length = m_blob.blobLength();
    while (attrIndex < length)
    {
        Offset node = m_blob.attrID(attrIndex);
        bool doing = true;
        Value val;
        while (doing && attrIndex < length)
        {
            uint8_t selector = m_blob.selector(attrIndex);
            switch (Blob::selectorType(selector))
            {
                case Blob::Address:
                    m_stack.push(eval(m_blob.address(selector, attrIndex)));
                    break;

                case Blob::ValueType:
                    m_stack.push(m_blob.value(selector, attrIndex));
                    break;

                case Blob::Function:
                    val = m_func.call(m_blob.function(selector, attrIndex), m_stack, m_blob);
                    if (!val.invalid())
                        m_stack.push(val);
                    break;

                case Blob::ID:
                    doing = false;
                    break;
            }
        }
        val = m_stack.pop();
        if (!val.invalid())
            m_ringBuf.store(node, val);
    }

    m_func.end();
}

Value Eval::eval(Node node)
{
    Value result = m_ringBuf.retrieve(node);
    if (result.invalid())
    {
        Node src;
        Node dst;
        Link lnk;
        while (m_blob.nextLink(m_linkIndex, src, dst, lnk))
        {
            if (lnk == UseFor)
            {
                result = eval(src);
                m_ringBuf.store(dst, result);
                if (dst == node)
                    break;
            }
        }
    }
    return result;
}
