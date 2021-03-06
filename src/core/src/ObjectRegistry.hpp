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

#ifndef SPHOBJECT_REGISTRY_H
#define SPHOBJECT_REGISTRY_H

#include <algorithm>          // for find_if, max
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>           // for operator<<, basic_ostream, endl, char_...
#include <map>
#include <memory> // for shared_ptr, __shared_ptr_access, make_...
#include <stdexcept>
#include <stdio.h> // for size_t
#include <string>  // for string, operator==, operator<<
#include <utility> // for move
#include <vector>  // for vector<>::iterator, vector

#include "Field.hpp"  // for PointField
#include "Logger.hpp" // for Logger
#include "Object.hpp" // for SPHObject, GenericType, SPHObjectType
#include "yaml-cpp/yaml.h" // for Node

class ObjectRegistry {

  private:
    using ObjReg = std::vector<std::shared_ptr<SPHObject>>;

    ObjReg objects_;

    size_t n_particles_ = 0;

    Logger logger_ = {};

  public:
    template <class T> T &register_object(std::shared_ptr<SPHObject> f) {
        std::cout << "Register " << f->get_type_str() << " " << f->get_name()
                  << std::endl;
        int idx = objects_.size();
        objects_.push_back(std::move(f));
        return dynamic_cast<T &>(*objects_[idx]);
    }

    template <class T>
    std::shared_ptr<T> register_object_get_ptr(std::shared_ptr<SPHObject> f) {
        std::cout << "Register " << f->get_type_str() << " " << f->get_name()
                  << std::endl;
        int idx = objects_.size();
        objects_.push_back(std::move(f));
        return std::dynamic_pointer_cast<T>(objects_[idx]);
    }

    // Setter

    void set_n_particles(size_t n_particles) { n_particles_ = n_particles; }

    void update_n_particles() {
        n_particles_ = get_pos().size();
    }

    size_t get_n_particles() {
        update_n_particles();
        return n_particles_;
    }


    template <class T> T &get_object(const std::string name) {
        for (auto &&f : objects_) {
            if (f == nullptr) continue;
            if (f->get_name() == name) {
                return dynamic_cast<T &>(*f);
            };
        }
        std::string error_str = "no object " + name + " found in object registry";
        throw std::runtime_error(error_str);
    }

    // std::unique_ptr<SPHObject> &get_object_ptr(const std::string name) {
    auto get_object_ptr(const std::string name) {
        return std::find_if(
            objects_.begin(), objects_.end(), [&](const auto &val) {
                return val->get_name() == name;
            });
    }

    ObjReg &get_objects() { return objects_; }

    bool object_exists(const std::string name) const {
        for (auto &&f : objects_) {
            if (f->get_name() == name) {
                return true;
            };
        }

        return false;
    }

    PointField &get_points() {
        return get_object<PointField &>("Points");
    }

    VectorField &get_pos() {
        return get_object<VectorField &>("Pos");
    }

    // create an generic with default val
    template <class T> T &create_generic(const std::string name) {
        if (object_exists(name)) return get_object<T>(name);
        return register_object<T>(
            std::make_unique<T>(name, GenericType, typename T::value_type()));
    }

    // create an generic with defined  val
    template <class T>
    T &create_generic(const std::string name, typename T::value_type val) {
        if (object_exists(name)) return get_object<T>(name);
        return register_object<T>(std::make_unique<T>(name, GenericType, val));
    }

    template <class T>
    T &create_field(
        const std::string name,
        typename T::value_type init_value,
        const std::vector<std::string> comp_names) {
        if (object_exists(name)) return get_object<T>(name);
        return register_object<T>(
            std::make_unique<T>(n_particles_, init_value, name, comp_names));
    }

    template <class T>
    T &create_field(const std::string name, typename T::value_type init_value) {
        if (object_exists(name)) return get_object<T>(name);
        return register_object<T>(std::make_unique<T>(
            std::vector<typename T::value_type>(n_particles_, init_value),
            name));
    }

    template <class T> T &create_field(const std::string name) {
        if (object_exists(name)) return get_object<T>(name);
        return register_object<T>(std::make_unique<T>(
            std::vector<typename T::value_type>(n_particles_), name));
    }

    template <class T>
    T &get_or_create_model(
        const std::string model_name,
        YAML::Node parameter,
        ObjectRegistry &objReg) {
        if (object_exists(model_name)) return get_object<T>(model_name);
        return register_object<T>(
            std::make_unique<T>(model_name, parameter, objReg));
    }
};

class FieldIdMap : public SPHObject {

  private:
    std::vector<std::string> fields_;

  public:
    FieldIdMap(const std::string name, const SPHObjectType type)
        : SPHObject(name, type), fields_({}) {};

    // get field id from name string
    int getId(const std::string name) {
        for (size_t i = 0; i < fields_.size(); i++) {
            if (name == fields_[i]) return i;
        }
        std::string error_str = "no field " + name + " found in fieldIdMap";
        throw std::runtime_error(error_str);
    }

    int append(std::string field_name) {
        int id = fields_.size();
        fields_.push_back(field_name);
        return id;
    };
};

#endif
