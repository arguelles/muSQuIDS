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



  return 0;
}
