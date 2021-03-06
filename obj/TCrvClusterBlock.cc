//
// 2014-01-27 P.Murat
//

#include "Stntuple/obj/TCrvClusterBlock.hh"
#include "Stntuple/obj/TCrvCoincidenceCluster.hh"
#include "Stntuple/obj/TCrvRecoPulse.hh"

ClassImp(TCrvClusterBlock)

//-----------------------------------------------------------------------------
// R_v is, so far, unused
//-----------------------------------------------------------------------------
  void TCrvClusterBlock::Streamer(TBuffer &R__b) {
  if(R__b.IsReading()) {
    //    Version_t R__v = R__b.ReadVersion();
    R__b.ReadVersion();
    R__b >> fNPulses;
    fListOfPulses->Streamer(R__b);

    R__b >> fNClusters;
    fListOfClusters->Streamer(R__b);
    fClusterPulseLinks->Streamer(R__b);
  }
  else {
    R__b.WriteVersion(TCrvClusterBlock::IsA());
    R__b << fNPulses;
    fListOfPulses->Streamer(R__b);

    R__b << fNClusters;
    fListOfClusters->Streamer(R__b);
    fClusterPulseLinks->Streamer(R__b);
  }
}

//______________________________________________________________________________
TCrvClusterBlock::TCrvClusterBlock() {
  fListOfPulses = new TClonesArray("TCrvRecoPulse",1000);
  fListOfPulses->BypassStreamer(kFALSE);

  fListOfClusters = new TClonesArray("TCrvCoincidenceCluster",10);
  fListOfClusters->BypassStreamer(kFALSE);

  fClusterPulseLinks = new TStnLinkBlock();

  Clear();
}

//______________________________________________________________________________
TCrvClusterBlock::~TCrvClusterBlock() {
  fListOfPulses->Delete();
  delete fListOfPulses;

  fListOfClusters->Delete();
  delete fListOfClusters;

  delete fClusterPulseLinks;
}

//______________________________________________________________________________
void TCrvClusterBlock::Clear(Option_t* opt) {
  fNPulses = 0;
  fListOfPulses->Clear();

  fNClusters = 0;
  fListOfClusters->Clear();
  fClusterPulseLinks->Clear();

  f_EventNumber       = -1;
  f_RunNumber         = -1;
  f_SubrunNumber      = -1;
  fLinksInitialized   =  0;
}

//______________________________________________________________________________
void TCrvClusterBlock::Print(Option_t* opt) const {
  // print all hits in the straw tracker
  printf(" *** N(CRV pulses): %d\n",fNPulses);
  int banner_printed = 0;

  TCrvClusterBlock* crv_block = (TCrvClusterBlock*) this;

  for(int i=0; i<fNPulses; i++) {
    if (! banner_printed) {
      fListOfPulses->At(i)->Print("banner");
      banner_printed = 1;
    }
    fListOfPulses->At(i)->Print("data");
  }

  printf(" *** N(CRV coincidence clusters): %d\n",fNClusters);
  
  banner_printed = 0;
  for(int i=0; i<fNClusters; i++) {
    if (! banner_printed) {
      fListOfClusters->At(i)->Print("banner");
      banner_printed = 1;
    }
    const TCrvCoincidenceCluster* crvcc = crv_block->Cluster(i);
    crvcc->Print("data");
    int np = crvcc->NPulses();
    for (int ip=0; ip<np; ip++) {
      int loc = ClusterPulseIndex(i,ip);
      const TCrvRecoPulse* p =  crv_block->Pulse(loc);
      p->Print("data");
    }
  }
}

