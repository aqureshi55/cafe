#ifndef CAFE_EXPANDER_H__
#define CAFE_EXPANDER_H__

#include <string>
#include "TObject.h"

namespace cafe {

    /**
     * Abstract base class for expanding input specifications.

     * Subclasses provide the actual functionality. These
     * are created via the TPluginManager of ROOT. 
     *
     * An input specification is basically an URL which is
     * interpreted by one of the expanders. Examples are:
     *
     *  - file:/path/to/file.root
     *  - sam:aSamDefinition
     *  - listfile:/path/to/file/with/filenames.list
     *  - rootk://server.fnal.gov/myfile.root
     *
     * @see FileExpander, FilelistExpander, SAMExpander
     *
     * \ingroup cafe
     */

    class Expander : public TObject {
        protected:
            Expander();
        public:
            virtual ~Expander();
            static Expander *create(const std::string& url);
            virtual std::string nextFile() = 0;
            ClassDef(Expander, 0);
    };
}


#endif // CAFE_EXPANDER_HPP__
