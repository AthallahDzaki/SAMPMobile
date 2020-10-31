// ----------------------------------------------------------------------
// File: protect.h
// Date: 04.08.2019
// Comments: protect against ip spoofing and library double injecting
// ----------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------
#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen
// ---------------------------------------------------------------------
#include "main.h"
// ----------------------------------------------------------------------
typedef bool(*unique_handler_t)(const char*);
// ----------------------------------------------------------------------

class CProtect
{
public:
    CProtect();
    ~CProtect();

    /*bool CheckModules();
    bool CheckHost();*/

    unique_handler_t GetUniqueHandler();

private:
    unique_handler_t library_handler;

protected:
    static bool unique_library_handler(const char *library)
    {
        Dl_info info;
        if(dladdr(__builtin_return_address(0), &info) != 0)
        {
            void *current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
            if(!current_library_addr)
                return false;

            //Log("%p | %p", current_library_addr, dlopen("libsamp.so", RTLD_NOW));

            if(dlopen(library, RTLD_NOW) != current_library_addr)
                return false;
        }
        return true;
    }
};
