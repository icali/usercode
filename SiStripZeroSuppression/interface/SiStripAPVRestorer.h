#ifndef RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPAPVRESTORER_H
#define RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPAPVRESTORER_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/SiStripObjects/interface/SiStripNoises.h"
#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"
#include "CalibFormats/SiStripObjects/interface/SiStripQuality.h"
#include "CondFormats/DataRecord/interface/SiStripNoisesRcd.h"
#include "CondFormats/DataRecord/interface/SiStripPedestalsRcd.h"
#include "CalibTracker/Records/interface/SiStripQualityRcd.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripProcessedRawDigi.h"

#include <vector>
#include <stdint.h>

typedef std::map< uint16_t, int16_t> DigiMap;
typedef std::map< uint16_t, std::vector < int16_t> > RawDigiMap;
typedef std::map< uint16_t, int16_t>::iterator DigiMapIter;
typedef std::map<uint32_t, std::vector<float> > CMMap;  //detId, Vector of MeanCM per detId


class SiStripAPVRestorer {

 friend class SiStripRawProcessingFactory;

 public:
  
  virtual ~SiStripAPVRestorer() {};

  void     init(const edm::EventSetup& es);
  int16_t  inspect(const uint32_t&, std::vector<int16_t>&, const std::vector< std::pair<short,float> >&);
  void     restore(std::vector<int16_t>&);
  void     fixAPVsCM(edm::DetSet<SiStripProcessedRawDigi>& );
  void     LoadMeanCMMap(edm::Event&);
  RawDigiMap& GetBaselineMap(){return BaselineMap_;}
  std::vector< DigiMap >& GetSmoothedPoints(){return SmoothedMaps_;}
  void GetAPVFlags(std::vector<bool>&);

 protected:

  SiStripAPVRestorer(const edm::ParameterSet& conf);

 private:
 
  //template<typename T>float median( std::vector<T>& );
  //template<typename T>void IterativeMedian(std::vector<T>&, uint16_t); 
  
  template<typename T >int16_t NullInspect(std::vector<T>&);
  template<typename T >int16_t AbnormalBaselineInspect(std::vector<T>&);
  template<typename T >int16_t BaselineFollowerInspect(std::vector<T>&);
  template<typename T >int16_t BaselineAndSaturationInspect(std::vector<T>&);
  template<typename T >void FlatRestore( std::vector<T>& , uint16_t);
  template<typename T >void BaselineFollowerRestore( std::vector<T>&, uint16_t, float );
  
  void BaselineFollower(DigiMap&, std::vector<int16_t>&, float);
  bool FlatRegionsFinder(std::vector<int16_t>&, DigiMap&, float , uint16_t);
  void BaselineCleaner(std::vector<int16_t>&, DigiMap&, uint16_t );

  void CreateCMMapRealPed(const edm::DetSetVector<SiStripRawDigi>& );
  void CreateCMMapCMstored(const edm::DetSetVector<SiStripProcessedRawDigi>& );
 
  float pairMedian( std::vector<std::pair<float,float> >&); 
  
  edm::ESHandle<SiStripQuality> qualityHandle;
  uint32_t  quality_cache_id;
  
  edm::ESHandle<SiStripNoises> noiseHandle;
  uint32_t noise_cache_id;
  
  edm::ESHandle<SiStripPedestals> pedestalHandle;
  uint32_t pedestal_cache_id;
  
  std::vector<std::string> apvFlags_;
  std::vector<float> median_;
  std::vector<bool> badAPVs_;
  std::vector< DigiMap > SmoothedMaps_;
  RawDigiMap BaselineMap_;
  
  
  uint32_t detId_;
  
  CMMap MeanCMmap_;
  edm::InputTag inputTag_;
  
  
  bool ForceNoRestore_;
  bool SelfSelectRestoreAlgo_;
  std::string InspectAlgo_;
  std::string RestoreAlgo_;
  bool useRealMeanCM_;
  
  //--------------------------------------------------
  // Configurable Parameters of Algorithm
  //--------------------------------------------------

  double   fraction_;                  // fraction of strips deviating from nominal baseline
  uint32_t deviation_;                 // ADC value of deviation from nominal baseline 
  double   restoreThreshold_;          // for Null inspect (fraction of adc=0 channels)
  uint32_t DeltaCMThreshold_;          // for BaselineFollower inspect
  
  uint32_t nSigmaNoiseDerTh_;          // threshold for rejecting hits strips
  uint32_t consecThreshold_;           // minimum length of flat region
  uint32_t hitStripThreshold_;         // height above median when strip is definitely a hit
  uint32_t nSmooth_;                   // for smoothing and local minimum determination (odd number)
  uint32_t minStripsToFit_;            // minimum strips to try spline algo (otherwise default to median)
  uint32_t distortionThreshold_;       // (max-min) of flat regions to trigger baseline follower
  double   CutToAvoidSignal_;	       // for iterative median implementation internal to APV restorer
  uint32_t nSaturatedStrip_;           // for BaselineAndSaturation inspect
  bool ApplyBaselineCleaner_;
    
};

#endif
