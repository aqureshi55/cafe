
#include "cafe/Controller.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "cafe/Function.h"
#include "cafe/ParseRun.h"
#include "cafe/Config.h"
#include "xAODRootAccess/TEvent.h"


#include "TTree.h"
#include "TFile.h"

namespace cafe {

    Controller::Controller(const char *name)
        : Processor(name),
        _outfile(0)
    {
        Config config(name);

        // Get .Debug entry
        Processor::setDebug(config.get("Debug", 0));

        // Get .Run entry
        std::string run = config.get("Run","");
        if(run != "") {
            ParseRun parser;
            add(parser.parse(run));
        } // else assume Processors are added by other means
    }

    Controller::Controller(const char *name, const std::list<Processor*>& procs)
        : Processor(name),
        _outfile(0)
    {
        add(procs);
    }

    Controller::~Controller()
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            delete *it;
        }
    }


    void Controller::begin()
    {
        Config config(name());

        // Get .Output entry
        std::string outfilename = config.get("Output", "");
        if(outfilename.size() != 0) {

            bool useUpdate = config.get("UseUpdate",false);
            if (useUpdate) _outfile = TFile::Open(outfilename.c_str(), "UPDATE");
            else  _outfile = TFile::Open(outfilename.c_str(), "RECREATE");

            if(_outfile && _outfile->IsOpen()) {
                setDirectory(_outfile);
                out() << "Controller[" << name() << "] Created output file: "
                    << outfilename << std::endl;
            } else {
                throw std::runtime_error("Controller[" + name() +  
                        "] Cannot create output file: "+ outfilename)  ;
                delete _outfile;
                _outfile = 0;
            }
        }

        // Get .Directory entry
        std::string dirname = config.get("Directory", "");
        if(dirname.size() != 0) {
            if(TDirectory *dir = getDirectory()->mkdir(dirname.c_str())) {
                setDirectory(dir);
                out() << "Controller[" << name() << "] Created directory: " 
                    << dirname << std::endl;
            } else {
                getDirectory()->cd(dirname.c_str());
            }
        }

        std::for_each(_processors.begin(), _processors.end(),
                std::mem_fun(&Processor::begin));

        if(_processors.empty() && config.get("Run","---") == "---") {
            throw std::runtime_error("No .Run: specification for " + name());
        }
    }

    void Controller::finish()
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            std::cout << (*it)->fullName() << " : " << (*it)->eventCount() << std::endl;
        }

        std::for_each(_processors.begin(), _processors.end(),
                std::mem_fun(&Processor::finish));

        if(_outfile) {
            out() << "Controller[" << name() << "] Closing output file: " 
                << _outfile->GetName() << std::endl;
            _outfile->Write();
            _outfile->Close();
            delete _outfile;
            _outfile = 0;
        }
    }

    void Controller::inputFileOpened(TFile *file) 
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            (*it)->inputFileOpened(file);
        }
    }

    void Controller::inputFileClosing(TFile *file)
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            (*it)->inputFileClosing(file);
        }
    }

    bool Controller::processEvent(xAOD::TEvent& event)
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            (*it)->incEventCount();
            if(!(*it)->processEvent(event)) return false;
        }
        return true;
    }

    void Controller::setDebug(unsigned int level)
    {
        Processor::setDebug(level);
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            (*it)->setDebug(level);
        } 
    }


    bool Controller::add(Processor *proc)
    {
        if(proc) {
            setParent(proc,this);
            if(debug() > 0) {
                proc->setDebug(debug());
            }
            out() << "Controller[" << name() << "]: Adding " << proc->fullName() << std::endl;
            _processors.push_back(proc);
            return true;
        } else {
            return false;
        }
    }

    bool Controller::add(bool (*func)(xAOD::TEvent&), const std::string& instance)
    {
        return add(new Function(instance, func));
    }

    bool Controller::add(const std::string& name, const std::string& instance)
    {
        return add(Processor::Create(name, instance == "" ? name : instance));
    }

    bool Controller::add(const std::list<Processor*>& procs)
    {
        for(std::list<Processor*>::const_iterator it = procs.begin();
                it != procs.end();
                ++it) {
            add(*it);
        }
        return true;
    }
}

ClassImp(cafe::Controller)
