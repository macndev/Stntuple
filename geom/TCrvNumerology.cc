//

#include "Stntuple/geom/TCrvNumerology.hh"

// |--------+--------+------------+--------+-----------|
// | sector | name   | N(modules) | bars / | first bar |
// |        |        |            | layer  |     index |
// |--------+--------+------------+--------+-----------|
// |      0 | CRV_R1 |         18 |     16 |         0 |
// |      1 | CRV_R2 |          1 |     16 |      1152 |
// |      2 | CRV_R3 |          1 |     16 |      1216 |
// |      3 | CRV_R4 |          2 |     16 |      1280 |
// |      4 | CRV_R5 |          4 |     16 |      1408 |
// |      5 | CRV_L1 |         15 |     16 |      1664 |
// |      6 | CRV_L2 |          4 |     16 |      2624 |
// |      7 | CRV_T1 |          3 |     16 |      2880 |
// |      8 | CRV_T2 |          2 |     16 |      3072 |
// |      9 | CRV_T3 |          4 |     16 |      3200 |
// |     10 | CRV_T4 |         16 |     16 |      3456 |
// |     11 | CRV_E1 |          1 |     16 |      4480 |
// |     12 | CRV_E2 |          1 |     16 |      4544 |
// |     13 | CRV_U  |          4 |     16 |      4608 |
// |     14 | CRV_D1 |          3 |     16 |      4864 |
// |     15 | CRV_D2 |          1 |     16 |      5056 |
// |     16 | CRV_D3 |          1 |     16 |      5120 |
// |     17 | CRV_D4 |          1 |     16 |      5184 |
// |     18 | CRV_C1 |          1 |     16 |      5248 |
// |     19 | CRV_C2 |          1 |     16 |      5312 |
// |     20 | CRV_C3 |          1 |     22 |      5376 |
// |     21 | CRV_C4 |          1 |     10 |      5464 |
// |--------+--------+------------+--------+-----------|


TCrvNumerology::SectorData_t     sector[] = {
  TCrvNumerology::SectorData_t {  0 , TString("CRV_R1"),  18 ,   4 , 16 ,     0 } ,
  TCrvNumerology::SectorData_t {  1 , TString("CRV_R2"),   1 ,   4 , 16 ,  1152 } ,
  TCrvNumerology::SectorData_t {  2 , TString("CRV_R3"),   1 ,   4 , 16 ,  1216 } ,
  TCrvNumerology::SectorData_t {  3 , TString("CRV_R4"),   2 ,   4 , 16 ,  1280 } ,
  TCrvNumerology::SectorData_t {  4 , TString("CRV_R5"),   4 ,   4 , 16 ,  1408 } ,
  TCrvNumerology::SectorData_t {  5 , TString("CRV_L1"),  15 ,   4 , 16 ,  1664 } ,
  TCrvNumerology::SectorData_t {  6 , TString("CRV_L2"),   4 ,   4 , 16 ,  2624 } ,
  TCrvNumerology::SectorData_t {  7 , TString("CRV_T1"),   3 ,   4 , 16 ,  2880 } ,
  TCrvNumerology::SectorData_t {  8 , TString("CRV_T2"),   2 ,   4 , 16 ,  3072 } ,
  TCrvNumerology::SectorData_t {  9 , TString("CRV_T3"),   4 ,   4 , 16 ,  3200 } ,
  TCrvNumerology::SectorData_t { 10 , TString("CRV_T4"),  16 ,   4 , 16 ,  3456 } ,
  TCrvNumerology::SectorData_t { 11 , TString("CRV_E1"),   1 ,   4 , 16 ,  4480 } ,
  TCrvNumerology::SectorData_t { 12 , TString("CRV_E2"),   1 ,   4 , 16 ,  4544 } ,
  TCrvNumerology::SectorData_t { 13 , TString("CRV_U" ),   4 ,   4 , 16 ,  4608 } ,
  TCrvNumerology::SectorData_t { 14 , TString("CRV_D1"),   3 ,   4 , 16 ,  4864 } ,
  TCrvNumerology::SectorData_t { 15 , TString("CRV_D2"),   1 ,   4 , 16 ,  5056 } ,
  TCrvNumerology::SectorData_t { 16 , TString("CRV_D3"),   1 ,   4 , 16 ,  5120 } ,
  TCrvNumerology::SectorData_t { 17 , TString("CRV_D4"),   1 ,   4 , 16 ,  5184 } ,
  TCrvNumerology::SectorData_t { 18 , TString("CRV_C1"),   1 ,   4 , 16 ,  5248 } ,
  TCrvNumerology::SectorData_t { 19 , TString("CRV_C2"),   1 ,   4 , 16 ,  5312 } ,
  TCrvNumerology::SectorData_t { 20 , TString("CRV_C3"),   1 ,   4 , 22 ,  5376 } ,
  TCrvNumerology::SectorData_t { 21 , TString("CRV_C4"),   1 ,   4 , 10 ,  5464 } 
};


TCrvNumerology*        TCrvNumerology::fgInstance       = 0;

//-----------------------------------------------------------------------------
TCrvNumerology::TCrvNumerology() {
  for (int i=0; i<kNSectors; i++) {
    fSector[i] = sector[i];
  }
}

//_____________________________________________________________________________
TCrvNumerology::~TCrvNumerology() {
}

//_____________________________________________________________________________
TCrvNumerology*  TCrvNumerology::Instance() { 
  static Cleaner cleaner;
  return (fgInstance) ? fgInstance : (fgInstance = new TCrvNumerology());
}

//------------------------------------------------------------------------------
TCrvNumerology::Cleaner::Cleaner() {
}

//------------------------------------------------------------------------------
TCrvNumerology::Cleaner::~Cleaner() {
  if (TCrvNumerology::fgInstance) {
    delete TCrvNumerology::fgInstance;
    TCrvNumerology::fgInstance = 0;
  }
}

//------------------------------------------------------------------------------
int TCrvNumerology::GetBarInfo(int BarIndex, int& Sector, int& Module, int& Layer, int& Bar) {
  Sector = -1;
  Module = -1;
  Layer  = -1;
  Bar    = -1;
  for (int is=0; is<kNSectors; is++) {
    SectorData_t* s = fSector+is;
    int nbars = s->fNModules*s->fNLayers*s->fNBarsPerLayer;
    if ((BarIndex >= s->fFirstIndex) and (BarIndex < s->fFirstIndex+nbars)) {
      // found 
      Sector  = is;
      int loc = BarIndex-s->fFirstIndex;
      int nbm = s->fNBarsPerLayer*s->fNLayers;   // n(bars per module)
      Module  = loc/nbm;
      int l2  = loc - nbm*Module;
      Layer   = l2/s->fNBarsPerLayer;
      Bar     = l2 - s->fNBarsPerLayer*Layer;
      break;
    }
  }
  return Sector;
}

//------------------------------------------------------------------------------
void TCrvNumerology::Clear(Option_t* Opt) {
}

//------------------------------------------------------------------------------
void TCrvNumerology::Print(Option_t* Opt) const {
  printf ("N(sectors): %i\n",kNSectors);
  for (int i=0; i<kNSectors; i++) {
    printf("%3i %-10s %5i\n",i,fSector[i].fName.Data(),fSector[i].fFirstIndex);
  }
}
