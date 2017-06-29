#include "cafe/ParseRun.h"

#include <ctype.h>
#include <stdexcept>
#include <iostream>
#include "cafe/Processor.h"

namespace cafe {

    ParseRun::ParseRun() 
    {}

    void ParseRun::init(const std::string& input)
    {
        _input = input;
        _ptr   = input.c_str();
        _ch    = *_ptr;
        _ident.clear();
    }

    bool ParseRun::next() 
    { 
        return (_ch = *++_ptr) != '\0'; 
    }

    void ParseRun::skip_ws()
    {
        while(isspace(_ch) && next()) {}
    }

    void ParseRun::ident()
    {
        _ident.clear();

        while(isalnum(_ch) || (_ch == '_') || (_ch == ':')) {
            _ident += _ch;
            next();
        }
    }

    cafe::Processor *ParseRun::def()
    {
        std::string class_name;
        std::string instance;

        if(isalpha(_ch) || (_ch == '(')) {
            if(_ch == '(') {
                next();
                ident();
                class_name = "Group";
                instance   = _ident;
                if(_ch != ')') {
                    error(") expected");
                } else {
                    next();
                }
            } else {
                ident();
                class_name = _ident;
                instance   = _ident;
                skip_ws();
                if(_ch == '(') {
                    next();
                    skip_ws();
                    ident();
                    instance = _ident;
                    skip_ws();
                    if(_ch != ')') { 
                        error(") expected");
                    }
                    next();
                }
            }
            return Processor::Create(class_name, instance);
        } else {
            error("Identifier expected");
            return 0;
        }
    }

    std::list<Processor*> ParseRun::parse(const std::string& input)
    {
        init(input);

        std::list<Processor*> result;

        skip_ws();
        while(Processor *p = def()) {
            result.push_back(p);
            // skip intermediate whitespace
            while(_ch == ',' || isspace(_ch)) { 
                next(); 
            }
            if(_ch == '\0') break;
        }
        return result;
    }

    void ParseRun::error(const char *msg)
    {
        std::cerr << "ParseRun: Error at " << _ptr - _input.c_str() << " : " << msg << std::endl;
        throw std::runtime_error("Parse Error in Run specification");
    }
}

