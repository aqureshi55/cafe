
#include "cafe/Function.h"
#include "xAODRootAccess/TEvent.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

namespace cafe {

    Function::Function(const char *name)
        : Processor(name)
    {
        // ok, now try to find the function somewhere
        // in the loaded libraries...

        _func = getMap()[name];
        if(_func == 0) {
            std::string msg("Function: Cannot load function: ");
            msg += name;
            // err() << msg << std::endl;
            throw std::runtime_error(msg);
        }
    }

    Function::Function(const std::string& name, bool (*func)(xAOD::TEvent&))
        : Processor(name.c_str()),
        _func(func)
    {
    }


    bool Function::processEvent(xAOD::TEvent& event)
    {
        if(_func) {
            return _func(event);
        } else {
            err() << "cafe::Function: FATAL: function not found: " << name() << std::endl;
            abort();
        }
    }

    Function::Map& Function::getMap()
    {
        static Map s_map;
        return s_map;
    }


    Function::Register::Register(const char *name, FUNC func)
    {
        getMap()[name] = func;
    }
}

ClassImp(cafe::Function)

