#include <iostream>

#include <filesystem>

#include <boost/program_options.hpp>

#include "TROOT.h"
#include "TInterpreter.h"
#include "TGlobal.h"
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TPython.h"

#include "SNDLHCEventHeader.h"
#include "sndFilterProcessBase.h"
#include "ShipMCTrack.h"

namespace po = boost::program_options;
int main(int argc, char ** argv) {

    po::options_description desc("SNDLHC event filter");
    desc.add_options()
    ("help,h", "Show help message")
    ("input,i", po::value<std::string>()->required(), "Input file name (or reg exp)")
    ("output,o", po::value<std::string>()->default_value("output.root"), "Output file name")
    ("geofile,g", po::value<std::string>()->default_value("output.root"), "Geometry file name")
    ("pipeline,p", po::value<std::string>()->default_value("output.py"), "Path to process pipeline definition. For example, in ${SNDSW_ROOT}/analysis/pipelines/")
    ("recofile,r", po::value<std::string>()->default_value(""), "Path to reconstructed muon tracks file.");
  
    po::variables_map vm;

    try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::string input = vm["input"].as<std::string>();
    std::string output = vm["output"].as<std::string>();
    std::string geofile = vm["geofile"].as<std::string>();
    std::string pipeline_filename = vm["pipeline"].as<std::string>();
    std::string recofile = vm["recofile"].as<std::string>();
    
    // Check if files exist
    if (! std::filesystem::exists(geofile)) {
        std::cout << "Geometry file does not exist: "+geofile; 
    }
    if (! std::filesystem::exists(pipeline_filename)) {
        std::cout << "Cut set definition file does not exist: "+pipeline_filename;
    }

    // Input files
    bool isMC = false;
    TChain * ch = new TChain("rawConv");
    ch->Add(input.c_str());
    if (ch->GetEntries() == 0){
        delete ch;
        ch = new TChain("cbmsim");
        ch->Add(input.c_str());
        if (ch->GetEntries() > 0) {
        isMC = true;
        } else {
        std::cout << "Didn't find rawConv or cbmsim in input file" << std::endl;
        }
    }
    std::cout << "Got input tree" << std::endl;
    std::cout.flush();

    TChain * ch_reco;
    if (!recofile.empty()){
        if (isMC) ch_reco = new TChain("cbmsim");
        else ch_reco = new TChain("rawConv");
        ch_reco->Add(recofile.c_str());
        ch->AddFriend(ch_reco);
    }

    // Use ROOT list of globals to share the event data with the cut classes
    // Add TTree to ROOT globals
    TCollection * rootGlobals = gROOT->GetListOfGlobals();
    rootGlobals->Add(ch);

    // Add branches to ROOT globals
    // EventHeader
    SNDLHCEventHeader * event_header = new SNDLHCEventHeader();
    ch->SetBranchAddress("EventHeader", &event_header);
    ch->GetEntry(0);

    if (event_header->GetEventNumber() == -1) {
        ch->SetBranchAddress("EventHeader.", &event_header);
        ch->GetEntry(0);
        if (event_header->GetEventNumber() == -1) {
        std::cout << "Invalid event header" << std::endl; 
        }
    }
    rootGlobals->Add(event_header);

    // MC truth
    TClonesArray * MCTracks; 
    if (isMC) {
        MCTracks = new TClonesArray("ShipMCTrack", 5000);
        ch->SetBranchAddress("MCTrack", &MCTracks);
        rootGlobals->Add(MCTracks); // Name: ShipMCTracks
    }


    // Output file
    TFile * outFile = new TFile(output.c_str(), "RECREATE");
    std::cout << "Got output file" << std::endl;

    
    // Get the pipeline via the ROOT interpreter
    gInterpreter->Declare(("#include \""+pipeline_filename+"\"").c_str());
    if (! static_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("pipeline"))){
        std::cout << "Invalid pipeline file" << std::endl;
    }
    std::vector< snd::analysis_core::baseProcess * > * pipeline_ptr = static_cast<std::vector< snd::analysis_core::baseProcess * > *>(static_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("pipeline"))->GetAddress());

    std::vector< snd::analysis_core::baseProcess * >& pipeline = *pipeline_ptr;

    
    ch->GetEntry(0);
    ch->GetFile()->Get("BranchList")->Write("BranchList", TObject::kSingleKey);
    ch->GetFile()->Get("TimeBasedBranchList")->Write("TimeBasedBranchList", TObject::kSingleKey);
    if (ch->GetFile()->Get("FileHeader")) ch->GetFile()->Get("FileHeader")->Write();
    if (ch->GetFile()->Get("FileHeaderHeader")) ch->GetFile()->Get("FileHeaderHeader")->Write();

    // Set up all branches to copy to output TTree.
    TTree * outTree = ch->CloneTree(0);
    std::cout << "Got output tree" <<std::endl;
    
    // Get number of entries
    unsigned long int n_entries = ch->GetEntries();


    std::cout << "sndEventFilter works!" << std::endl;
    return 0;
}
