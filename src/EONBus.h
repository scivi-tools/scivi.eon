/* 
 * EONBus.h
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

#ifndef __EONBUS_H__
#define __EONBUS_H__

#include <OneWire.h>


#define MAX_DEVICES_NUMBER 16
#define CMD_PING 0xEE

namespace EON
{
    class Bus
    {
        OneWire *m_1w;
        /**
         * Structure of address:
         *     0        1        2        3        4        5        6        7
         * |--------|--------|--------|--------|--------|--------|--------|--------|
         * |  0xE0  |  0xXX  |  0xXX  |  0xXX  |  0xXX  |  0xXX  |  0xXX  |  0xFF  |
         * |--------|-----------------|-----------------------------------|--------|
         * |  EON   |   ID of mother  |            UID of device          |  CRC   |
         * | device |       node      |                                   |        |
         * | family |                 |                                   |        |
         * |--------|-----------------|-----------------------------------|--------|
         */
        uint8_t m_addresses[MAX_DEVICES_NUMBER][8];

    public:
        /**
         * Bus constructor
         *
         * @param bus - one wire bus.
         */
        Bus(OneWire *bus)
        {
            m_1w = bus;
        };

        /**
         * Scan the bus for devices.
         *
         * @return number of devices found on the bus.
         */
        uint8_t scan()
        {
            m_1w->reset_search();
            uint8_t devCount = 0;
            while (m_1w->search(m_addresses[devCount]) && devCount < 255)
            {
                if (m_1w->crc8(m_addresses[devCount], 7) == m_addresses[devCount][7])
                    ++devCount;
            }
            return devCount;
        };

        /**
         * Get the devices address.
         *
         * @param index - index of device on the bus.
         * @return address of the requested device or null incase of incorrect index.
         */
        const uint8_t *device(uint8_t index) const
        {
            return index < MAX_DEVICES_NUMBER ? m_addresses[index] : nullptr;
        };

        /**
         * Get the mother node ID of the device ontological description.
         *
         * @param index - index of device on the bus.
         * @return mother node ID of the device.
         */
        uint16_t deviceID(uint8_t index) const
        {
            return index < MAX_DEVICES_NUMBER ?
                   (m_addresses[index][1] << 8) | m_addresses[index][2] :
                   (uint16_t)-1;
        };

        /**
         * Get device UID.
         *
         * @param index - index of device on the bus.
         * @return device UID.
         */
        uint32_t deviceUID(uint8_t index) const
        {
            return index < MAX_DEVICES_NUMBER ?
                   (m_addresses[index][3] << 24) | (m_addresses[index][4] << 16) | (m_addresses[index][5] << 8) | m_addresses[index][6] :
                   0;
        }

        /**
         * Send byte to given device.
         *
         * @param address - address of device.
         * @param data - byte to send.
         */
        void send(uint8_t *address, uint8_t data)
        {
            m_1w->reset();
            m_1w->select(address);
            m_1w->write(data);
        };

        /**
         * Send array of bytes to given device.
         *
         * @param address - address of device.
         * @param data - array of bytes to send.
         * @poaram length - length of bytes array.
         */
        void send(uint8_t *address, uint8_t *data, uint16_t length)
        {
            m_1w->reset();
            m_1w->select(address);
            for (uint16_t i = 0; i < length; ++i)
                m_1w->write(data[i]);
        };

        /**
         * Receive byte from the bus.
         *
         * @return byte from the bus.
         */
        uint8_t read()
        {
            return m_1w->read();
        };

        /**
         * Receive array of bytes from the bus.
         *
         * @param data - array to receive.
         * @param length - length of array to receive.
         */
        void read(uint8_t *data, uint16_t length)
        {
            for (uint16_t i = 0; i < length; ++i)
                data[i] = m_1w->read();
        };

        /**
         * Ping device on the bus.
         *
         * @param address - address of device to ping.
         */
        void ping(uint8_t *address)
        {
            send(address, CMD_PING);
        };
    };
}

#endif // __EONBUS_H__
