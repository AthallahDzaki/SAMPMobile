// ----------------------------------------------------------------------
// File: protect.cpp
// Date: 04.08.2019
// Comments: protect against ip spoofing and library double injecting
// ----------------------------------------------------------------------
#include "protect.h"
// ----------------------------------------------------------------------
//constexpr auto unique_library_path = cryptor::create("libsamp.so", 11);
// ----------------------------------------------------------------------

CProtect::CProtect()
{
    this->library_handler = unique_library_handler;
    //this->ip_handler = unique_ip_handler;
}

CProtect::~CProtect()
{
    this->library_handler = nullptr;
    //this->ip_handler = nullptr;
}

// too dangerous due to this thing can hook
// ----------------------------------------------------------------------
/*bool CProtect::CheckModules()
{
    return this->library_handler(unique_library_path.decrypt());
}
bool CProtect::CheckHost()
{
    return this->ip_handler("127.0.0.1");
}*/
// ----------------------------------------------------------------------

unique_handler_t CProtect::GetUniqueHandler(/*const unsigned char handler_type*/)
{
    /*if(handler_type == 1)
    {
        return this->library_handler;
    }
    else if(handler_type == 2)
    {
        return this->ip_handler;
    }
    else
    {
        return nullptr;
    }*/
    return this->library_handler;
}