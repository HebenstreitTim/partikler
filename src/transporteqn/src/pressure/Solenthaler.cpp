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

#include "Solenthaler.hpp"

#include "Time.hpp"

SolenthalerGradient::SolenthalerGradient(
    const std::string &model_name, YAML::Node parameter, ObjectRegistry &objReg)

    : ScalarGradientEquation(
          "PressureGradient",
          parameter,
          objReg,
          objReg.get_object<ScalarField>("p")),
      conti_(objReg.get_object<ScalarFieldEquation>("Conti")),
      pressure_(objReg.get_object<ScalarFieldEquation>("Pressure")),
      rho_0_(read_or_default_coeff<Scalar>("rho_0", 1.0)),
      eta_(read_or_default_coeff<Scalar>("eta", 1.0)),
      min_iter_(read_or_default_coeff<int>("minIter", 1)),
      mp_(objReg.get_object<Generic<Scalar>>("specific_particle_mass")()) {}

void SolenthalerGradient::execute() {

    // Check convergence criteria
    VectorFieldEquation &vel_(
        objReg_.get_object<VectorFieldEquation>("Momentum"));
    VectorFieldEquation &pos_(
        objReg_.get_object<VectorFieldEquation>("Position"));

    auto &p = pressure_.get();

    std::fill(p.begin(), p.end(), 0.0);

    vel_.store_old_value();
    pos_.store_old_value();

    for (int iter = 0; iter < min_iter_; iter++) {

        auto &kernelModel = objReg_.get_object<Model>("KernelEqn");

        kernelModel.execute();

        auto &vel = vel_.estimate();

        auto &pos = pos_.estimate();

        auto &rho = conti_.estimate();

        Scalar alpha = 0.0;
        Scalar beta_ = 0.0;
        Scalar rho_diff = 0.0;
        for (size_t it=0; it<rho.size(); it++){
            alpha = 1.0/((Scalar) (it+1));
            beta_ = 1.0 - alpha;
            Scalar diff = rho[it] - rho_0_;
            rho_diff = alpha * diff + beta_ * rho_diff;
        }

        std::cout << "ITERATION " <<  iter << " rho_diff " << rho_diff << std::endl;

        // update pressure

        Scalar dt =  time_.get_deltaT();

        Scalar beta = 2.0 * dt * dt * mp_ * mp_ / (rho_0_ * rho_0_);

        auto &dW = this->get_objReg().template get_object<KernelGradientField>(
            "KerneldWdx");

        // auto &ptilda = objReg_.create_field<ScalarField>(ptildaname, 0);
        // TODO implement a lazy variant
        VectorField &sumABone = objReg_.create_field<VectorField>(
            "sumABone", {0.0, 0.0, 0.0}, {"sABox", "sABoy", "sABoz"});
        ScalarField &sumABtwo = objReg_.create_field<ScalarField>(
            "sumABtwo", 0.0, {"sumABtwo"});

        std::fill(sumABone.begin(), sumABone.end(), Vec3 {0.0,0.0,0.0});
        std::fill(sumABtwo.begin(), sumABtwo.end(), 0.0);

        for (size_t ab = 0; ab < np_.size(); ab++) {
            size_t i = np_[ab].ownId;
            size_t j = np_[ab].neighId;

            Scalar sumABtwores = dW[ab] * dW[ab];

            sumABone[i] += dW[ab];
            sumABone[j] -= dW[ab];

            sumABtwo[i] += sumABtwores;
            sumABtwo[j] += sumABtwores;
        }

        std::string ptildaname = "ptilda" + std::to_string(iter);

        auto &ptilda = objReg_.create_field<ScalarField>(ptildaname, 0);

        std::fill(ptilda.begin(), ptilda.end(), 0);

        IntField &id_(objReg_.get_object<IntField>("id"));

        solve_impl(
            ptilda,
            id_,
            -(rho - rho_0_) / (beta * (-(sumABone * sumABone) - sumABtwo)));

        for (size_t i = 0; i < f_.size(); i++) {
            p[i] += ptilda[i];
        }

        auto sum_AB_e = Sum_AB_dW_sym<VectorField>(f_, np_, dW, dW);
        auto sum_AB_dW_e = boost::yap::make_terminal(sum_AB_e);

        solve(
            mp_ * mp_ * rho *
            sum_AB_dW_e(
                p.a() / (rho.a() * rho.a()) + p.b() / (rho.b() * rho.b())));
    }

    iteration_ = time_.get_current_timestep();
}

REGISTER_DEF_TYPE(PRESSUREGRADIENT, SolenthalerGradient);