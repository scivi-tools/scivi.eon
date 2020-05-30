/* 
 * EONHeap.h
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

#ifndef __EONHEAP_H__
#define __EONHEAP_H__

#include <stdint.h>


#define EON_HEAP_CAPACITY 1024

namespace EON
{
    class Heap
    {
    private:
        uint8_t m_heap[EON_HEAP_CAPACITY];
        uint16_t m_ptr;

    public:
        inline void clear()
        {
            m_ptr = 0;
        };

        inline uint16_t alloc(uint16_t n)
        {
            if (m_ptr + n >= EON_HEAP_CAPACITY)
            {
                m_ptr = n;
                return 0;
            }
            else
            {
                uint16_t result = m_ptr;
                m_ptr += n;
                return result;
            }
        };

        template<class T> inline T *get(uint16_t index)
        {
            return reinterpret_cast<T *>(&m_heap[index]);
        };
    };
};

#endif // __EONHEAP_H__
