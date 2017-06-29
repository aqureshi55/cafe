
#include <iostream>
#include <algorithm>
#include "cafe/Config.h"
#include "cafe/RunController.h"
#include "xAODRootAccess/Init.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"

#include <cstdlib>
#include <stdexcept>

namespace {

    void load_package(const std::string& pkg)
    {
        std::string name = "lib";
        name += pkg;
        if(gSystem->Load(name.c_str()) == -1) {
            std::cerr << "cafe: Cannot load requested package: " << pkg << std::endl;
            exit(EXIT_FAILURE);
        }
    }


}


int main(int argc, char *argv[])
{
    using namespace cafe;
    using namespace std;

    // 
    // If the first argument does not end in ':', assume
    // it is the name of the configuration file
    //
    // if so, set CAFE_CONFIG to filename and skip to the
    // rest of arguments; this has to happen before we instantiate
    // the first version of 'Config'
    //
    if((argc > 1) && argv[1][strlen(argv[1])-1] != ':') {
        gSystem->Setenv("CAFE_CONFIG", argv[1]);
        --argc;
        ++argv;
    }

    Config config("cafe");

    //
    // Every pair of arguments 'name: value' is entered
    // into the configuration db. If the 'name' has not '.'
    // in it, prefix it automatically with 'cafe.'
    //
    for(int i = 1; i < argc; i++) {
        if(argv[i][strlen(argv[i])-1] == ':') {
            argv[i][strlen(argv[i])-1] = '\0';

            std::string key(argv[i]);
            if(key.find('.') == string::npos && key.find("cafe.", 0) != 0) {
                key = "cafe." + key;
            }
            config.set(key, argv[i+1]);
            i++;
        } else {
            std::cerr << "Unknown argument: " << argv[i] << std::endl;
        }
    }

    Config::resolve();

    if ( ! xAOD::Init().isSuccess() ) {
        throw std::runtime_error("Cannot initialise xAOD access !");
    }
    //xAOD::TReturnCode::enableFailure();
    gROOT->Macro("$ROOTCOREDIR/scripts/load_packages.C");


    try {
        RunController c("cafe");
        c.Run();
    } catch(std::exception& ex) {
        std::cerr << "cafe: caught exception during running:  " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch(...) {
        std::cerr << "cafe: caught unknown exception during running" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
