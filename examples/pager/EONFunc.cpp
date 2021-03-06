
//
// This file is automatically generated by SciVi::ES v0.1
//

#include "EONFunc.h"

#include <ESP8266WiFi.h>


#ifdef EON_DEBUG
#define LOG(...) printf(__VA_ARGS__)
#define MCU(...)
#else
#define LOG(...)
#define MCU(...) __VA_ARGS__
#endif // EON_DEBUG

using namespace EON;

void Func::setup()
{
    LOG("SETUP\n");

    m_wifiOn = false;
}

void Func::begin()
{
    LOG("BEGIN\n");
}

void Func::end()
{
    LOG("END\n");
}

#define F_ADD                0
#define F_SUB                1
#define F_MUL                2
#define F_DIV                3
#define F_GT                 4
#define F_LT                 5
#define F_GE                 6
#define F_LE                 7
#define F_EQ                 8
#define F_dw                 9
#define F_osc                10
#define F_wifiAP             11
#define F_wifiAPClientsCount 12
#define F_adc                13
#define F_tone               14

#define ARG(type) (stack.pop().typedValue<type>())
#define VALUE(v) (Value::floatValue(v))

Value Func::call(uint8_t opCode, Stack &stack, Blob &blob)
{
    switch (opCode)
    {
        case F_ADD:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f + %f\n", y, x);
            return VALUE(y + x);
        }

        case F_SUB:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f - %f\n", y, x);
            return VALUE(y - x);
        }

        case F_MUL:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f * %f\n", y, x);
            return VALUE(y * x);
        }

        case F_DIV:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f / %f\n", y, x);
            return VALUE(y / x);
        }

        case F_GT:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f > %f\n", y, x);
            return VALUE(y > x ? 1 : 0);
        }

        case F_LT:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f < %f\n", y, x);
            return VALUE(y < x ? 1 : 0);
        }

        case F_GE:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f >= %f\n", y, x);
            return VALUE(y >= x ? 1 : 0);
        }

        case F_LE:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f <= %f\n", y, x);
            return VALUE(y <= x ? 1 : 0);
        }

        case F_EQ:
        {
            float x = ARG(float);
            float y = ARG(float);
            LOG("%f == %f\n", y, x);
            return VALUE(y == x ? 1 : 0);
        }

        case F_dw:
        {
            int x = ARG(int);
            int pin = ARG(int);
            LOG("digitalWrite %d %d\n", pin, x);
            MCU
            (
                digitalWrite(pin, x ? HIGH : LOW);
            )
            break;
        }

        case F_osc:
        {
            LOG("oscillate\n");
            break;
        }

        case F_wifiAP:
        {
            int passIdx = ARG(int);
            int ssidIdx = ARG(int);
            char pass[256];
            char ssid[256];
            blob.cString(passIdx, pass);
            blob.cString(ssidIdx, ssid);
            LOG("wifiAP <%s> <%s>\n", ssid, pass);
            MCU
            (
                if (!m_wifiOn)
                {
                    WiFi.softAP(ssid, pass);
                    m_wifiOn = true;
                }
            )
            break;
        }

        case F_wifiAPClientsCount:
        {
            uint8_t result = 0;
            LOG("wifiAPClientsCount\n");
            MCU
            (
                result = WiFi.softAPgetStationNum();
                Serial.print("WiFi clients: ");
                Serial.println(result);
            )
            return VALUE(result);
        }

        case F_adc:
        {
            int result = 0;
            LOG("adc\n");
            MCU
            (
                result = analogRead(0);
            )
            return VALUE(result);
        }

        case F_tone:
        {
            int freq = ARG(int);
            int pin = ARG(int);
            LOG("tone %d %d\n", pin, freq);
            pin = mapPin(pin);
            MCU
            (
                Serial.print("Tone: ");
                Serial.println(freq);
                if (freq > 0)
                    tone(pin, freq);
                else
                    noTone(pin);
            )
            break;
        }
    }

    return Value::invalidValue();
}

int Func::mapPin(int pin) const
{
    static int mapping[] = { D0, D1, D2, D3, D4, D5, D6, D7, D8 };
    if (pin < 0)
        pin = 0;
    else if (pin >= sizeof(mapping))
        pin = sizeof(mapping) - 1;
    return mapping[pin];
}
