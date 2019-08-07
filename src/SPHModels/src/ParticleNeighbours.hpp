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

#ifndef PARTICLENEIGHBOURS_H
#define PARTICLENEIGHBOURS_H

#include "SPHModels.hpp"
#include "SearchCubes.hpp"
#include "SPHDatastructures.hpp"


class SPHSTLParticleNeighbours: public SPHModel {

    REGISTER_DEC_TYPE(SPHSTLParticleNeighbours);

private:

    // Coeffs
    float dx_;

    // In
    SPHPointField &pos_;

    SPHField<Facet_handle> & facets_;

    SPHField<searchcubes::SearchCube> & sc_;

    // Out
    SPHField<searchcubes::NeighbourPair> &np_;

    SPHField<STLSurfaceDist> &sd_;

    // Regular data member
    SPHGeneric<searchcubes::SearchCubeDomain> & scd_;

    float search_cube_size_ = 1.0;
public:

    SPHSTLParticleNeighbours(
        const std::string &model_name,
        YAML::Node parameter,
        RunTime & runTime);

    void execute();

    void update_search_cube_domain();

};

#endif