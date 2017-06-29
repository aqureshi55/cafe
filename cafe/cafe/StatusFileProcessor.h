#ifndef caf_util_StatusFileProcessor_H_
#define caf_util_StatusFileProcessor_H_

#include "cafe/Processor.h"

#include <string>
#include <iostream>
#include <vector>

namespace cafe {

    class StatusFileProcessor : public cafe::Processor {
        public:
            StatusFileProcessor(const char *name);

            struct StatusInfo_t {
                StatusInfo_t(const std::string &n, const std::string &s, const int &nn):
                    name(n),status(s),nevt(nn) {;}

                void print(std::ostream &out) {
                    out << name << "  " << status << " " << nevt << std::endl;
                }

                std::string name;
                std::string status;
                int nevt;
            };//end struct StatusInfo_t


            //void begin(); //nothing interesting to do on begin
            void DumpStatus();

            void inputFileOpened(TFile *file);
            bool processEvent(xAOD::TEvent& event);
            void inputFileClosing(TFile *file);
            void finish();

        private:

            std::string _status_file_name;
            std::vector<StatusInfo_t> _status_info;

            StatusInfo_t _job_status;

        public:
            ClassDef(StatusFileProcessor,0);
    };
}

#endif // caf_util_StatusFileProcessor_H_
