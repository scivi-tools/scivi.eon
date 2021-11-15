/* 
 * EONBlob.h
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

#ifndef __EONBLOB_H__
#define __EONBLOB_H__

#ifndef EON_RAM
#include <EEPROM.h>
#endif // !EON_RAM
#include "EONValue.h"


#define EON_BLOB_CAPACITY 1024
#define UPR(b) ((b & 0xF0) >> 4)
#define LWR(b) (b & 0x0F)

namespace EON
{
    class Blob
    {
#ifdef EON_RAM
        uint8_t m_blob[EON_BLOB_CAPACITY];
#endif // EON_RAM
        Offset m_length;
        Offset m_dataFlowChunkLen;
        Offset m_settingsChunkAddr;
        Offset m_settingsChunkLen;
        Offset m_keysChunkAddr;

        uint16_t readU16(Offset index) const
        {
            return ((uint16_t)byte(index)) << 8 | (uint16_t)byte(index + 1);
        };

    public:
        Blob();

        bool load(uint8_t *blob, Offset length);

        uint8_t byte(Offset index) const
        {
#ifdef EON_RAM
            return m_blob[index];
#else
            return EEPROM[index];
#endif // EON_RAM
        };

        Offset length() const
        {
            return m_length;
        };
        Offset dataFlowChunkLen() const
        {
            return m_dataFlowChunkLen;
        };
        Offset dataFlowChunk() const
        {
            return 1;
        };
        Offset settingsChunkLen() const
        {
            return m_settingsChunkLen;
        };
        Offset settingsChunk() const
        {
            return m_settingsChunkAddr;
        };
        Offset keysChunk() const
        {
            return m_keysChunkAddr;
        };

        MotherNodeID findMotherID(NodeID opInstID) const;

        Value readValue(Offset &index, Heap &heap) const;
        void skipValue(Offset &index) const;
    };
};

#endif // __EONBLOB_H__
