#include "loadFlux.h"

int LoadData(nusquids::marray<double,1>& e_nodes,
             nusquids::marray<double,2>& muon_flux,
             nusquids::marray<double,3>& neutrino_state,
             std::string FileName)
{
  squids::Const units;
  using namespace boost;

  // Open text file
  std::cout<<"Opening the flux file " << FileName<<std::endl;

  std::string data(FileName.c_str());
  std::ifstream in(data.c_str());
  if (!in.is_open())
    {
      std::cerr<<"CANNOT OPEN THE DATA FILE!! ABORT!!"<<std::endl;
      abort();
    }

  // Parse text file
  typedef tokenizer< char_separator<char> > Tokenizer;

  std::vector< std::string > vec;
  std::string line;
  boost::char_separator<char> sep{" "};

  std::vector<std::vector<std::string> >Fluxes;
  std::vector<std::string>              ENodes;

  int LineID=0;
  while (getline(in,line))
  {
    Tokenizer tok(line,sep);
    vec.assign(tok.begin(),tok.end());
    if(LineID==0)
      ENodes=vec;
    else
    {
      Fluxes.push_back(vec);
    }
    LineID++;
  }

  // Set up for indexing the text file
  enum nus    {nNUMU, nNUMUBAR, nNUTAU, nNUTAUBAR, nNUE, nNUEBAR, nMUPLUS, nMUMINUS, nEND};
  enum parts  {pCONV, pPR, pTOTAL, pPI, pK, pMU, pEND};

  double Emax = 1.0e5; // GeV
  size_t e_size_max = ENodes.size();
  for(size_t j=0; j<ENodes.size(); j++){
    if(stod(ENodes[j]) < Emax) // cut off
      continue;
    else {
      e_size_max = j+1;
      break;
    }
  }

  // Freshen up the arrays
  e_nodes        = nusquids::marray<double,1>({e_size_max});
  neutrino_state = nusquids::marray<double,3>({e_size_max,2,3});
  muon_flux      = nusquids::marray<double,2>({e_size_max,2});

  // Fill the vectors
  for(size_t i=0; i!=e_size_max; ++i)
  {
      e_nodes[i]=stod(ENodes[i])*1.e9; // eV

      // The indexing convention in Bens MCEq files
      // says that the total flux of flavor F is at
      // index F*pEND + p TOTAL.

      muon_flux[i][0]         = stod(Fluxes[nMUMINUS*pEND  + pTOTAL][i]);
      muon_flux[i][1]         = stod(Fluxes[nMUPLUS*pEND   + pTOTAL][i]);
      neutrino_state[i][0][0] = stod(Fluxes[nNUE*pEND      + pTOTAL][i]);
      neutrino_state[i][1][0] = stod(Fluxes[nNUEBAR*pEND   + pTOTAL][i]);
      neutrino_state[i][0][1] = stod(Fluxes[nNUMU*pEND     + pTOTAL][i]);
      neutrino_state[i][1][1] = stod(Fluxes[nNUMUBAR*pEND  + pTOTAL][i]);
      neutrino_state[i][0][2] = stod(Fluxes[nNUTAU*pEND    + pTOTAL][i]);
      neutrino_state[i][1][2] = stod(Fluxes[nNUTAUBAR*pEND + pTOTAL][i]);
  }

  return 0;
}

