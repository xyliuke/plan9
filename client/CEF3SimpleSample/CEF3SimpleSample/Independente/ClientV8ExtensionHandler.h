/************************************************************************************************
*   Copyright (c) 2013 Álan Crístoffer
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
*   of the Software, and to permit persons to whom the Software is furnished to do so,
*   subject to the following conditions:
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
*   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
*   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
*   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*   DEALINGS IN THE SOFTWARE.
************************************************************************************************/

#ifndef __CEF3SimpleSample__ClientV8ExtensionHandler__
#define __CEF3SimpleSample__ClientV8ExtensionHandler__

#include "include/cef_app.h"

struct CallbackItem
{
    int brower_id;
    CefRefPtr<CefV8Context> context;
    CefRefPtr<CefV8Value> callback;
};

struct ClientV8ExtensionHandler : public CefV8Handler {
    ClientV8ExtensionHandler(CefRefPtr<CefApp> app);

    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) OVERRIDE;

private:
    void callback(unsigned long long id, std::string result);
    void notify(std::string result);
    
private:
    CefRefPtr<CefApp> app;
    typedef std::map<unsigned long long, CallbackItem* > CallbackMap;
    CallbackMap callback_map;
    
    IMPLEMENT_REFCOUNTING(ClientV8ExtensionHandler);
};

#endif /* defined(__CEF3SimpleSample__ClientV8ExtensionHandler__) */
