#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <nuSQuIDS/nuSQuIDS.h>
#include <nuSQuIDS/marray.h>
#include <nuSQuIDS/tools.h>
#include "loadFlux.h"
#include "exCross.h"

using namespace nusquids;

int main(int argc, char* argv[])
{
  unsigned const numneu = 3;
  squids::Const units;

  marray<double,1> e_nodes({1});
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

  nuSQUIDS musq(e_nodes,numneu,both,true,
                std::make_shared<nusquids::NeutrinoDISCrossSectionsFromTablesExtended>());

  std::shared_ptr<SunASnu> sunass = std::make_shared<SunASnu>();
  double b = 0.1*sunass->GetRadius();
  std::shared_ptr<SunASnu::Track> sunass_track = std::make_shared<SunASnu::Track>(b);
  musq.Set_Body(sunass);
  musq.Set_Track(sunass_track);

  musq.Set_initial_state(neutrino_state,flavor);

  musq.Set_ProgressBar(true);

  musq.WriteStateHDF5("in.hdf5");
  //return 0;

  musq.EvolveState();

  musq.WriteStateHDF5("out.hdf5");

  return 0;
}
