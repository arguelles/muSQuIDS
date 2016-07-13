#ifndef _MU_NSQ_
#define _MU_NSQ_

#include <vector>
#include <iostream>
#include <nuSQuIDS/nuSQuIDS.h>

using namespace nusquids;

/// \brief This class implements muon energy loss into nuSQuIDS.
class muSQUIDS: public nuSQUIDS {
  protected:
    // These scalar functions will manage the muon decay and energy loss
    double GammaScalar(unsigned int ei,unsigned int index_scalar) const{
      return nuSQUIDS::GammaScalar(ei,index_scalar) + decay_terms;
    }
    double InteractionsScalar(unsigned int ei,unsigned int index_scalar) const{
      return nuSQUIDS::InteractionsScalar(ei,index_scalar) + energy_loss_terms;
    }
    // This rho function will add the neutrinos from muon decay
    squids::SU_vector InteractionsRho(unsigned int ei,unsigned int index_rho) const{
      return nuSQUIDS::InteractionsRho(ei,index_rho) + decay_terms;
    }
  public:
    muSQUIDS(){}
    muSQUIDS(marray<double,1> Emuon_range, marray<double,1> Enu_range,
             int numneu, NeutrinoType NT,bool iinteraction):
      nuSQUIDS(Enu_range,numneu,NT,iinteraction)
    {
    }
};

#endif
