#ifndef CAFE_TIMER_H_
#define CAFE_TIMER_H_

#include "TStopwatch.h"

#include "cafe/Controller.h"

namespace cafe {

    /**
     * Time the execution of children.
     */
    class Timer : public cafe::Controller {
        public:
            Timer(const char *name);
            ~Timer();
            bool processEvent(xAOD::TEvent& event);
        private:
            TStopwatch _timer;
        public:
            ClassDef(Timer,0 );
    };

}

#endif // CAFE_TIMER_HPP_
