#include "rgba.h"

CRGBA::CRGBA(unsigned char red, unsigned char green, unsigned blue)
{
    Set(red, green, blue, 255);
}

CRGBA::CRGBA(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha)
{
    Set(red, green, blue, alpha);
}

CRGBA::CRGBA(CRGBA const &rhs)
{
    Set(rhs);
}

CRGBA::CRGBA(unsigned int intValue)
{
    Set(intValue);
}

CRGBA::CRGBA(){}

void CRGBA::Set(unsigned char red, unsigned char green, unsigned blue)
{
    r = red;
    g = green;
    b = blue;
}

void CRGBA::Set(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha)
{
    Set(red, green, blue);
    a = alpha;
}  

void CRGBA::Set(CRGBA const &rhs)
{
    Set(rhs.r, rhs.g, rhs.b, rhs.a);
}

void CRGBA::Set(CRGBA const &rhs, unsigned char alpha)
{
    Set(rhs.r, rhs.g, rhs.b, alpha);
}

void CRGBA::Set(unsigned int intValue)
{
    r = (intValue >> 24) & 0xFF;
    g = (intValue >> 16) & 0xFF;
    b = (intValue >> 8) & 0xFF;
    a = intValue & 0xFF;
}

CRGBA CRGBA::ToRGB() const
{
    return CRGBA(r, g, b, 255);
}

unsigned int CRGBA::ToInt() const
{
    return a | (b << 8) | (g << 16) | (r << 24);
}

unsigned int CRGBA::ToIntARGB() const
{
    return b | (g << 8) | (r << 16) | (a << 24);
}

void CRGBA::Invert()
{
    Set(255 - r, 255 - g, 255 - b);
}

CRGBA CRGBA::Inverted() const
{
    CRGBA invertedColor = *this;
    invertedColor.Invert();
    return invertedColor;
}

bool CRGBA::operator==(CRGBA const &rhs) const
{

}

CRGBA &CRGBA::operator=(CRGBA const &rhs)
{

}