#ifndef _LOADFLUX_H_
#define _LOADFLUX_H_
#include <nuSQuIDS/marray.h>
#include <boost/tokenizer.hpp>

int LoadData(nusquids::marray<double,1>& ENodes,
             nusquids::marray<double,2>& MuFlux,
             nusquids::marray<double,3>& NuFlux,
             std::string FileName);

#endif
