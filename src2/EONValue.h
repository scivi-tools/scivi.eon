/* 
 * EONValue.h
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

#ifndef __EONVALUE_H__
#define __EONVALUE_H__

#include "EONHeap.h"


namespace EON
{
    typedef uint16_t Offset;
    typedef uint8_t  NodeID;
    typedef uint16_t MotherNodeID;

    enum Type
    {
        UINT8   = 0,
        UINT16  = 1,
        UINT32  = 2,
        INT8    = 3,
        INT16   = 4,
        INT32   = 5,
        FLOAT32 = 6,
        STRING  = 7
    };
    enum InternalType
    {
        T_UINT,
        T_INT,
        T_FLOAT,
        T_STRING,
        T_INVALID
    };

    class Value
    {
        InternalType m_type;
        union
        {
            uint32_t u;     // type = 0, 1, 2
            int32_t  i;     // type = 3, 4, 5
            float    f;     // type = 6
            void     *addr; // type = 7+
        } m_value;

    public:
        Value()
        {
            clear();
        };

        void clear()
        {
            m_type = T_INVALID;
            m_value.u = 0;
        };

        bool isValid() const
        {
            return m_type != T_INVALID;
        };

        template<typename T> operator T() const
        {
            switch (m_type)
            {
                case T_UINT:
                    return static_cast<T>(m_value.u);

                case T_INT:
                    return static_cast<T>(m_value.i);

                case T_FLOAT:
                    return static_cast<T>(m_value.f);

                default:
                    return static_cast<T>(0);
            }
        };
        template<typename T> operator T*() const
        {
            switch (m_type)
            {
                case T_STRING:
                    return static_cast<T*>(m_value.addr);

                default:
                    return NULL;
            }
        };

        uint32_t operator =(uint32_t val)
        {
            m_type = T_UINT;
            m_value.u = val;
            return val;
        };
        int32_t operator =(int32_t val)
        {
            m_type = T_INT;
            m_value.i = val;
            return val;
        };
        float operator =(float val)
        {
            m_type = T_FLOAT;
            m_value.f = val;
            return val;
        };
        char *operator =(char *val)
        {
            m_type = T_STRING;
            m_value.addr = val;
            return val;
        }
    };
};

#endif // __EONVALUE_H__
