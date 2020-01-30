/* 
 * EONContainers.h
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

#ifndef __EONCONTAINERS_H__
#define __EONCONTAINERS_H__

#include "EONBlob.h"


#define EON_RINGBUFFER_CAPACITY 128
#define EON_STACK_CAPACITY 64

namespace EON
{
    class Stack
    {
    private:
        uint16_t m_ptr;
        Value m_stack[EON_STACK_CAPACITY];

    public:
        inline void clear()
        {
            m_ptr = 0;
        };

        inline bool push(const Value &val)
        {
            if (m_ptr == EON_STACK_CAPACITY)
                return false; // Overflow.
            m_stack[m_ptr] = val;
            ++m_ptr;
            return true;
        };

        inline Value pop()
        {
            if (m_ptr == 0)
                return Value::invalidValue();
            else
            {
                --m_ptr;
                return m_stack[m_ptr];
            }
        };
    };

    class RingBuffer
    {
    private:
        struct RingBufferEntry
        {
            Node node;
            Value value;
        };

        uint16_t m_ptr;
        RingBufferEntry m_buffer[EON_RINGBUFFER_CAPACITY];

    public:
        inline void clear()
        {
            m_ptr = 0;
            memset(m_buffer, Value::INVALID, sizeof(m_buffer));
        };

        inline void store(Node node, const Value &value)
        {
            m_buffer[m_ptr].node = node;
            m_buffer[m_ptr].value = value;
            m_ptr = (m_ptr + 1) % EON_RINGBUFFER_CAPACITY;
        };

        inline Value retrieve(Node node) const
        {
            for (uint16_t i = 0; i < EON_RINGBUFFER_CAPACITY; ++i)
            {
                if (m_buffer[i].node == node)
                    return m_buffer[i].value;
            }
            return Value::invalidValue();
        };
    };
};

#endif // __EONCONTAINERS_H__
