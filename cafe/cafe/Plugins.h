#ifndef CAFE_PLUGINS_H__
#define CAFE_PLUGINS_H__

#include "TPluginManager.h"
#include "TROOT.h"

#include <stdexcept>

namespace cafe {

    /**
     * Interface to plugin handler. Loads all necessary
     * rootrc files from the package list.
     */

    class Plugins {
        public:
            static Plugins *instance();

            // Look up and load plugin with 'url', pass it 'arg'
            // as 'const char *' argument.
            template<class T>
                T *load(const std::string& type, const std::string& url, const std::string& arg);
        private:
            Plugins();
            ~Plugins();
            static Plugins *s_instance;
    };


    // Implementation
    template<class T>
        T *Plugins::load(const std::string& type, const std::string& url, const std::string& arg)
        {
            TPluginManager *mgr = gROOT->GetPluginManager();
            if(TPluginHandler *handler = mgr->FindHandler(type.c_str(), url.c_str())) {
                if(handler->LoadPlugin() == 0) {
                    return reinterpret_cast<T*>(handler->ExecPlugin(1, arg.c_str()));
                } else {
                    throw std::runtime_error("Plugin not found: " + type);
                }
            }
            return 0;
        }
}

#endif // CAFE_PLUGINS_HPP__
