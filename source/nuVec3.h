#pragma once

#include <fstream>

namespace Nultima
{

class Vec3
{
public:
    Vec3() : m_x(0), m_y(0), m_z(0) {}
    Vec3(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}
    ~Vec3() {}

    Vec3 operator+ (Vec3);
    Vec3 operator- (Vec3);
    void getFloats(float* array);

    float   length          ();
    float   lengthSquared   ();

    float m_x;
    float m_y;
    float m_z;
};

class Vec3ui
{
public:
    Vec3ui() : m_x(0), m_y(0), m_z(0) {}
    Vec3ui(unsigned int x, unsigned int y, unsigned int z) : m_x(x), m_y(y), m_z(z) {}
    ~Vec3ui() {}

    Vec3ui operator+(Vec3ui);

    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};

class Vec3i
{
public:
    Vec3i() : m_x(0), m_y(0), m_z(0) {}
    Vec3i(int x, int y, int z) : m_x(x), m_y(y), m_z(z) {}
    ~Vec3i() {}

    Vec3i operator+(Vec3i);
    Vec3i operator-(Vec3i);
    bool  operator==(Vec3i);
    bool  operator!=(Vec3i);

    void    serialize       (std::ofstream* stream);
    void    deserialize     (std::ifstream* stream);

    float   length          ();
    float   lengthSquared   ();

    int m_x;
    int m_y;
    int m_z;
};

};
