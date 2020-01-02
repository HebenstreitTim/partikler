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

# include "FixedValue.hpp"

FixedValue::FixedValue(
    const std::string &model_name, YAML::Node parameter, ObjectRegistry &objReg)
    : Model(model_name, parameter, objReg)
{
    for (YAML::Node ps : parameter["boundaries"]) {
        std::string fname = ps["field"].as<std::string>();
        for (auto p : ps["values"]) {
            std::string bname = p["name"].as<std::string>();
            float val = p["value"].as<float>();
            float_fields_[fname][bname] = val;
        }
    }
}

void FixedValue::execute() {

    // TODO move to constructor use create fields methods
    FieldIdMap& fieldIdMap_(get_objReg().get_object<FieldIdMap>("FieldIdMap"));
    IntField& boundary_id_(get_objReg().get_object<IntField>("boundary"));

    for(auto field: float_fields_) {
        for (auto boundary: field.second) {
            int fieldId = fieldIdMap_.getId(boundary.first);

            auto & target = get_objReg().get_object<FloatField>(field.first);
            std::cout << target.get_name() << target.size() << std::endl;
                for(size_t idx=0; idx<target.size(); idx++) {
                    std::cout << idx << " " << boundary_id_[idx] << std::endl;
                    if (boundary_id_[idx] == fieldId) {
                        target[idx] = boundary.second;
                    }
                }
        }
    }


}

REGISTER_DEF_TYPE(BOUNDARY, FixedValue);
