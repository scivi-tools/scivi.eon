/* 
 * EONBlob.cpp
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

#include "EONBlob.h"


using namespace EON;

Blob::Blob()
{
#if defined(ESP32) || defined(ESP8266)
    EEPROM.begin(4096);
#endif // ESP
    m_linkStart = sizeof(Offset);
    m_attrStart = m_linkStart + offset(0) + sizeof(Offset);
    m_blobLen = offset(0) + offset(m_attrStart - sizeof(Offset)) + sizeof(Offset) * 2;
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
    m_blobLen = length;
    m_linkStart = sizeof(Offset);
    m_attrStart = m_linkStart + offset(0) + sizeof(Offset);
    return true;
}

bool Blob::nextLink(Offset &linkIndex, Node &src, Node &dst, Link &lnk) const
{
    if (linkIndex < m_linkStart)
        linkIndex = m_linkStart;
    if (linkIndex >= m_attrStart - sizeof(Offset))
        return false;

    uint8_t lb1 = byte(linkIndex);
    uint8_t lb2 = byte(linkIndex + 1);
    linkIndex += 2;
    src = lb1 >> 2;
    lnk = ((lb1 & 0x3) << 2) | (lb2 >> 6);
    dst = lb2 & 0x3F;

    return true;
}

Value Blob::value(uint8_t selector, Offset &index) const
{
    Value result;
    ++index;
    result.type = selector & 0x3F;
    switch (result.type)
    {
        case Value::UINT8:
        case Value::INT8:
            result.value.ub = byte(index);
            ++index;
            break;

        case Value::UINT16:
        case Value::INT16:
            result.value.us = (((uint16_t)byte(index)) << 8) | (uint16_t)byte(index + 1);
            index += 2;
            break;

        case Value::UINT32:
        case Value::INT32:
        case Value::FLOAT:
            result.value.ui = (((uint32_t)byte(index)) << 24) | (((uint32_t)byte(index + 1)) << 16) |
                              (((uint32_t)byte(index + 2)) << 8) | (uint32_t)byte(index);
            index += 4;
            break;

        case Value::CSTRING:
            result.value.addr = index; // In fact a pointer to the string in memory.
            while (byte(index) != 0x0) // String is null-terminated.
                ++index;
            ++index;
            break;
    }
    return result;
}

Offset Blob::cString(Offset index, char *str) const
{
    Offset len = 0;
    while ((str[len] = byte(index)) != 0x0)
    {
        ++index;
        ++len;
    }
    str[len] = 0x0;
    return len + 1;
}

Offset Blob::cStringLen(Offset index) const
{
    Offset len = 0;
    while (byte(index) != 0x0)
    {
        ++index;
        ++len;
    }
    return len + 1;
}
