/* 
 * EONOpInstance.h
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

#ifndef __EONOPINSTANCE_H__
#define __EONOPINSTANCE_H__

#include "EONBlob.h"


#define OPERATOR_CAPACITY 4

namespace EON
{
    class OpInstance
    {
        Blob *m_blob;
        bool m_called;
        Value m_inputs[OPERATOR_CAPACITY];
        Value m_outputs[OPERATOR_CAPACITY];
        Value m_settings[OPERATOR_CAPACITY];
        Value m_cache[OPERATOR_CAPACITY];

    public:
        void init(Blob *blob)
        {
            m_blob = blob;
            m_called = false;
            for (uint8_t i = 0; i < OPERATOR_CAPACITY; ++i)
            {
                m_inputs[i].clear();
                m_outputs[i].clear();
                m_settings[i].clear();
                m_cache[i].clear();
            }
        };

        bool isCalled() const
        {
            return m_called;
        };
        void setCalled()
        {
            m_called = true;
        };

        Value &input(uint8_t index)
        {
            return m_inputs[index];
        };
        Value &output(uint8_t index)
        {
            return m_outputs[index];
        };
        Value &setting(uint8_t index)
        {
            return m_settings[index];
        };
        Value &cache(uint8_t index)
        {
            return m_cache[index];
        };
    };
};

#endif // __EONOPINSTANCE_H__
