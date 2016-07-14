#ifndef _MU_NSQ_
#define _MU_NSQ_

#include <vector>
#include <iostream>
#include <nuSQuIDS/nuSQuIDS.h>
#include <gsl/gsl_deriv.h>

namespace musquids {
using nusquids::nuSQUIDS;
using nusquids::marray;
/// \brief This class implements muon energy loss into nuSQuIDS.
class muSQUIDS: public nuSQUIDS {
  private:
    std::vector<double> inv_lambda;
    marray<double,2> dScalardE;
    /// The following variables are to evaluate the derivative of the scalar flux
    std::shared_ptr<gsl_spline> scalar_spline;
    std::shared_ptr<gsl_interp_accel> scalar_spline_acc;
    std::vector<double> tmp_scalar_state;
  protected:
    /// \brief Here we will calculate the muon flux derivative
    void AddToPreDerive(double x){
      for(size_t si=0; si<dScalardE.extent(0); si++){
        for(size_t ei; ei<dScalardE.extent(1); ei++){
          tmp_scalar_state[ei] = state[ei].scalar[si];
        }
        gsl_spline_init(scalar_spline.get(),E_range.get_data(),tmp_scalar_state.data(),ne);
        gsl_interp_accel_reset(scalar_spline_acc.get());
        for(size_t ei; ei<dScalardE.extent(1); ei++){
          dScalardE[si][ei] = gsl_spline_eval_deriv(scalar_spline.get(),E_range[ei],scalar_spline_acc.get());
        }
      }
    }
  protected:
    double EnergyLoss(double Emuon) const{
      // From T. Gaisser Cosmic Ray book
      // From http://pdg.lbl.gov/2015/AtomicNuclearProperties/
      // Good from Emuon>10Gev
      double RadDensH=4.405e5*params.gr/(params.cm*params.cm);
      return (-1.9-0.08*log(Emuon/params.muon_mass))*current_density*params.MeV/params.cm-Emuon/RadDensH;
    }
    double F(double Emuon, double Enu) const {
      return 0;
    }
    double lambda(double Emuon) const {
      return Emuon*params.muon_lifetime/params.muon_mass;
    }
  protected:
    // These scalar functions will manage the muon decay and energy loss
    double GammaScalar(unsigned int ei,unsigned int index_scalar) const {
      double muon_decay_term = -state[ei].scalar[index_scalar]*inv_lambda[ei];
      return nuSQUIDS::GammaScalar(ei,index_scalar) + muon_decay_term;
    }
    double InteractionsScalar(unsigned int ei,unsigned int index_scalar) const {
      double muon_energy_loss_terms=EnergyLoss(E_range[ei])*dScalardE[index_scalar][ei];
      return nuSQUIDS::InteractionsScalar(ei,index_scalar) + muon_energy_loss_terms;
    }
    // This rho function will add the neutrinos from muon decay
    squids::SU_vector InteractionsRho(unsigned int ei,unsigned int index_rho) const {
      squids::SU_vector from_muon_decay_terms(nsun);
      double muon_decay_integral = 0.;
      double other_muon_decay_integral = 0.;
      unsigned int other_index_rho = (index_rho == 0) ? 1 : 0;
      for(unsigned int em = ei+1; em < ne; em++){ // loop in the tau neutrino energies
        muon_decay_integral += state[em].scalar[index_rho]*F(E_range[em],E_range[ei])*inv_lambda[em]*delE[em];
        other_muon_decay_integral += state[em].scalar[other_index_rho]*F(E_range[em],E_range[ei])*inv_lambda[em]*delE[em];
      }
      from_muon_decay_terms += evol_b1_proj[index_rho][1][ei]*muon_decay_integral;
      from_muon_decay_terms += evol_b1_proj[index_rho][0][ei]*other_muon_decay_integral;
      return nuSQUIDS::InteractionsRho(ei,index_rho) + from_muon_decay_terms;
    }
  public:
    muSQUIDS(){}
    muSQUIDS(marray<double,1> E_range,
             int numneu=3, nusquids::NeutrinoType NT=nusquids::both,bool iinteraction=true):
      nuSQUIDS(E_range,numneu,NT,iinteraction),
      scalar_spline(new gsl_spline,[](gsl_spline* t){ gsl_spline_free(t); delete t;}),
      scalar_spline_acc(new gsl_interp_accel,[](gsl_interp_accel* t){ gsl_interp_accel_free(t); delete t;})
    {
      // initializing the muon decay lenght
      inv_lambda.resize(ne);
      for(unsigned int ei=0; ei<ne; ei++)
        inv_lambda[ei] = 1./lambda(E_range[ei]);
      // initializing the scalar derivative matrix
      dScalardE.resize(std::vector<size_t>{nscalars,ne});
      std::fill(dScalardE.begin(),dScalardE.end(),0);
      // initializing the scalar temporary state
      tmp_scalar_state.resize(ne);
      std::fill(tmp_scalar_state.begin(),tmp_scalar_state.end(),0);
      // allocating splines
      scalar_spline.reset(gsl_spline_alloc(gsl_interp_cspline,ne));
      scalar_spline_acc.reset(gsl_interp_accel_alloc());
    }
};

} // close musquids namespace

#endif
