

#include "cafe/Processor.h"

//LD #include "cafe/Plugins.h"
#include "cafe/Config.h"
//LD #include "cafe/Stat.h"

#include <assert.h>
#include <iostream>
#include <string>

// for gDirectory
#include "TROOT.h"
#include "TClass.h"
#include "TMethodCall.h"
#include "TInterpreter.h"

//LD extern cafe::Stat* STAT ; 

namespace cafe {

    Processor::Processor(const char *name): 
        _writexAOD(false),
        _eventCount(0),
        _debug(0),
        _name(name),
        _directory(0),
        _parent(0)
    {}

    Processor::Processor(): 
        _writexAOD(false),
        _eventCount(0),
        _debug(0),
        _name(""),
        _directory(0),
        _parent(0)
    {}

    Processor::~Processor()
    {}


    void Processor::begin() 
    {
    }

    void Processor::finish()
    {
    }

    void Processor::inputFileOpened(TFile *file)
    {
    }

    void Processor::inputFileClosing(TFile *file)
    {
    }

    bool Processor::processEvent(xAOD::TEvent& event)
    {
        return true;
    }

    unsigned int Processor::debug() const
    {
        return _debug;
    }

    void Processor::setDebug(unsigned int level)
    {
        _debug = level;
    }

    void Processor::incEventCount()
    {
        _eventCount++;
        //LD if (STAT) STAT->EventSelected("PROCESSOR \"" + name() + "\" (input)") ;
    }

    int  Processor::eventCount() const
    {
        return _eventCount;
    }

    std::string Processor::name() const 
    {
        return _name;
    }

    void Processor::setDirectory(TDirectory *dir)
    {
        _directory = dir;
    }

    TDirectory *Processor::getDirectory() const
    {
        return _directory  ? _directory : (_parent != 0 ? (_directory = _parent->getDirectory()) : _directory = gROOT);
    }


    void  Processor::setParent(Processor *parent)
    {
        _parent = parent;
    }


    Processor *Processor::getParent() const
    {
        return _parent;
    }

    std::string Processor::resolve(const std::string& name, bool recurse) const
    {
        const Processor *proc = this;

        std::string result = "";

        while(proc != 0) {
            Config conf(proc->name());
            result = conf.get(name, "");
            if((result != "") || !recurse) break;
            proc = proc->getParent();
        }

        return result;
    }

    std::string Processor::replace(const std::string& input, bool recurse) const
    {
        using namespace std;

        string result(input);
        string::size_type pos = 0;

        while(pos != string::npos) {
            pos = result.find("%{", pos);
            if(pos != string::npos) {
                string::size_type end = result.find('}', pos);
                if(end != string::npos) {
                    result.replace(pos, end - pos + 1, resolve(result.substr(pos + 2, end - pos - 2), recurse));
                }
                pos = end;
            }
        }
        return result;
    }

    std::string Processor::fullName() const
    {
        // Return a path starting with 'name()'
        //
        // Not efficient but correct
        //
        if(_parent != 0) {
            std::string result = _parent->fullName();
            result += "/";
            result += _name;
            return result;
        } else {
            return "/" + _name;
        }
    }

    std::ostream& Processor::out()
    {
        return std::cout;
    }

    std::ostream& Processor::info()
    {
        return std::cerr;
    }

    std::ostream& Processor::warn()
    {
        return std::cerr;
    }


    std::ostream& Processor::err()
    {
        return std::cerr;
    }


    // static 
    Processor* Processor::Create(const std::string& name, const std::string& instance)
    {

        // Controller always does this right, but protect against users
        std::string inst = instance;
        if(inst == "") {
            inst = name;
        }

        ///
        /// Try to load class. This requires that the user
        /// has updated the .rootmap file (e.g. with autoroot.py) and
        /// the Processor name and class name are identical.
        ///
        /// Todo: check if/how to handles namespaces.
        ///
        if(TClass *cl = gROOT->GetClass (name.c_str())) {

            std::string::size_type pos = name.rfind(':');
            std::string n = name;
            if(pos != std::string::npos) {
                n = name.substr(pos + 1);
            }

            if(cl->GetMethodWithPrototype(n.c_str(),"const char *") != 0) {
                TMethodCall call;
                call.InitWithPrototype(cl, n.c_str(), "const char *");
                call.ResetParam();
                call.SetParam((Long_t )inst.c_str());
                Long_t ret;
                call.Execute(ret);
                return reinterpret_cast<Processor*>(ret);
            }
        }

        ///
        /// Use the plugin manager to fetch the processor
        ///

        //LD Plugins *plugins = Plugins::instance();
        //LD return plugins->load<Processor>("Processor", name, instance);
        return 0;
    }

    void Processor::write_xAOD_event()
    {
        _writexAOD = true;
        if ( _parent ) _parent->write_xAOD_event();
    }


}

ClassImp(cafe::Processor)
