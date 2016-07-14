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

  nusquids::marray<double,1> e_nodes = logspace(1.0e1*units.GeV,1.0e3*units.GeV,100);
  muSQUIDS musq(e_nodes);

  std::shared_ptr<Vacuum> vacuum = std::make_shared<Vacuum>();
  std::shared_ptr<Vacuum::Track> vacuum_track = std::make_shared<Vacuum::Track>(100.*units.meter);
  musq.Set_Body(vacuum);
  musq.Set_Track(vacuum_track);

  // construct muon flux
  marray<double,2> muon_flux({e_nodes.size(),2});
  for(size_t ie=0; ie<e_nodes.size(); ie++){
    muon_flux[ie][0] = 0.;
    muon_flux[ie][1] = 0.;
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
  musq.Set_ProgressBar(true);

  musq.EvolveState();

  return 0;
}
