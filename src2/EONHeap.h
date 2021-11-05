/* 
 * EONHeap.h
 *
 * This file is part of SciVi (https://github.com/scivi-tools).
 * Copyright (c) 2021 Konstantin Ryabinin.
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
#include <string.h>


#define EON_HEAP_CAPACITY 4096

namespace EON
{
    class Heap
    {
        uint8_t m_heap[EON_HEAP_CAPACITY];
        uint16_t m_ptr;

    public:
        Heap()
        {
            clear();
        };

        void clear()
        {
            m_ptr = 0;
        };
        template<class T> T *alloc(uint16_t count = 1)
        {
            T *result = reinterpret_cast<T*>(&m_heap[m_ptr]);
            m_ptr += sizeof(T) * count;
            return result;
        };
    };
};

#endif // __EONHEAP_H__
