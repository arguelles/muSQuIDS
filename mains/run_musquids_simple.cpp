#include <vector>
#include <iostream>
#include <nuSQuIDS/nuSQuIDS.h>
#include <nuSQuIDS/marray.h>
#include <nuSQuIDS/tools.h>
#include "muSQuIDS.h"

using namespace nusquids;
using namespace musquids;

int main()
{
  squids::Const units;

  nusquids::marray<double,1> e_nodes = logspace(1.0e0*units.GeV,1.0e3*units.GeV,200);
  //nusquids::marray<double,1> e_nodes = linspace(1.0e0*units.GeV,1.0e3*units.GeV,100);
  muSQUIDS musq(e_nodes);

  std::shared_ptr<Vacuum> vacuum = std::make_shared<Vacuum>();
  std::shared_ptr<Vacuum::Track> vacuum_track = std::make_shared<Vacuum::Track>(1000.*units.km);
  musq.Set_Body(vacuum);
  musq.Set_Track(vacuum_track);

  // construct muon flux
  marray<double,2> muon_flux({e_nodes.size(),2});
  std::fill(muon_flux.begin(),muon_flux.end(),0);
  for(size_t ie=0; ie<e_nodes.size(); ie++){
    if(e_nodes[ie]>100.0*units.GeV){
      muon_flux[ie][0] = 1.;
      muon_flux[ie][1] = 0.;
    }
  }

  marray<double,3> neutrino_state({e_nodes.size(),2,musq.GetNumNeu()});
  std::fill(neutrino_state.begin(),neutrino_state.end(),0);

  /*
   * fill this with a real neutrino flux later
  for(size_t ie=0; ie<e_nodes.size(); ie++){
    neutrino_state[ie][0] = 1.;
    neutrino_state[ie][1] = 1.;
  }
  */

  musq.Set_initial_state(muon_flux,neutrino_state,flavor);
  //musq.Set_ProgressBar(true);

  musq.EvolveState();

  unsigned int e = 0;
  unsigned int mu = 1;
  unsigned int tau = 2;
  for(size_t ie=0; ie<e_nodes.size(); ie++){
    std::cout << e_nodes[ie]/units.GeV << " " << musq.GetMuonFlux(ie,0) << " " << musq.GetMuonFlux(ie,1) << " ";
    std::cout << musq.EvalFlavorAtNode(e,ie,0) << " " << musq.EvalFlavorAtNode(e,ie,1) << " ";
    std::cout << musq.EvalFlavorAtNode(mu,ie,0) << " " << musq.EvalFlavorAtNode(mu,ie,1) << " ";
    std::cout << musq.EvalFlavorAtNode(tau,ie,0) << " " << musq.EvalFlavorAtNode(tau,ie,1) << " ";
    std::cout << std::endl;
  }

  return 0;
}
