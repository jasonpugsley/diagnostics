// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "sosplugin.h"
#include <dlfcn.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <limits.h>

class setsostidCommand : public lldb::SBCommandPluginInterface
{
public:
    setsostidCommand()
    {
    }

    virtual bool
        DoExecute(lldb::SBDebugger debugger,
        char** arguments,
        lldb::SBCommandReturnObject &result)
    {
        result.SetStatus(lldb::eReturnStatusSuccessFinishResult);

        if (arguments == nullptr || arguments[0] == nullptr)
        {
            if (g_currentThreadSystemId == (ULONG)-1 || g_currentThreadIndex == (ULONG)-1)
            {
                result.Printf("sos OS tid not mapped\n");
            }
            else {
                result.Printf("sos OS tid 0x%x mapped to lldb thread index %d\n",
                    g_currentThreadSystemId, g_currentThreadIndex);
            }
        }
        else if (strcmp(arguments[0], "-clear") == 0) {
            g_currentThreadIndex = (ULONG)-1;
            g_currentThreadSystemId = (ULONG)-1;
            result.Printf("Cleared sos OS tid/index\n");
        }
        else if (arguments[1] == nullptr)
        {
            result.Printf("Need thread index parameter that maps to the OS tid. setsostid <tid> <index>\n");
        }
        else
        {
            ULONG tid = strtoul(arguments[0], nullptr, 16);
            g_currentThreadSystemId = tid;

            ULONG index = strtoul(arguments[1], nullptr, 16);
            g_currentThreadIndex = index;

            result.Printf("Mapped sos OS tid 0x%x to lldb thread index %d\n", tid, index);
        }
        return result.Succeeded();
    }
};

bool
setsostidCommandInitialize(lldb::SBDebugger debugger)
{
    lldb::SBCommandInterpreter interpreter = debugger.GetCommandInterpreter();
    lldb::SBCommand command = interpreter.AddCommand("setsostid", new setsostidCommand(), "Set the current os tid/thread index instead of using the one lldb provides. setsostid <tid> <index>");
    return true;
}
