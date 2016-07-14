#ifndef _MU_NSQ_
#define _MU_NSQ_

#include <vector>
#include <iostream>
#include <nuSQuIDS/nuSQuIDS.h>
#include <gsl/gsl_deriv.h>
#include "exCross.h"

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
      double RadDensH=4.405e5; // gr/cm^2
      // current_density in gr/cm^3
      // coeffcients in MeV/cm^2
      return (((-1.9-0.08*log(Emuon/params.muon_mass))*-(Emuon/params.MeV)/RadDensH)*current_density)*(params.MeV/params.cm);
    }
    double Fmue(double Emuon, double Enu) const {
      // Fits to muon decay spectrum including spin
      double z = Enu/Emuon;
      return (1.79779466e-02*pow(z,4)+1.20239959e+01*pow(z,3.)-2.38837016e+01*z*z+1.17861335e+01*z+5.85725324e-02)/Emuon;
    }
    double Fmumu(double Emuon, double Enu) const {
      // Fits to muon decay spectrum including spin
      double z = Enu/Emuon;
      return (-0.24794224*pow(z,4.)+4.51300659*pow(z,3.)-6.2556965*z*z-0.03647084*z+2.02480429)/Emuon;
    }
    double lambda(double Emuon) const {
      return Emuon*params.muon_lifetime/params.muon_mass;
    }
  protected:
    // These scalar functions will manage the muon decay and energy loss
    double GammaScalar(unsigned int ei,unsigned int index_scalar) const {
      double muon_decay_term = inv_lambda[ei];
      return nuSQUIDS::GammaScalar(ei,index_scalar) + muon_decay_term;
    }
    double InteractionsScalar(unsigned int ei,unsigned int index_scalar) const {
      double muon_energy_loss_terms=EnergyLoss(E_range[ei])*dScalardE[index_scalar][ei];
      return nuSQUIDS::InteractionsScalar(ei,index_scalar) + muon_energy_loss_terms;
    }
    // This rho function will add the neutrinos from muon decay
    squids::SU_vector InteractionsRho(unsigned int ei,unsigned int index_rho) const {
      squids::SU_vector from_muon_decay_terms(nsun);
      double muon_decay_to_muon_integral = 0.;
      double muon_decay_to_e_integral = 0.;
      unsigned int other_index_rho = (index_rho == 0) ? 1 : 0;
      for(unsigned int em = ei+1; em < ne; em++){ // loop in the tau neutrino energies
        muon_decay_to_muon_integral += state[em].scalar[index_rho]*Fmumu(E_range[em],E_range[ei])*inv_lambda[em]*delE[em];
        muon_decay_to_e_integral += state[em].scalar[other_index_rho]*Fmue(E_range[em],E_range[ei])*inv_lambda[em]*delE[em];
      }
      from_muon_decay_terms += evol_b1_proj[index_rho][1][ei]*muon_decay_to_muon_integral;
      from_muon_decay_terms += evol_b1_proj[index_rho][0][ei]*muon_decay_to_e_integral;
      return nuSQUIDS::InteractionsRho(ei,index_rho) + from_muon_decay_terms;
    }
  public:
    muSQUIDS(){}
    muSQUIDS(marray<double,1> E_range,
             int numneu=3, nusquids::NeutrinoType NT=nusquids::both,bool iinteraction=true):
      nuSQUIDS(E_range,numneu,NT,iinteraction,std::make_shared<nusquids::NeutrinoDISCrossSectionsFromTablesExtended>()),
      scalar_spline(gsl_spline_alloc(gsl_interp_cspline,E_range.size()),[](gsl_spline* t){ gsl_spline_free(t);}),
      scalar_spline_acc(gsl_interp_accel_alloc(),[](gsl_interp_accel* t){ gsl_interp_accel_free(t);})
    {
      // resetting squids nodes to the right scalar size
      ini(ne,numneu,nrhos,2,Get_t());
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
    }

    void Set_initial_state(const marray<double,2>& muon_flux,const marray<double,3>& neutrino_state,nusquids::Basis basis)
    {
      nuSQUIDS::Set_initial_state(neutrino_state,basis);

      for(unsigned int ie = 0; ie < ne; ie++){
        for(unsigned int ir = 0; ir < nscalars; ir++){
          state[ie].scalar[ir] = muon_flux[ie][ir];
        }
      }
    }

    double GetMuonFlux(unsigned int ie, unsigned int irho){
      return state[ie].scalar[irho];
    }
};

} // close musquids namespace

#endif
