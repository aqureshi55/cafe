
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "cafe/FilelistExpander.h"

namespace cafe {

    FilelistExpander::FilelistExpander(const char *url)
    {
        const char *ptr = strchr(url, ':') + 1;
        _file.open(ptr);
	if(!_file) {
	    std::string msg("FilelistExpander: cannot find ");
	    msg += url;
	    throw std::runtime_error(msg);
	}
    }

    std::string FilelistExpander::nextFile()
    {
        std::string next;
        if(_file && _file >> next) {
            return next;
        }
        return "";
    }

}

ClassImp(cafe::FilelistExpander)

