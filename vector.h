#pragma once

#include <iostream>
#include <cmath>

struct Vector {
    float x;
    float y;

    Vector() : x(0.f), y(0.f) {
    }

    Vector(float a, float b) : x(a), y(b) {
    }

    Vector& operator+=(const Vector& other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vector& operator-=(const Vector& other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vector& operator*=(float num) {
        x *= num;
        y *= num;

        return *this;
    }

    Vector& operator/=(float num) {
        x /= num; 
        y /= num;

        return *this;
    }

    Vector operator-() const {
        Vector output(-x, -y);

        return output;
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vector normalized() const {
        float len = length();
        if (len == 0.f) {
            return Vector(0.f, 0.f);
        }

        return Vector(x / len, y / len);
    }

    float length_square() const {
        return x * x + y * y;
    }
};

inline std::ostream& operator<<(std::ostream& output, const Vector& obj) {
    output << "(" << obj.x << ", " << obj.y << ")";

    return output;
}

inline Vector operator+(const Vector& a, const Vector& b) {
    Vector res(a);
    return res += b;
}

inline Vector operator-(const Vector& a, const Vector& b) {
    Vector res(a);
    return res -= b;
}

inline Vector operator*(const Vector& obj, float num) {
    Vector res(obj);
    return res *= num;
}

inline Vector operator*(float num, const Vector& obj) {
    Vector res(obj);
    return res *= num;
}

inline Vector operator/(const Vector& obj, float num) {
    Vector res(obj);
    return res /= num;
}
