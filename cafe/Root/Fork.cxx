

#include "cafe/Fork.h"
#include "cafe/ParseRun.h"
#include "cafe/Config.h"
#include "xAODRootAccess/TEvent.h"

namespace cafe {

    Fork::Fork(const char *name) 
        : Controller(name)
    {
    }

    // Processor interface
    bool Fork::processEvent(xAOD::TEvent& event)
    {
        for(std::list<Processor*>::iterator it = _processors.begin();
                it != _processors.end();
                ++it) {
            (*it)->incEventCount();
            (*it)->processEvent(event) ;
        }
        return true;
    }
}

ClassImp(cafe::Fork)

