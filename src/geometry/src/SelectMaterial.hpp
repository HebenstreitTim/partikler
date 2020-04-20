
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

#ifndef PARTIKLER_INITSHAPE_INCLUDED_H
#define PARTIKLER_INITSHAPE_INCLUDED_H

#include "Field.hpp"
#include "Models.hpp" // for ModelRegister (ptr only), REGISTER_DEC_TYPE
#include "ParticleGeneratorBase.hpp"
#include "yaml-cpp/yaml.h"

class SelectMaterial : public Model {

    REGISTER_DEC_TYPE(SelectMaterial);

  private:

    FieldIdMap &fieldIdMap_;

    MaterialMap &materialMap_;

    Material material_;

    std::string name_;

    int fieldId_;

    VectorField& pos_;

    IntField &id_;

    std::string shape_;

    Vec3 start_;

    Vec3 end_;


  public:
    SelectMaterial(
        const std::string &model_name,
        YAML::Node parameter,
        ObjectRegistry &objReg);

    void execute();
};

#endif