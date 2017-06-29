
#include "cafe/StatusFileProcessor.h"
#include "cafe/Config.h"
#include "xAODRootAccess/TEvent.h"
#include "TFile.h"

#include <fstream>

using cafe::StatusFileProcessor;

StatusFileProcessor::StatusFileProcessor(const char *name)
    : cafe::Processor(name), _job_status("jobsummary","running",0)
{

    cafe::Config  config(name);
    _status_file_name =config.get("fname","jobstatus.txt");

}//end StatusFileProcessor::StatusFileProcessor(const char *name)

void StatusFileProcessor::DumpStatus() {

    std::ofstream ofile(_status_file_name.c_str());

    int nfiles=_status_info.size();
    for (int ifile=0; ifile<nfiles; ++ifile)
        _status_info[ifile].print(ofile);

    _job_status.print(ofile);

}//end StatusFileProcessor::DumpStatus()

void StatusFileProcessor::inputFileOpened(TFile *file) {
    std::string fname(file->GetName());
    StatusInfo_t file_status(fname,"open",0);
    _status_info.push_back(file_status);

    DumpStatus();
}//end StatusFileProcessor::inputFileOpened(TFile *file)

bool StatusFileProcessor::processEvent(xAOD::TEvent& event) {
    StatusInfo_t &status=_status_info.back();

    ++status.nevt;    
    ++_job_status.nevt;

    DumpStatus();

    return true;
}//end StatusFileProcessor::processEvent(xAOD::TEvent& event)

void StatusFileProcessor::inputFileClosing(TFile *file) {

    StatusInfo_t &status=_status_info.back();
    if (file->GetName() != status.name)
    {
        err() << name() << "::inputFileClosing() -- "
            << "closing file named " << file->GetName()
            << ", but expected file name " << status.name 
            << std::endl;
    }//end if file name mismatch

    status.status="closed";

    DumpStatus();
}//end StatusFileProcessor::inputFileClosing(TFile *file)

void StatusFileProcessor::finish()
{

    _job_status.status="finished";
    DumpStatus();
}

ClassImp(StatusFileProcessor)

