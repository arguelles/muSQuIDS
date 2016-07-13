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

  auto e_nodes = logspace(1.0e1*units.GeV,1.0e3*units.GeV,100);
  //muSQUIDS(e_nodes);

  return 0;
}
