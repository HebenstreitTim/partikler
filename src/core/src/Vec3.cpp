
/*  Partikler - A general purpose framework for smoothed particle hydrodynamics
    simulations Copyright (C) 2019 Gregor Olenik

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    contact: go@hpsim.de
*/

#include "Vec3.hpp"

Vec3& Vec3::operator=(const Vec3 & x) {
    operator[](0) = x[0];
    operator[](1) = x[1];
    operator[](2) = x[2];
    return *this;
}

Vec3 operator*(float a, Vec3 x) {
    return {a * x[0], a * x[1], a * x[2]};
}

// scalar division
Vec3 operator/(Vec3 x, float a) {
    float ia = 1. / a;
    return {ia * x[0], ia * x[1], ia * x[2]};
}

// dot product
float operator*(Vec3& x, Vec3& y) {
    return {x[0] * y[0] + x[1] * y[1] + x[2] * y[2]};
}

// addition
Vec3 operator+(Vec3& x, Vec3& y) {
    return {x[0] + y[0],  x[1] + y[1], x[2] + y[2]};
}

Vec3 operator-(Vec3& x, Vec3& y) {
    return {x[0] - y[0],  x[1] - y[1], x[2] - y[2]};
}

std::ostream &operator<<(std::ostream &os, Vec3 const &f) {
    os << "[" << f[0] << ", " << f[1] << ", " << f[2] << "]";
    return os;
}
