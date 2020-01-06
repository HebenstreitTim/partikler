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

#ifndef KERNEL_H
#define KERNEL_H

#include <string> // for string
#include <vector> // for vector

#include "Field.hpp"  // for Field (ptr only), FloatField, PointField
#include "Models.hpp" // for ModelRegister (ptr only), REGISTER_DEC_TYPE
#include "SearchCubes.hpp"
#include "cgal/CGALHelper.hpp"
#include "yaml-cpp/yaml.h"

class ObjectRegistry;
namespace YAML {
class Node;
} // namespace YAML
struct NeighbourPair;
struct Vec3;

class Wendland : public Model {

  private:
    // Coeffs
    const float h_;  // Smoothing length
    const float ih_; // Inverse Smoothing length
    // // 3d
    // const float W_fak2 = 21. / (256. * M_PI * h * h * h);
    // const float dW_fak2 = 21. / (256. * M_PI * h * h * h * h);
    // 2d
    const float W_fak2_;  // = 7. / (64. * M_PI * h * h);
    const float dW_fak2_; // = 7. / (64. * M_PI * h * h * h);

    // In
    const PointField &pos_; // Particle positions

    const Field<std::vector<NeighbourPair>> &np_;
    // const Field<STLSurfaceDist> &sd_;

    // Out
    // Kernel &kernel                               // Kernel field
    FloatField &W_;
    Field<std::vector<Vec3>> &dWdx_;

  public:
    Wendland(
        const std::string &model_name,
        YAML::Node parameter,
        ObjectRegistry &objReg,
        float hfact);

    void execute();
};

class Wendland2D : public Wendland {

    REGISTER_DEC_TYPE(Wendland2D);

  public:
    Wendland2D(
        const std::string &model_name,
        YAML::Node parameter,
        ObjectRegistry &objReg);
};

class Wendland3D : public Wendland {

    REGISTER_DEC_TYPE(Wendland3D);

  public:
    Wendland3D(
        const std::string &model_name,
        YAML::Node parameter,
        ObjectRegistry &objReg);
};

#endif
