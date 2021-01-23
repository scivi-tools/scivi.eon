/* 
 * EONBlob.h
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

#ifndef __EON_H__
#define __EON_H__

#include <string.h>
#ifdef EON_DEBUG
#include <stdio.h>
#endif // EON_DEBUG
#ifndef EON_RAM
#include <EEPROM.h>
#endif // !EON_RAM
#include "EONValue.h"


#define EON_BLOB_CAPACITY 1024
#define EON_BLOB_OFFSET 64

namespace EON
{
    /**
     * The EON::Blob class provides basic functions to handle ontologies in EON format.
     *
     * EON is a binary chunk-based format to store task ontologies. It has the following structure:
     * LinksChunkSize LinksChunk AttrChunkSize AttrChunk
     * where 
     * LinksChunkSize - S bit unsigned integer length (in bytes) of LinksChunk,
     * LinksChunk - chunk describing links,
     * AttrChunkSize - S bit unsigned integer length (in bytes) of AttrChunkSize,
     * AttrsChunk - chunk describing nodes' attributes.
     *
     * Note that relations are called "links" for a sake of bravity (accross entire SciVi code).
     * Also note, that all numbers in EON are Big Endian.
     *
     * The LinksChunk is a sequence of links' descriptions, where each link is represented as the following:
     * SrcID LnkID DstID
     * where
     * SrcID is 6 bit unsigned integer identifier of the source node,
     * LnkID is L bit unsigned integer identifier of the link,
     * DstID is N bit unsigned integer identifier of the destination node.
     *
     * SrcID is always internal identifier (denotes the node inside the task ontology stored in EON),
     * while DstID can denote external node (of the domain ontology stored elsewhere).
     * Let's call the task ontology T and domain ontology D (the domain ontology can be also called "mother" ontology).
     *
     * The following situations are possible
     * ('s' denotes bit of source node ID, 'r' – bit of relation ID, 'd' – bit of destination node ID):
     * 1. Regular relation for the nodes inside T:
     * s s s s s s 0 r r r d d d d d d
     * 2. 'instance_of' relation joining T and D, if D has no more than 256 entities:
     * s s s s s s 1 0 d d d d d d d d
     * 3. 'instance_of' relation joining T and D, if D has more than 256, up to 65536 entities
     * s s s s s s 1 1 d d d d d d d d d d d d d d d d
     *
     * In general the set of links in the situation (1) may vary, but normally,
     * it is constrained to the following (with IDs):
     * 0x0 : use_for
     * 0x1 : has
     * 0x2 : is_a
     * 0x3 : a_part_of
     * 0x4 : base_type
     * 0x5 : language
     * 0x6 : instance_of
     *
     * The AttrsChunk is a sequence of the following elements:
     * NodeID NodeAttr
     * where NodeID is the following (n denotes node ID bit):
     * 1 1 n n n n n n
     * and NodeAttr is an expression in reverse notation to be evaluated.
     *
     * Expression consists of tokens having the following structure:
     * Selector Body
     * where Selector is 2-bit marker of the token's kind
     * and Body is content of token.
     *
     * Following selectors are possible:
     * 0 0 - the token is an address (of other node which value should be substituted instead of this token while evaluated)
     * 0 1 - the token is a value (see below)
     * 1 0 - the token is a function (that should be called right now to evaluete the result on top of the stack of reverse notation calculator)
     * 1 1 - no more tokens, beginning of the next AttrsChunk element
     * 
     * Token's body varies in length and depends on the selector:
     * 1. Address body is 6-bit unsigned integer denoting the ID of the node.
     * 2. Value contains of the following:
     *    Type Val
     *    where Type is 6-bit type identifier (see EONValue.h for available types)
     *    and Val is actual value the has the corresponding length and should be interpreted according to the Type.
     * 3. Function is 6-bit identifier denoting the function in the generated function table (see EONFunc.*).
     */
    class Blob
    {
    public:
        static constexpr const uint8_t Address   = 0x0;
        static constexpr const uint8_t ValueType = 0x40;
        static constexpr const uint8_t Function  = 0x80;
        static constexpr const uint8_t ID        = 0xC0;

    private:
#ifdef EON_RAM
        uint8_t m_blob[EON_BLOB_CAPACITY];
#endif // EON_RAM
        Offset m_blobLen;
        Offset m_linkStart;
        Offset m_attrStart;

        inline Offset offset(Offset index) const;

    public:
        Blob();

        bool load(uint8_t *blob, Offset length);

        inline uint8_t byte(Offset index) const;
        inline Offset blobLength() const { return m_blobLen; };

        bool nextLink(Offset &linkIndex, Node &src, Node &dst, Link &lnk) const;

        inline Offset attrStart() const { return m_attrStart; };
        inline Node attrID(Offset &index) const;
        inline uint8_t selector(Offset index) const;
        inline Node address(uint8_t selector, Offset &index) const;
        Value value(uint8_t selector, Offset &index) const;
        inline uint8_t function(uint8_t selector, Offset &index) const;
        Offset cString(Offset index, char *str) const;
        Offset cStringLen(Offset index) const;

        inline static uint8_t selectorType(uint8_t selector) { return selector & 0xC0; };
        inline static uint8_t selectorValue(uint8_t selector) { return selector & 0x3F; };
    };

    uint8_t Blob::byte(Offset index) const
    {
#ifdef EON_RAM
        return m_blob[index];
#else
        return EEPROM[index + EON_BLOB_OFFSET];
#endif // EON_RAM
    }

    Offset Blob::offset(Offset index) const
    {
        return ((Offset)byte(index)) << 8 | (Offset)byte(index + 1);
    }

    Node Blob::attrID(Offset &index) const
    {
        Node result = selectorValue(byte(index));
        ++index;
        return result;
    }

    uint8_t Blob::selector(Offset index) const
    {
        return byte(index);
    }

    Node Blob::address(uint8_t selector, Offset &index) const
    {
        ++index;
        return selectorValue(selector);
    }

    uint8_t Blob::function(uint8_t selector, Offset &index) const
    {
        ++index;
        return selectorValue(selector);
    }
}

#endif // __EON_H__
