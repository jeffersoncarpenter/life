#ifndef LIFE_VECT2_H
#define LIFE_VECT2_H

#include <cmath>

template<class T>
struct Vect2 {
    T x;
    T y;

    // magnitude squared
    T magnitude2() const {
        const T& x = this->x;
        const T& y = this->y;
        return x * x + y * y;
    }
    // magnitude
    T magnitude() const {
        return std::sqrt(this->magnitude2());
    }

    // cross product: Z component of the cross product of this and
    // another 2d vector
    T cross(const Vect2<T>& op2) const {
        return this->x * op2.y - this->y * op2.x;
    }
    // cross product: 2d projection of the cross product of this and a
    // Z-direction vector of the given length
    Vect2<T> cross(const T& z = 1) const {
        Vect2<T> result = {
            -z * this->y,
            z * this->x,
        };
        return result;
    }
    // dot product
    T dot(const Vect2<T>& op2) const {
        return this->x * op2.x + this->y * op2.y;
    }
    // returns a vector scaled by the given amount
    Vect2<T> scale(const T& factor) const {
        Vect2<T> result = {
            this->x * factor,
            this->y * factor,
        };
        return result;
    }

    Vect2<T> normalize() {
        double mag = this->magnitude();
        return {
            this->x / mag,
            this->y / mag,
        };
    }
    void Normalize() {
        double mag = this->magnitude();
        this->x /= mag;
        this->y /= mag;
    }

    // +, -, *, and / operators, and assignment operators.  * and /
    // take Ts (not vectors).
    Vect2<T> operator+(const Vect2<T>& op2) const {
        Vect2<T> result = {
            this->x + op2.x,
            this->y + op2.y,
        };
        return result;
    }
    Vect2<T>& operator+=(const Vect2<T>& op2) {
        *this = *this + op2;
        return *this;
    }
    Vect2<T> operator-(const Vect2<T>& op2) const {
        Vect2<T> result = {
            this->x - op2.x,
            this->y - op2.y,
        };
        return result;
    }
    Vect2<T>& operator-=(const Vect2<T>& op2) {
        *this = *this - op2;
        return *this;
    }
    Vect2<T> operator*(const T& op2) const {
        return this->scale(op2);
    }
    Vect2<T>& operator*=(const T& op2) {
        this->x *= op2;
        this->y *= op2;
        return *this;
    }
    Vect2<T> operator/(const T& op2) const {
        return this->scale(1 / op2);
    }
    Vect2<T>& operator/=(const T& op2) const {
        this->x /= op2;
        this->y /= op2;
        return *this;
    }
};


// some type aliases
using Vect2I = Vect2<int>;
using Vect2D = Vect2<double>;


// zero vectors
const Vect2I zero2I = {
    0,
    0,
};
const Vect2D zero2D = {
    0.0,
    0.0,
};


// unit vectors
const Vect2I unitXI = {
    1,
    0,
};
const Vect2D unitXD = {
    1.0,
    0.0,
};
const Vect2I unitYI = {
    0,
    1,
};
const Vect2D unitYD = {
    0.0,
    1.0,
};

#endif
