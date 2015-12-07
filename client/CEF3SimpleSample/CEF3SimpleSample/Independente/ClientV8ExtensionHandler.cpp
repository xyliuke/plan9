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

#include "ClientV8ExtensionHandler.h"
#include <iosfwd>
#include <string>

static unsigned long long uuid()
{
    static unsigned long long id = 0;
    return id ++;
}


ClientV8ExtensionHandler::ClientV8ExtensionHandler(CefRefPtr<CefApp> app)
{
    this->app = app;
}

bool ClientV8ExtensionHandler::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception)
{
    if ( name == "ChangeTextInJS" ) {
        if ( (arguments.size() == 1) && arguments[0]->IsString() ) {
            CefString           text   = arguments[0]->GetStringValue();
            CefRefPtr<CefFrame> frame  = CefV8Context::GetCurrentContext()->GetBrowser()->GetMainFrame();
            std::string         jscall = "ChangeText('";
            jscall += text;
            jscall += "');";
            frame->ExecuteJavaScript(jscall, frame->GetURL(), 0);
            /*
             * If you want your method to return a value, just use retval, like this:
             * retval = CefV8Value::CreateString("Hello World!");
             * you can use any CefV8Value, what means you can return arrays, objects or whatever you can create with CefV8Value::Create* methods
             */
            return true;
        }
    } else if (name == "__jscall__") {
        if (arguments.size() == 3 && arguments[0]->IsString()) {
            CefString method = arguments[0]->GetStringValue();
            printf("call method: %s", method.ToString().c_str());
            if (!arguments[1]->IsNull()) {
                CefString param = arguments[1]->GetStringValue();
                printf("param: %s", param.ToString().c_str());
            }
            
            if (!arguments[2]->IsNull()) {
                //有函数名、参数和callback
                CefRefPtr<CefV8Value> callback = arguments[2];
                
                int browser_id = CefV8Context::GetCurrentContext()->GetBrowser()->GetIdentifier();
                unsigned long long id = uuid();
                CallbackItem* item = new CallbackItem();
                item->brower_id = browser_id;
                item->callback = callback;
                item->context = CefV8Context::GetCurrentContext();
                callback_map.insert(std::make_pair(id, item));
                
                this->callback(id, "{\"ret\":\"callback from map\"}");
                
                this->notify("{\"ret\":\"notify from c++\"}");
            }
            printf("\n");
        }
    }

    return false;
}

void ClientV8ExtensionHandler::callback(unsigned long long id, std::string result)
{
    if (!callback_map.empty()) {
        CallbackMap::const_iterator it = callback_map.find(id);
        if (it != callback_map.end()) {
            CallbackItem* item = it->second;
            
            int brower_id = item->brower_id;
            CefRefPtr<CefV8Value> callback = item->callback;
            CefRefPtr<CefV8Context> context = item->context;
            
            context->Enter();
            
            CefV8ValueList arguments_;
            arguments_.push_back(CefV8Value::CreateString(result));
            callback->ExecuteFunction(NULL, arguments_);

            
            context->Exit();
            
            callback_map.erase(id);
            delete item;
        }
    }
}

void ClientV8ExtensionHandler::notify(std::string result)
{
    CefRefPtr<CefFrame> frame  = CefV8Context::GetCurrentContext()->GetBrowser()->GetMainFrame();
    std::string jscall = "window.app.__notify__('";
    jscall += result;
    jscall += "');";
    frame->ExecuteJavaScript(jscall, frame->GetURL(), 0);
}











