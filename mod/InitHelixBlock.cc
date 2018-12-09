//-----------------------------------------------------------------------------
//  Apr 2016 G. Pezzullo: initialization of the MU2E STNTUPLE Helix block
//
//-----------------------------------------------------------------------------
#include <cstdio>
#include "TROOT.h"
#include "TFolder.h"
#include "TLorentzVector.h"
#include "TVector2.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"


#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStnEvent.hh"

#include "Stntuple/obj/TStnTimeCluster.hh"
#include "Stntuple/obj/TStnTimeClusterBlock.hh"

#include "Stntuple/obj/TStnHelix.hh"
#include "Stntuple/obj/TStnHelixBlock.hh"

#include "Stntuple/obj/TStnTrackSeed.hh"
#include "Stntuple/obj/TStnTrackSeedBlock.hh"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h"

#include "GeometryService/inc/GeometryService.hh"
#include "GeometryService/inc/GeomHandle.hh"

#include "TTrackerGeom/inc/TTracker.hh"
#include "CalorimeterGeom/inc/Calorimeter.hh"

#include "DataProducts/inc/XYZVec.hh"
#include "RecoDataProducts/inc/TimeCluster.hh"
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/HelixHit.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include "RecoDataProducts/inc/AlgorithmIDCollection.hh"
#include "RecoDataProducts/inc/CaloCluster.hh"
#include "RecoDataProducts/inc/StrawHitPositionCollection.hh"

#include "MCDataProducts/inc/PtrStepPointMCVectorCollection.hh"
#include "MCDataProducts/inc/StrawDigiMC.hh"
#include "MCDataProducts/inc/SimParticle.hh"
#include "MCDataProducts/inc/SimParticleCollection.hh"
#include "MCDataProducts/inc/StepPointMC.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "MCDataProducts/inc/StrawDigiMCCollection.hh"
#include "TrkDiag/inc/TrkMCTools.hh"

#include "Mu2eUtilities/inc/LsqSums4.hh"
#include "Math/VectorUtil.h"

using namespace ROOT::Math::VectorUtil;

//-----------------------------------------------------------------------------
// assume that the collection name is set, so we could grab it from the event
//-----------------------------------------------------------------------------
int  StntupleInitMu2eHelixBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode) {

  const mu2e::HelixSeedCollection* list_of_helices(0);
  mu2e::AlgorithmIDCollection*     aid_coll    (0);

  char                 helix_module_label[100], helix_description[100]; 
  char                 makeSD_module_label[100];

  TStnHelixBlock*      cb = (TStnHelixBlock*) Block;
  TStnHelix*           helix(0);


  cb->Clear();
  cb->GetModuleLabel("mu2e::StrawDigiMCCollection",makeSD_module_label);

//-----------------------------------------------------------------------------
// algorithm ID's are determined by the same modules as the helices themselves
//-----------------------------------------------------------------------------
  cb->GetModuleLabel("mu2e::HelixSeedCollection", helix_module_label);
  cb->GetDescription("mu2e::HelixSeedCollection", helix_description );

  art::Handle<mu2e::AlgorithmIDCollection> aidcH;
  art::Handle<mu2e::HelixSeedCollection>   hch;
  const fhicl::ParameterSet*               pset(nullptr);
  std::string                              module_type;

  if (helix_module_label[0] != 0) {
    if (helix_description[0] == 0) {
      Evt->getByLabel(helix_module_label,aidcH);
      Evt->getByLabel(helix_module_label,hch);
    }
    else {
      Evt->getByLabel(helix_module_label,helix_description,aidcH);
      Evt->getByLabel(helix_module_label,helix_description,hch);
    }

    if (aidcH.isValid()) aid_coll = (mu2e::AlgorithmIDCollection*) aidcH.product();
    if (hch.isValid()) { 
      list_of_helices = hch.product();
      pset            = &hch.provenance()->parameterSet();
      module_type     = pset->get<std::string>("module_type");
    }
  }

  const mu2e::StrawDigiMCCollection* mcdigis(0);
  art::Handle<mu2e::StrawDigiMCCollection> mcdH;
  Evt->getByLabel(makeSD_module_label, mcdH);
  mcdigis = mcdH.product();
  
  const mu2e::HelixSeed     *tmpHel(0);
  int                        nhelices(0);
  const mu2e::RobustHelix   *robustHel(0);
  const mu2e::CaloCluster   *cluster(0);
 
  if (list_of_helices) nhelices = list_of_helices->size();
  
  TParticlePDG* part(0);
  TDatabasePDG* pdg_db = TDatabasePDG::Instance();
  static XYZVec zaxis(0.0,0.0,1.0); // unit in z direction

  for (int i=0; i<nhelices; i++) {
    std::vector<int>     hits_simp_id, hits_simp_index, hits_simp_z;
    
    helix                  = cb->NewHelix();
    tmpHel                 = &list_of_helices->at(i);
    cluster                = tmpHel->caloCluster().get();
    robustHel              = &tmpHel->helix();
    if (cluster != 0){
      mu2e::GeomHandle<mu2e::Calorimeter> ch;
      const mu2e::Calorimeter* _calorimeter = ch.get();      
      
      helix->fClusterTime    = cluster->time();
      helix->fClusterEnergy  = cluster->energyDep();
      CLHEP::Hep3Vector gpos = _calorimeter->geomUtil().diskToMu2e(cluster->diskId(),cluster->cog3Vector());
      CLHEP::Hep3Vector tpos = _calorimeter->geomUtil().mu2eToTracker(gpos);
      helix->fClusterX       = tpos.x();
      helix->fClusterY       = tpos.y();
      helix->fClusterZ       = tpos.z();
    } else {
      helix->fClusterTime    = 0; 
      helix->fClusterEnergy  = 0; 
      helix->fClusterX       = 0; 
      helix->fClusterY       = 0; 
      helix->fClusterZ       = 0; 
    }
    
    helix->fHelix        = tmpHel;
    helix->fHelicity     = robustHel->helicity()._value;
    helix->fT0           = tmpHel->t0()._t0;
    helix->fT0Err        = tmpHel->t0()._t0err;     
    helix->fRCent        = robustHel->rcent  ();
    helix->fFCent        = robustHel->fcent  ();     
    helix->fRadius       = robustHel->radius ();
    helix->fLambda       = robustHel->lambda ();     
    helix->fFZ0          = robustHel->fz0    ();
    helix->fD0           = robustHel->rcent  () - robustHel->radius ();
    
    const mu2e::HelixHitCollection* hits = &tmpHel->hits();
    int nhits = hits->size();

    const mu2e::ComboHit*           hit(0);
    //    XYZVec                          pos(0,0,0), wdir(0,0,0), sdir(0,0,0), helix_center(0,0,0);
    //    double                          phi(0), helix_phi(0);
//-----------------------------------------------------------------------------
// figure out which algorithm was used to reconstruct the helix - helix collections 
// could be merged
// 1: RobustHelixFinder 2: CalHelixFinder
//-----------------------------------------------------------------------------
    int alg(-1);

    int mask = (0xffff << 16) | 0x0000;

    if (aid_coll) {
//-----------------------------------------------------------------------------
// MergeHelixFinder always stored the algorithm ID collection
//-----------------------------------------------------------------------------
      const mu2e::AlgorithmID* aid = &aid_coll->at(i);
      alg  = aid->BestID();
      mask = aid->BestID() | (aid->AlgMask() << 16);
    }
    else {
//-----------------------------------------------------------------------------
// no algorithm ID stored - either RobustHelixFinder or CalHelixFinder
//-----------------------------------------------------------------------------
      if      (module_type == "RobustHelixFinder") {
	alg  = mu2e::AlgorithmID::TrkPatRecBit;
	mask = alg | (alg << 16);
      }
      else if (module_type == "CalHelixFinder"   ) {
	alg  = mu2e::AlgorithmID::CalPatRecBit;
	mask = alg | (alg << 17);
      }
      else {
	printf(" ERROR in InitHelixBlock: module_type = %s\n",module_type.data());
      }
    }

    helix->fAlgorithmID = mask;

    int      nStrawHits(0);

    for (int j=0; j<nhits; ++j) {      //this loop is made over the ComboHits
      hit       = &hits->at(j);
      //get the MC truth info
      if (hit->_flag.hasAnyProperty(mu2e::StrawHitFlag::outlier))         continue;

      std::vector<StrawDigiIndex> shids;
      hits->fillStrawDigiIndices(*(Evt),j,shids);

      for (size_t k=0; k<shids.size(); ++k) {
	mu2e::StrawDigiMC const&     mcdigi = mcdigis->at(shids[k]);
	art::Ptr<mu2e::StepPointMC>  spmcp;
	mu2e::TrkMCTools::stepPoint(spmcp,mcdigi);
	const mu2e::StepPointMC* Step = spmcp.get();
      
	if (Step) {
	  art::Ptr<mu2e::SimParticle> const& simptr = Step->simParticle(); 
	  int sim_id        = simptr->id().asInt();
	  float   dz        = Step->position().z();// - trackerZ0;
	  hits_simp_id.push_back   (sim_id); 
	  hits_simp_index.push_back(shids[k]);
	  hits_simp_z.push_back(dz);
	  break;
	}
      }

      //increase the counter of the StrawHits
      nStrawHits += hit->nStrawHits();
    } 

//-----------------------------------------------------------------------------
// find the SimParticle that created the majority of the hits
//-----------------------------------------------------------------------------
    int     max(0), mostvalueindex(-1), mostvalue(-1), id_max(0);
    float   dz_most(1e4);
    if (hits_simp_id.size()>0) mostvalue = hits_simp_id[0];
    
    for (int  k=0; k<(int)hits_simp_id.size(); ++k){
      int co = (int)std::count(hits_simp_id.begin(), hits_simp_id.end(), hits_simp_id[k]);
      if ( (co>0) && (co>=max)) {
	float  dz      = std::fabs(hits_simp_z[k]);
	if (dz < dz_most){
	  dz_most        = dz;
	  max            = co;
	  id_max         = k;
	  mostvalue      = hits_simp_id[k];
	  mostvalueindex = hits_simp_index[k];
	}
      }
    }

    helix->fSimpId1Hits = max;
    helix->fSimpId2Hits = -1;
    helix->fSimpPDGM1   = -1;
    helix->fSimpPDGM2   = -1;
    
    if (hits_simp_id.size()>0) {
      if ( (mostvalueindex != hits_simp_index[id_max]) ){
	printf(">>> ERROR: event %i %s helix %i no MC found. MostValueindex = %i hits_simp_index[id_max] =%i \n", 
	       Evt->event(), module_type.c_str(), i, mostvalueindex, hits_simp_index[id_max]);
      }
    }
    
    if ( (mostvalueindex<0) || (mostvalueindex >= (int)mcdigis->size()))        {
      printf(">>> ERROR: event %i %s helix %i no MC found. MostValueindex = %i hits_simp_index[id_max] = %i mcdigis_size =%li \n", 
	     Evt->event(), module_type.c_str(), i, mostvalueindex, hits_simp_index[id_max], mcdigis->size());
      continue;
    }
    
    mu2e::StrawDigiMC const&     mcdigi = mcdigis->at(mostvalueindex);
    art::Ptr<mu2e::StepPointMC>  spmcp;
    mu2e::TrkMCTools::stepPoint(spmcp,mcdigi);
    const mu2e::StepPointMC* Step = spmcp.get();
    const mu2e::SimParticle * sim (0);

    if (Step) {
      art::Ptr<mu2e::SimParticle> const& simptr = Step->simParticle(); 
      helix->fSimpPDG1    = simptr->pdgId();
      art::Ptr<mu2e::SimParticle> mother = simptr;
      part   = pdg_db->GetParticle(helix->fSimpPDG1);

      while(mother->hasParent()) mother = mother->parent();
      sim = mother.operator ->();

      helix->fSimpPDGM1   = sim->pdgId();
      
      double   px = Step->momentum().x();
      double   py = Step->momentum().y();
      double   pz = Step->momentum().z();
      double   mass  (-1.);
      double   energy(-1.);
      if (part) {
	mass   = part->Mass();
	energy = sqrt(px*px+py*py+pz*pz+mass*mass);
      }
      helix->fMom1.SetPxPyPzE(px,py,pz,energy);

      const CLHEP::Hep3Vector* sp = &simptr->startPosition();
      helix->fOrigin1.SetXYZT(sp->x(),sp->y(),sp->z(),simptr->startGlobalTime());
    }
    
    //look for the second most frequent hit
    if (max != int(hits_simp_id.size()) ){  //nhits){
      int   secondmostvalueindex(-1);
      max     = 0;//reset max
      dz_most = 1e4;

      for (int k=0; k<(int)hits_simp_id.size(); ++k){
	int value = hits_simp_id[k];
	int co = (int)std::count(hits_simp_id.begin(), hits_simp_id.end(), value);
	if ( (co>0) && (co>=max) && (value != mostvalue)) {
	  float  dz      = std::fabs(hits_simp_z[k]);
	  if (dz < dz_most){
	    max                  = co;
	    dz_most              = dz;
	    secondmostvalueindex = hits_simp_index[k];
	  }
	}
      }
      helix->fSimpId2Hits = max;

      if (secondmostvalueindex >= 0) {

	mu2e::StrawDigiMC const&     mcdigi = mcdigis->at(secondmostvalueindex);
	art::Ptr<mu2e::StepPointMC>  spmcp;
	mu2e::TrkMCTools::stepPoint(spmcp,mcdigi);
	const mu2e::StepPointMC* Step = spmcp.get();
	const mu2e::SimParticle * sim (0);

	if (Step) {
	  art::Ptr<mu2e::SimParticle> const& simptr = Step->simParticle(); 
	  helix->fSimpPDG2    = simptr->pdgId();
	  art::Ptr<mu2e::SimParticle> mother = simptr;
	  part   = pdg_db->GetParticle(helix->fSimpPDG2);

	  while(mother->hasParent()) mother = mother->parent();
	  sim = mother.operator ->();

	  helix->fSimpPDGM2   = sim->pdgId();
      
	  double   px = simptr->startMomentum().x();
	  double   py = simptr->startMomentum().y();
	  double   pz = simptr->startMomentum().z();
	  double   mass(-1.);//  = part->Mass();
	  double   energy(-1.);// = sqrt(px*px+py*py+pz*pz+mass*mass);
	  if (part) {
	    mass   = part->Mass();
	    energy = sqrt(px*px+py*py+pz*pz+mass*mass);
	  }
	  helix->fMom2.SetPxPyPzE(px,py,pz,energy);

	  const CLHEP::Hep3Vector* sp = &simptr->startPosition();
	  helix->fOrigin2.SetXYZT(sp->x(),sp->y(),sp->z(),simptr->startGlobalTime());
	}      
      }
    }
    
    helix->fNComboHits   = tmpHel->hits().size();
    helix->fNHits        = nStrawHits;
    helix->fChi2XYNDof   = robustHel->chi2dXY();
    helix->fChi2PhiZNDof = robustHel->chi2dZPhi();
  }

  return 0;
}

//-----------------------------------------------------------------------------
Int_t StntupleInitMu2eHelixBlockLinks(TStnDataBlock* Block, AbsEvent* AnEvent, int Mode) 
{

  Int_t  ev_number, rn_number;

  ev_number = AnEvent->event();
  rn_number = AnEvent->run();

  if (! Block->Initialized(ev_number,rn_number)) return -1;

					// do not do initialize links 2nd time

  if (Block->LinksInitialized()) return 0;

  TStnEvent*           ev;
  TStnHelixBlock*      hb;
  TStnHelix*           helix;
  TStnTrackSeedBlock*  tsb;
  TStnTrackSeed*       trkseed;
  TStnTimeClusterBlock*   tpb;
  TStnTimeCluster*        tp;

  const mu2e::HelixSeed*   khelix, *fkhelix;
  const mu2e::KalSeed*     kseed;
  const mu2e::TimeCluster* ktimepeak, *fktimepeak;

  char                     short_helix_block_name[100], timepeak_block_name[100];

  ev     = Block->GetEvent();
  hb     = (TStnHelixBlock*) Block;
  
  hb->GetModuleLabel("mu2e::KalSeedCollection"    , short_helix_block_name);
  hb->GetModuleLabel("mu2e::TimeClusterCollection", timepeak_block_name   );

  tsb    = (TStnTrackSeedBlock*) ev->GetDataBlock(short_helix_block_name);
  tpb    = (TStnTimeClusterBlock* ) ev->GetDataBlock(timepeak_block_name   );

  int    nhelix   = hb ->NHelices();
  int    ntrkseed = tsb->NTrackSeeds();
  int    ntpeak   = tpb->NTimeClusters();

  for (int i=0; i<nhelix; ++i){
    helix  = hb   ->Helix(i);
    khelix = helix->fHelix;
    int      trackseedIndex(-1);
    for (int j=0; j<ntrkseed; ++j){
      trkseed = tsb->TrackSeed(j);
      kseed   = trkseed->fTrackSeed;
      fkhelix = kseed->helix().get();
      if (fkhelix == khelix) {
	trackseedIndex = j;
	break;
      }
    }
    
    if (trackseedIndex < 0) {
      printf(">>> ERROR: CalHelixFinder helix %i -> no TrackSeed associated\n", i);//FIXME!
      continue;
    }
    helix->SetTrackSeedIndex(trackseedIndex);

    ktimepeak = khelix->timeCluster().get();
    int      timepeakIndex(-1);
    for (int j=0; j<ntpeak;++j){
      tp         = tpb->TimeCluster(j);
      fktimepeak = tp->fTimeCluster;
      if (fktimepeak == ktimepeak){
	timepeakIndex = j;
	break;
      }
    }

    if (timepeakIndex < 0) {
      printf(">>> ERROR: CalHelixFinder helix %i -> no TimeCluster associated\n", i);//FIXME!
      continue;
    }

    helix->SetTimeClusterIndex(timepeakIndex);
  }
//-----------------------------------------------------------------------------
// mark links as initialized
//-----------------------------------------------------------------------------
  hb->fLinksInitialized = 1;

  return 0;
}

