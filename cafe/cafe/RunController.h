#ifndef CAFE_RUNCONTROLLER_H__
#define CAFE_RUNCONTROLLER_H__

#include <string>
#include <set>
#include <vector> 
#include "cafe/Controller.h"

class TTree;
class TFile;
namespace xAOD {
    class TEvent;
    class TStore;
}

namespace cafe {

    class Processor;

    /**
     * A special version of a Controller.
     *
     * This opens the input files and loops over the events.
     *
     * Input specifications can be of the form:
     *
     * -  file:pathname/to/file
     * -  listfile:pathname/to/listfile
     * -  sam:datadefinition
     *
     * and any other root prefix (rootd:, rootk:, http:, rfio:, dcache:)
     *
     * If '.Events' is greater than 0, only that many events will be processed.
     *
     * Configuration options:
     *
     * - .Input:     InputSpecification (see above) [default: file:input.root]
     * - .Events:    MaxEvents [default: 0, i.e. no limit]
     * - .Files:     MaxFiles  [default: 0, i.e. no limit]
     * - .Skip:      NumEvents [default: 0]
     * - .SkipFiles: NumFiles [default: 0]
     * - .Progress:  Number (print progress after 'Number' events)
     * - .StoreDump: Number (print transient store after 'Number' events)
     * - .xAODOutput: name for the xAOD output if any [default: none]
     * - .Containers: list of containers to copy from input to output xAOD
     * - .WriteAllEvents: write all events ;-)
     *
     * @see Controller 
     *
     * \ingroup cafe
     */
    class RunController : public Controller {
        public:
            RunController(const char *name);
            ~RunController();

            void loop(xAOD::TEvent& event, xAOD::TStore& store);
            bool Run(unsigned int max_events = 0);
        private:
            void process_mem_usage(long& vm_usage, long& resident_set);
            void setTreeCaches(TFile* tfile);
	    TFile* OpenWithRetries(const std::string& name);

            std::string              _input;
            unsigned int             _num_events;
            unsigned int             _max_events;   
            unsigned int             _max_files;
            unsigned int             _num_files;
            unsigned int             _progress;
            unsigned int             _storeDump;
            int                      _skip;
            int                      _skip_files;
            TFile *                  _xAODoutfile;
            std::vector<std::string> _containers;
            bool                     _WriteAllEvents;
            unsigned int             _AccessMode;
            int                      _TTreeCacheSize;
        public:
            ClassDef(RunController, 0);
    };
}

#endif // CAFE_RUNCONTROLLER_HPP__
