
#include "cafe/Expander.h"
#include "cafe/Plugins.h"

namespace cafe {


    //
    // Search for plugins in:
    //
    // $ROOTSYS/etc/.rootrc
    // ~/.rootrc
    // .rootrc
    //
    // add 
    //   TEnv env("${PACKAGE}/scripts/rootrc");
    //
    //  for every package 
    //
    // Plugin.Expander:   ^sam:      SAMExpander  cafe "cafe::SAMExpander(const char *)"
    // +Plugin.Expander:  ^listfile: ListExpander cafe "cafe::ListExpander(const char *)"
    // +Plugin.Expander:  *          FileExpander cafe "cafe::FileExpander(const char *)"
    //

    Expander::Expander()
    {
    }

    Expander::~Expander()
    {
    }

    Expander *Expander::create(const std::string& url)
    {
        Plugins *plugins = Plugins::instance();
        return plugins->load<Expander>("Expander", url, url);
    }

}

ClassImp(cafe::Expander)
