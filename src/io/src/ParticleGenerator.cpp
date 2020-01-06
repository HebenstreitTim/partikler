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

#include "ParticleGenerator.hpp"

SPHSTLReader::SPHSTLReader(
    const std::string &model_name, YAML::Node parameter, ObjectRegistry &objReg)
    : Model(model_name, parameter, objReg),
      fn_(parameter["file"].as<std::string>()) {

    log().info_begin() << "Reading input file: " << fn_;

    std::ifstream *istream = new std::ifstream(fn_);

    // read_STL(istream, points, facets, false);
    Polyhedron_builder_from_STL<HalfedgeDS> *builder =
        new Polyhedron_builder_from_STL<HalfedgeDS>(*istream);

    // objReg.register_object<Generic<Polyhedron_builder_from_STL<HalfedgeDS>>>(
    //     std::make_unique<Generic<Polyhedron_builder_from_STL<HalfedgeDS>>>(
    //         "builder", "generic", *builder
    //         )
    //     );

    log().info_end();

    log().info_begin() << "Constructing polyhedron";

    // TODO make
    // Create input polyhedron
    CGALPolyhedron *polyhedron = new CGALPolyhedron;

    polyhedron->delegate(*builder);

    objReg.register_object<Generic<CGALPolyhedron>>(
        std::make_unique<Generic<CGALPolyhedron>>(
            "polyhedron", GenericType, *polyhedron));

    delete (polyhedron);
    delete (builder);
    delete (istream);

    log().info_end();
}

void SPHSTLReader::execute() { log().info() << "Doing nothing"; }

SPHParticleGenerator::SPHParticleGenerator(
    const std::string &model_name, YAML::Node parameter, ObjectRegistry &objReg)
    : Model(model_name, parameter, objReg), boundary_id_(read_coeff<int>("id")),
      polyhedron_(objReg.get_object<Generic<CGALPolyhedron>>("polyhedron")),
      facets_(objReg.create_field<Field<std::vector<Facet_handle>>>("facets")),
      pos_(objReg.create_field<PointField>("Pos", {}, {"X", "Y", "Z"})),
      idx_(objReg.create_field<SizeTField>("idx")),
      type_(objReg.create_field<IntField>("type")),
      boundary_(objReg.create_field<IntField>("boundary")),
      dx_(parameter["dx"].as<float>()) {}

void SPHParticleGenerator::execute() {

    log().info_begin() << "Generating initial particles ";
    Generate_Points_at_Facets gpf(dx_, pos_, facets_);

    size_t n_0 = pos_.size();

    std::for_each(
        polyhedron_().facets_begin(), polyhedron_().facets_end(), gpf);

    size_t n = pos_.size();

    log().info_end() << "Generated " << n << " Particles";

    get_objReg().set_n_particles(n);

    for (size_t i = 0; i < (n - n_0); i++) {
        idx_.push_back(n_0 + i);
        type_.push_back(2);
        boundary_.push_back(boundary_id_);
    }
}

REGISTER_DEF_TYPE(READER, SPHSTLReader);
REGISTER_DEF_TYPE(GENERATOR, SPHParticleGenerator);
