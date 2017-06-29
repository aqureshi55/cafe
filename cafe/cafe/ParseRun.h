#ifndef PARSE_RUN_H__
#define PARSE_RUN_H__

#include <list>
#include <string>

namespace cafe {

    class Processor;

    /**
     * A helper class to parse the .Run: specification used by
     * Controller's. 
     *
     * This is used internally to turn a list of Processore 
     * names into an object tree.
     *
     * Run Syntax:
     *
     * RUN = 
     *     CLASS(INSTANCE)
     *   | CLASS   (implies INSTANCE=CLASS#UUID)
     *   | (INSTANCE) (implies CLASS=Group)
     *   | RUN[,RUN]*
     *
     * CLASS    = IDENT  (can be class or function name)
     * INSTANCE = IDENT
     *
     * \ingroup cafe
     */
    class ParseRun {
        public:
            ParseRun();
            std::list<Processor*> parse(const std::string& input);
        private:
            void init(const std::string& input);
            bool next();
            void skip_ws();

            void ident();
            cafe::Processor *def();

            void error(const char *msg);
        private:
            std::string _input;		// string to be parsed
            const char *_ptr;		// pointer to current input character in input
            char        _ch;  		// the current input character
            std::string _ident;
    };
}

#endif // PARSE_RUN_HPP__
