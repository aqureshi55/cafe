#ifndef CAFE_FILELISTEXPANDER_H__
#define CAFE_FILELISTEXPANDER_H__

#include <string>
#include <iostream>
#include <fstream>

#include "cafe/Expander.h"

namespace cafe {

    /**
     * Expand a text file containg a list of file names.
     * Used internally by cafe.
     *
     * \ingroup cafe
     */

    class FilelistExpander : public Expander {
        public:
            FilelistExpander(const char *url);
            virtual std::string nextFile();
        private:
            std::ifstream _file;

            ClassDef(FilelistExpander, 0);
    };
}


#endif // CAFE_FILELISTEXPANDER_HPP__
