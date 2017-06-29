#ifndef PROCESSOR_H__
#define PROCESSOR_H__

#include <string>
#include <iosfwd>

#include "TObject.h"


class TFile;
class TDirectory;
class TPluginManager;
namespace xAOD {
    class TEvent;
}

namespace cafe {
    /**
     * The base class of all Processor's. Defines the
     * interface and provides default implementations
     * for all of them.
     *
     * A user should derive from this class and override
     * the methods she is interested in. Typically this
     * is at least processEvent().
     *
     * The derived class _must_ have a constructor that
     * takes a 'const char *' as an argument.
     *
     * \ingroup cafe
     */
    class Processor {
        public:
            Processor(const char *name);
            virtual ~Processor();

        public:
            // @name Event Processing

            //@{
            /// Called at beginning of processing.
            virtual void begin();

            /// Called at end of processing.
            virtual void finish();

            /// Called for every new input file that was opened.
            virtual void inputFileOpened(TFile *file);

            /// Called for every input file that is about to be closed.
            virtual void inputFileClosing(TFile *file);

            /// Called for every event.
            virtual bool processEvent(xAOD::TEvent& event);

            /// Increment the internal event counter (used by Controller only).      
            void         incEventCount();

            /// Number of events this Processor has seen.
            int          eventCount() const;

            //@}

            // @name Utility functions

            //@{

            /// The name of the Processor as passed to the constructor.
            std::string  name() const;

            /// The full name with all parents included. This is only
            /// valid once the whole Processor chain has been constructed.
            std::string  fullName() const;

            /// Return a stream to print normal output messages to (cout by default).
            std::ostream& out();

            /// Return a stream to print informational messages to (cout by default).
            std::ostream& info();

            /// Return a stream to print warning messages to (cout by default).
            std::ostream& warn();

            /// Return a stream to print error messages (cerr by default).
            std::ostream& err();

            /// Return debug level, 0 = no debug output, the higher the more verbose...
            unsigned int debug() const;

            /// Set the debug level. Usually done by controller.
            virtual void setDebug(unsigned int level);

            /// Get the current directory.
            TDirectory *getDirectory() const;

            /// Get the parent processor or NULL if top level Processor.
            Processor  *getParent() const;


            /// Try to Resolve 'name' in all configurations starting from this
            /// Processor. Try parent if 'recurse' is true.
            std::string resolve(const std::string& name, bool recurse = true) const;

            /// Replace every occurence in 'input' of the form %{name} by the result 
            /// of calling resolve(name).
            std::string replace(const std::string& input, bool recurse = true) const;

            /// Flag the xAOD TEvent as to be written, the flags is propagated 
            /// to parents where the root parent should actually write the event
            void write_xAOD_event();

            //@}

            // @name Creation

            //@{
            /**
             * Create a new Processor from the class name. Pass 'instance' as the
             * new name of the object.
             */
            static Processor* Create(const std::string& className, const std::string& instance = "");

            //@}

        protected:

            Processor();
            void        setDirectory(TDirectory *dir);
            void        setParent(Processor *parent);
            static void setParent(Processor* child, Processor *parent) {child->setParent(parent);}
            bool         _writexAOD;

        private:
            unsigned int _eventCount;
            unsigned int _debug;
            std::string  _name;
            mutable TDirectory   *_directory;
            Processor            *_parent;

        public:
            ClassDef(Processor, 0);
    };

}

#endif // PROCESSOR_HPP__
