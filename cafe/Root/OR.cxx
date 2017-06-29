#include "cafe/OR.h"
#include "cafe/ParseRun.h"
#include "cafe/Config.h"
#include "xAODRootAccess/TEvent.h"

#include "TFile.h"
#include "TTreeFormula.h"

#include <iostream>
#include <vector>

namespace cafe {

    OR::OR(const char *name) : Controller(name) {
    }
    // Processor interface
    bool OR::processEvent(xAOD::TEvent& event) {

        if (_processors.size()==0) return true ;
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end(); it++) {
            (*it)->incEventCount();
            if ((*it)->processEvent(event))  return true ;
        }

        return false ;
    }
}

ClassImp(cafe::OR)

