#include <vector>
#include <iostream>
#include <string>
#include <nuSQuIDS/nuSQuIDS.h>
#include <nuSQuIDS/marray.h>
#include <nuSQuIDS/tools.h>
#include <boost/tokenizer.hpp>
#include "muSQuIDS.h"
#include "loadFlux.h"

using namespace nusquids;
using namespace musquids;

int main(int argc, char* argv[])
{

  squids::Const units;

  nusquids::marray<double,1> e_nodes({1});
  marray<double,2> muon_flux({1,2});
  marray<double,3> neutrino_state({1,2,3});
  
  std::string FileName;
  if (argc>1)
    FileName=std::string(argv[1]);
  else
    {
      std::cout<<"No file name supplied, using standard sample"<<std::endl;
      FileName="data/sample.txt";
    }

  LoadData(e_nodes,muon_flux,neutrino_state,FileName);

  muSQUIDS musq(e_nodes);

  std::shared_ptr<Vacuum> vacuum = std::make_shared<Vacuum>();
  std::shared_ptr<Vacuum::Track> vacuum_track = std::make_shared<Vacuum::Track>(500.*units.meter);
  musq.Set_Body(vacuum);
  musq.Set_Track(vacuum_track);


  musq.Set_initial_state(muon_flux,neutrino_state,flavor);
  musq.Set_ProgressBar(true);

  musq.EvolveState();

  for(size_t ie=0; ie<e_nodes.size(); ie++){
    std::cout << e_nodes[ie]/units.GeV << " " << musq.GetMuonFlux(ie,0) << " " << musq.GetMuonFlux(ie,1) << std::endl;
  }

  return 0;
}
