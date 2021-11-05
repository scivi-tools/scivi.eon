/* 
 * EONBlob.cpp
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

#include "EONBlob.h"


using namespace EON;

Blob::Blob()
{
#if defined(ESP32) || defined(ESP8266)
    EEPROM.begin(4096);
#endif // ESP
    m_length = 0;
    m_dataFlowChunkLen = 0;
    m_settingsChunkAddr = 0;
    m_settingsChunkLen = 0;
    m_keysChunkAddr = 0;
}

bool Blob::load(uint8_t *blob, Offset length)
{
    if (length == 0 || !blob)
        return false;
#ifdef EON_RAM
    memcpy(m_blob, blob, length);
#else
    if (length > EEPROM.length())
        return false;
#if defined(ESP32) || defined(ESP8266)
    for (Offset i = 0; i < length; ++i)
        EEPROM.write(i, blob[i]);
    EEPROM.commit();
#else
    for (Offset i = 0; i < length; ++i)
        EEPROM.update(i, blob[i]);
#endif // ESP
#endif // EON_RAM
    m_length = length;
    m_dataFlowChunkLen = (Offset)byte(0) * 3;
    m_settingsChunkAddr = m_dataFlowChunkLen + 3;
    m_settingsChunkLen = readU16(m_dataFlowChunkLen + 1);
    m_keysChunkAddr = m_settingsChunkAddr + readU16(m_settingsChunkAddr - 2);
    return true;
}

Value Blob::readValue(Offset &index, Heap &heap) const
{
    Type t = (Type)LWR(byte(index));
    ++index;
    Value result;
    switch (t)
    {
        case UINT8:
            result = (uint8_t)byte(index);
            ++index;
            break;

        case UINT16:
            result = ((uint16_t)byte(index) << 8) |
                     (uint16_t)byte(index + 1);
            index += 2;
            break;

        case UINT32:
            result = ((uint32_t)byte(index) << 24) |
                     ((uint32_t)byte(index + 1) << 16) |
                     ((uint32_t)byte(index + 2) << 8) |
                     (uint32_t)byte(index + 3);
            index += 4;
            break;

        case INT8:
            result = (int8_t)byte(index);
            ++index;
            break;

        case INT16:
            result = ((int16_t)byte(index) << 8) |
                     (int16_t)byte(index + 1);
            index += 2;
            break;

        case INT32:
            result = ((int32_t)byte(index) << 24) |
                     ((int32_t)byte(index + 1) << 16) |
                     ((int32_t)byte(index + 2) << 8) |
                     (int32_t)byte(index + 3);
            index += 4;
            break;

        case FLOAT32:
            result = (float)(((uint32_t)byte(index) << 24) |
                             ((uint32_t)byte(index + 1) << 16) |
                             ((uint32_t)byte(index + 2) << 8) |
                             (uint32_t)byte(index + 3));
            index += 4;
            break;

        case STRING:
        {
            Offset n = index;
            while (n < m_length && byte(n) != 0x0)
                ++n;
            if (n < m_length)
            {
                char *str = heap.alloc<char>(n - index + 1);
                for (Offset i = index; i <= n; ++i)
                    str[i - index] = byte(i);
                result = str;
                index = n + 1;
            }
            break;
        }
    }
    return result;
}

void Blob::skipValue(Offset &index) const
{
    Type t = (Type)LWR(byte(index));
    ++index;
    switch (t)
    {
        case UINT8:
        case INT8:
            ++index;
            break;

        case UINT16:
        case INT16:
            index += 2;
            break;

        case UINT32:
        case INT32:
        case FLOAT32:
            index += 4;
            break;

        case STRING:
        {
            while (index < m_length && byte(index) != 0x0)
                ++index;
            break;
        }
    }
}

MotherNodeID Blob::findMotherID(NodeID opInstID) const
{
    Offset ptr = keysChunk();
    while (ptr < m_length)
    {
        MotherNodeID mID = readU16(ptr);
        ptr += 2;
        uint8_t b;
        while (ptr < m_length)
        {
            b = byte(ptr);
            ++ptr;
            if (b == 0x0)
                break;
            else if (b == opInstID)
                return mID;
        }
    }
    return static_cast<MotherNodeID>(-1);
}
