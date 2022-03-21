/* 
 * EONEval.h
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

#ifndef __EONEVAL_H__
#define __EONEVAL_H__

#include "EONOpInstance.h"


#define OP_INSTANCES 256

namespace EON
{
    class Eval
    {
        Heap m_heap;
        Blob m_blob;
        OpInstance *m_opInstances[OP_INSTANCES];

        void fillSettings(OpInstance &opInst, NodeID opInstID);
        void exec(NodeID opInstID);

    public:
        bool load(uint8_t *blob, Offset length)
        {
            return m_blob.load(blob, length);
        };

        void setup();
        void turn();
    };
};

#endif // __EONEVAL_H__
