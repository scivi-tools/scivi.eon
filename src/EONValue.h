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

#include <stdint.h>


namespace EON
{
    // Main types
    typedef uint16_t Offset;
    typedef uint8_t Node;
    typedef uint8_t Link;
    static constexpr const Node InvalidNode = 0xFF;
    static constexpr const Link InvalidLink = 0xFF;
    // Link types
    static constexpr const Link UseFor = 0x0;

    struct Value
    {
        static constexpr const uint8_t UINT8   = 0;
        static constexpr const uint8_t UINT16  = 1;
        static constexpr const uint8_t UINT32  = 2;
        static constexpr const uint8_t INT8    = 3;
        static constexpr const uint8_t INT16   = 4;
        static constexpr const uint8_t INT32   = 5;
        static constexpr const uint8_t FLOAT   = 6;
        static constexpr const uint8_t CSTRING = 7;
        static constexpr const uint8_t INVALID = 0xFF;

        uint8_t type;
        union
        {
            uint8_t  ub;       // type = 0
            uint16_t us;       // type = 1
            uint32_t ui;       // type = 2
            int8_t   b;        // type = 3
            int16_t  s;        // type = 4
            int32_t  i;        // type = 5
            float    f;        // type = 6
            Offset   constStr; // type = 7
        } value;

        inline static Value invalidValue()
        {
            Value result;
            result.type = INVALID;
            return result;
        };
        inline static Value floatValue(float v)
        {
            Value result;
            result.type = FLOAT;
            result.value.f = v;
            return result;
        };

        inline bool invalid() const
        {
            return type == INVALID;
        };
        template<class T> inline T typedValue() const
        {
            switch (type)
            {
                case UINT8:
                    return static_cast<T>(value.ub);

                case UINT16:
                    return static_cast<T>(value.us);

                case UINT32:
                    return static_cast<T>(value.ui);

                case INT8:
                    return static_cast<T>(value.b);

                case INT16:
                    return static_cast<T>(value.s);

                case INT32:
                    return static_cast<T>(value.i);

                case FLOAT:
                    return static_cast<T>(value.f);

                case CSTRING:
                    return static_cast<T>(value.constStr);

                default:
                    return static_cast<T>(0);
            }
        };
    };
};

#endif // __EONVALUE_H__
