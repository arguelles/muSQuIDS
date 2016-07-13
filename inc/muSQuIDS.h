#ifndef _MU_NSQ_
#define _MU_NSQ_

#include <vector>
#include <iostream>
#include <nuSQuIDS/nuSQuIDS.h>

namespace musquids {
using nusquids::nuSQUIDS;
/// \brief This class implements muon energy loss into nuSQuIDS.
class muSQUIDS: public nuSQUIDS {
  private:
    std::vector<double> inv_lambda;
  protected:
    double a(double Emuon) const {
      return 0;
    }
    double b(double Emuon) const {
      return 0;
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
      double E=E_range[ei];
      double muon_energy_loss_terms=(a(E)+b(E)*E)*dScalardE[index_scalar](E);
      return nuSQUIDS::InteractionsScalar(ei,index_scalar) + muon_energy_loss_terms;
    }
    // This rho function will add the neutrinos from muon decay
    squids::SU_vector InteractionsRho(unsigned int ei,unsigned int index_rho) const {
      squids::SU_vector from_muon_decay_terms(nsun);
      double muon_decay_integral = 0.;
      double other_muon_decay_integral = 0.;
      unsigned int other_index_rho = (index_rho == 0) ? 1 : 0;
      for(unsigned int em = ei+ 1; em < ne; en++){ // loop in the tau neutrino energies
        muon_decay_integral += state[ei].scalar[index_scalar]*F(Emuon,Enu)*inv_lambda[ei]*delE[ei]
        other_muon_decay_integral += state[ei].scalar[other_muon_decay_integral]*F(Emuon,Enu)*inv_lambda[ei]*delE[ei];
      }
      from_muon_decay_terms += evol_b1_proj[index_rho][1][ei]*muon_decay_integral;
      from_muon_decay_terms += evol_b1_proj[index_rho][0][ei]*other_muon_decay_integral;
      return nuSQUIDS::InteractionsRho(ei,index_rho) + from_muon_decay_terms;
    }
  public:
    muSQUIDS(){}
    muSQUIDS(marray<double,1> E_range,
             int numneu=3, NeutrinoType NT=both,bool iinteraction=true):
      nuSQUIDS(E_range,numneu,NT,iinteraction)
    {
      inv_lambda.resize(ne);
      for(unsigned int ei=0; ei<ne; ei++)
        inv_lambda[ei] = 1./lambda(E_range[ei]);
    }
};

} // close musquids namespace

#endif
