
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TKey.h"
#include "TSystem.h"

#include "cafe/Config.h"
#include "cafe/Expander.h"
#include "cafe/RunController.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODMetaDataCnv/FileMetaDataTool.h"
#include "xAODTriggerCnv/TriggerMenuMetaDataTool.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include <cassert>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

namespace cafe {

    RunController::RunController(const char *name)
      : Controller(name),
	_num_events(0),
	_max_events(0),
	_max_files(0),
	_num_files(0),
	_progress(0),
        _storeDump(0),
	_skip(0),
	_skip_files(0),
	_xAODoutfile(0),
	_containers(),
	_WriteAllEvents(false),
	_AccessMode(0),
	_TTreeCacheSize(-999)
    {
        Config config(name);
        _max_events = config.get("Events", 0);
        _max_files  = config.get("Files", 0);
        _input      = config.get("Input", "file:input.root");
        _progress   = config.get("Progress", 0);
        _storeDump  = config.get("StoreDump", 0);
        _skip       = config.get("Skip", 0);
        _skip_files = config.get("SkipFiles",0);
        assert(_skip_files >= 0);
        std::string xAODout = config.get("xAODOutput","");
        if ( xAODout != "" ) {
            _xAODoutfile = TFile::Open(xAODout.c_str(),"CREATE");
        }
        _containers = config.getVString("Containers");
        _WriteAllEvents = config.get("WriteAllEvents",false);

        _AccessMode = config.get("AccessMode",0);
        if ( _AccessMode > xAOD::TEvent::kAthenaAccess ) throw std::logic_error("No TEvent access mode corresponding to AccessMode !");
        _TTreeCacheSize = config.get("TTreeCacheSize",-999);
    }


    RunController::~RunController()
    {
        if ( _xAODoutfile ) {
            _xAODoutfile->Write();
            _xAODoutfile->Close();
            delete _xAODoutfile;
        }
    }

    void RunController::loop(xAOD::TEvent& event, xAOD::TStore& store)
    {

        Long64_t index = 0;
        Long64_t entry = -1;

        if(_skip > 0) {
            index = _skip;
            out() << "RunController[cafe]: Skipped events = " << _skip << std::endl;
        }

        while((_max_events == 0 || _num_events < _max_events) && 
                (entry = event.getEntry(index++)) >= 0 ) {
            incEventCount();
            _num_events++;
            if(_progress && (_num_events % _progress == 0)) {
                long vm_usage=0,resident_set=0;
                process_mem_usage(vm_usage, resident_set);
                out() << "cafe: Processed events = " << _num_events << "       VMEM " << vm_usage/1024 << "m  RSS " <<  resident_set/1024 << "m " << std::endl;
            }
            _writexAOD = false;
            processEvent(event);

            if(_storeDump && (_num_events % _storeDump == 0)) {
                store.print();
            }

            if ( !_xAODoutfile && (_WriteAllEvents || _writexAOD)) throw std::logic_error("xAOD writing required but no file specified");;

            if ( _xAODoutfile && (_WriteAllEvents ||_writexAOD)) {
                for (std::size_t i = 0; i < _containers.size(); ++i ) {
                    event.copy(_containers[i]);
                }
                event.fill();
            }
            store.clear(); // clear transient store
        }

        // Adjust skip event value.
        _skip = _skip - (int)event.getEntries();
        if(_skip < 0 ) _skip = 0; 
    }

    bool RunController::Run(unsigned int max_events)
    {
        if(_max_events == 0) {
            _max_events = max_events;
        }

        out() << "RunController[" << name() << "] Input: " << _input << std::endl;
        if(_max_events > 0) {
            out() << "RunController[" << name() << "] MaxEvents: " << _max_events << std::endl;
        }

        if(_max_files > 0) {
            out() << "RunController[" << name() << "] MaxFiles: " << _max_files << std::endl;
        }


        std::auto_ptr<Expander> exp(Expander::create(_input));
        if(exp.get()) {

            std::auto_ptr<xAOD::TEvent> event(new xAOD::TEvent(xAOD::TEvent::kBranchAccess));
            if ( _AccessMode == xAOD::TEvent::kClassAccess ) {
                event = std::auto_ptr<xAOD::TEvent> (new xAOD::TEvent(xAOD::TEvent::kClassAccess)); // kClassAccess needed for e/gamma in DC14-8TeV
            }
            if ( _xAODoutfile ) {
                event->writeTo(_xAODoutfile);
            }
            xAOD::TStore transientStorage;
            transientStorage.setActive();
            _num_events = 0;

	    std::auto_ptr<xAODMaker::FileMetaDataTool>  metadata_tool(new xAODMaker::FileMetaDataTool());
	    metadata_tool->initialize();

	    std::auto_ptr<xAODMaker::TriggerMenuMetaDataTool> trigger_metadata_tool(new xAODMaker::TriggerMenuMetaDataTool());
	    trigger_metadata_tool->initialize();

            begin();

            // check for unused config variables
            std::vector<std::string> unused = Config::checkConfiguration();

            if(unused.size() > 0) {
                err() << "-----------------" << std::endl;
                err() << "ERROR: Unused (mispelled?) configuration variables:" << std::endl;
                for(std::vector<std::string>::iterator it = unused.begin();
                        it != unused.end();
                        ++it) {
                    err() << *it << std::endl;
                }
                err() << "-----------------" << std::endl;
            }

            std::string nextFile = exp->nextFile();
            while(nextFile.size() > 0 &&
                    ((_max_files == 0) || (_num_files++ < _max_files)) &&
                    ((_max_events == 0) || (_num_events < _max_events))) {

                // Check if we should skip first N files, if no, open file
                if(_skip_files == 0) {

		  //TFile *file = TFile::Open(nextFile.c_str(), "READ");
		  TFile *file = OpenWithRetries(nextFile);

                    if(file && !file->IsZombie()) {
                        out() << "RunController[" << name() 
                            << "]: Input file opened: " << nextFile << std::endl;
                        if (  _TTreeCacheSize > -999 ) setTreeCaches(file);

                        if ( ! event->readFrom(file).isSuccess() ) {
                            throw std::runtime_error("Could not connect TEvent to file !");
                        }

                        inputFileOpened(file);

                        loop(*event,transientStorage);

                        inputFileClosing(file);

                        file->Close();
                        delete file;

                    } else {
                        // cannot open file
                        throw std::runtime_error("RunController: Cannot open file: "+nextFile);
                        return false;
                    }
                } else { // if(_skip_files == 0)
                    _num_files--;
                    _skip_files--;
                }

                // try to get next file
                nextFile = exp->nextFile();
            }

            out() << fullName() << " : " << eventCount() << std::endl;
            if ( _xAODoutfile ) {
                event->finishWritingTo(_xAODoutfile);
            }
            finish();

            return true;

        } else {
            // Invalid URL
            throw std::runtime_error("RunController: Cannot open input: " + _input);
            return false;
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    //
    // process_mem_usage(long &, long &) - takes two doubles by reference,
    // attempts to read the system-dependent data for a process' virtual memory
    // size and resident set size, and return the results in KB.
    //
    // On failure, returns 0.0, 0.0
    //
    // From a StackOverflow example from Don Wakefield
    void RunController::process_mem_usage(long& vm_usage, long& resident_set)
    {
        using std::ios_base;
        using std::ifstream;
        using std::string;

        vm_usage     = 0.0;
        resident_set = 0.0;

        // 'file' stat seems to give the most reliable results
        //
        ifstream stat_stream("/proc/self/stat",ios_base::in);

        // dummy vars for leading entries in stat that we don't care about
        //
        string pid, comm, state, ppid, pgrp, session, tty_nr;
        string tpgid, flags, minflt, cminflt, majflt, cmajflt;
        string utime, stime, cutime, cstime, priority, nice;
        string O, itrealvalue, starttime;

        // the two fields we want
        //
        unsigned long vsize;
        long rss;

        stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

        stat_stream.close();

        long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
        vm_usage     = vsize / 1024;
        resident_set = rss * page_size_kb;
    }

    void RunController::setTreeCaches(TFile* tfile)
    {
        if ( !tfile ) return;
        TIter next(tfile->GetListOfKeys());
        TKey *key;
        while ((key = (TKey*)next())) {
            TClass *cl = gROOT->GetClass(key->GetClassName());
            if (!cl->InheritsFrom("TTree")) continue;
            TTree* tree = (TTree*) tfile->Get( key->GetName());
            if ( tree ) {
                std::string name(tree->GetName());
                if ( name != "CollectionTree" ) {
                    tree->SetCacheSize(0);
                    tree->SetCacheSize(1000000);
                }
                else if ( _TTreeCacheSize > -2 ) {
                    tree->SetCacheSize(0);
                    tree->SetCacheSize( _TTreeCacheSize);
                }
            }
        }
    }

  TFile* RunController::OpenWithRetries(const std::string& name)
  {
    static std::vector<int> waitTimeInSec = {10,30,120,300,1200};
    // if there is a : in the name, it's a file on a server so it may be worth 
    // retrying opening the file.
    bool canRetry = name.find(':') !=  std::string::npos;
    if ( canRetry ) {
      for ( auto delay :  waitTimeInSec ){
	TFile* f = TFile::Open(name.c_str(),"READ");
	if (f && !f->IsZombie()) {
	  return f;
	}
	else {
	  out() << "Could not open file, will retry after " << delay << " seconds" << std::endl;
	  std::this_thread::sleep_for (std::chrono::seconds(delay));
	}
      }
      return 0;
    }
    else {
      return TFile::Open(name.c_str(),"READ");
    }
  }


}


ClassImp(cafe::RunController)



