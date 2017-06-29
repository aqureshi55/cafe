#ifndef CAFE_FUNCTION_H__
#define CAFE_FUNCTION_H__

#include <map>
#include "cafe/Processor.h"

namespace cafe {

    /**
     * A Processor encapsulating a normal C++ function.
     * The function's signature should be:
     *
     * - bool (*FUNC)(xAOD::TEvent& event)
     *
     * User functions that don't inherit from Processor
     * are encapsulated in an object of this type. A normal
     * user should never have to use this.
     *
     * \ingroup cafe
     */
    class Function : public Processor {
        public:
            typedef bool (*FUNC)(xAOD::TEvent&);

        public:
            Function(const char *name);
            Function(const std::string& name, FUNC func);

            // overrides Processor interface
            virtual bool processEvent(xAOD::TEvent &event);

            // Helper class to register a function
            class Register {
                public:
                    Register(const char *name, FUNC func);
            };

        private:
            FUNC _func;

            friend class Function::Register;

            typedef std::map<std::string,FUNC> Map;
            static Map& getMap();

        public:
            ClassDef(Function, 0);
    };
}

#define CAFE_FUNCTION(func) namespace {  cafe::Function::Register reg_##func(#func,func); }

#endif // CAFE_FUNCTION_HPP__
