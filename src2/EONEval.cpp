/* 
 * EONEval.cpp
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

#include "EONEval.h"
#include "EONOperator.h"


using namespace EON;

void Eval::fillSettings(OpInstance &opInst, NodeID opInstID)
{
    for (Offset i = m_blob.settingsChunk(), n = m_blob.settingsChunk() + m_blob.settingsChunkLen(); i < n;)
    {
        if (m_blob.byte(i) == opInstID)
        {
            ++i;
            uint8_t st = m_blob.byte(i);
            opInst.setting(UPR(st)) = m_blob.readValue(i, m_heap);
        }
        else
        {
            ++i;
            m_blob.skipValue(i);
        }
    }
}

void Eval::exec(NodeID opInstID)
{
    MotherNodeID mID = m_blob.findMotherID(opInstID);
    MotherNodeID opFunc = 0;
    for (; opFunc < m_operatorsTableLen; ++opFunc)
    {
        if (m_operatorsTable[opFunc] == mID)
            break;
    }
    m_operators[opFunc](*m_opInstances[opInstID]);
    m_opInstances[opInstID]->setCalled();
}

void Eval::turn()
{
    m_heap.clear();
    memset(m_opInstances, 0, OP_INSTANCES * sizeof(OpInstance *));

    for (Offset i = m_blob.dataFlowChunk(), n = m_blob.dataFlowChunkLen(); i < n;)
    {
        NodeID opOutID = m_blob.byte(i);
        ++i;
        uint8_t io = m_blob.byte(i);
        ++i;
        NodeID opInID = m_blob.byte(i);
        ++i;
        if (!m_opInstances[opOutID])
        {
            m_opInstances[opOutID] = m_heap.alloc<OpInstance>(1);
            m_opInstances[opOutID]->init(&m_blob);
            fillSettings(*m_opInstances[opOutID], opOutID);
        }
        if (!m_opInstances[opInID])
        {
            m_opInstances[opInID] = m_heap.alloc<OpInstance>(1);
            m_opInstances[opInID]->init(&m_blob);
            fillSettings(*m_opInstances[opInID], opInID);
        }
        if (!m_opInstances[opOutID]->output(UPR(io)).isValid())
            exec(opOutID);
        m_opInstances[opInID]->input(LWR(io)) = m_opInstances[opOutID]->output(UPR(io));
    }
    for (Offset i = m_blob.dataFlowChunk(), n = m_blob.dataFlowChunkLen(); i < n;)
    {
        ++i;
        uint8_t io = m_blob.byte(i);
        ++i;
        NodeID opInID = m_blob.byte(i);
        ++i;
        if (!m_opInstances[opInID]->isCalled())
            exec(opInID);
    }
}
