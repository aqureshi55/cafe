
#include "cafe/FileExpander.h"

namespace cafe {

    FileExpander::FileExpander(const char *url)
        : _url(url)        
    {
    }

    std::string FileExpander::nextFile()
    {
        std::string result(_url);
        _url = "";
        return result;
    }

}

ClassImp(cafe::FileExpander)
