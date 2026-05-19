//========= Resonance Team ============//
//
// Make the engine think we're loading 
// GameUI.dll, when we're really just 
// grabbing from the client.
//
//=====================================//

#include "IGameUI.h"
#include "IGameConsole.h"
#include "IVGuiModule.h"
#include "IVGuiModuleLoader.h"
#include "../game/gamepadui/igamepadui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


static CreateInterfaceFn ClientFactory()
{
    if (CommandLine()->FindParm("-gamepadui"))
    {
        static CSysModule* n = Sys_LoadModule("gamepadui");
        if (!n)
        {
            // If we don't load the client last, the game will outright crash.
            // This is because the client has our new interface, and not passing
            // an interface will outright crash the game because no gameui_interface_version is found.
            n = Sys_LoadModule("gamepadui");
            n = Sys_LoadModule("client", SYS_NOLOAD);
        }

        static CreateInterfaceFn fm = Sys_GetFactory(n);
        return fm;
    }
    else
    {
        // Fallback to the clients version of the interface from Alien Swarm.
        // This is dubbed old gamepadui
        static CSysModule* m = Sys_LoadModule("client", SYS_NOLOAD);
        static CreateInterfaceFn fn = Sys_GetFactory(m);
        return fn;
    }
}

#define INTERFACE_REDIRECT_TO_CLIENT(inter) \
static void* __##inter##_interface() { return ClientFactory()(inter, nullptr); } \
static InterfaceReg __g_Creat##inter##_reg(__##inter##_interface, inter);

INTERFACE_REDIRECT_TO_CLIENT(GAMEUI_INTERFACE_VERSION)
INTERFACE_REDIRECT_TO_CLIENT(GAMECONSOLE_INTERFACE_VERSION)
INTERFACE_REDIRECT_TO_CLIENT(VGUIMODULE_INTERFACE_VERSION)
INTERFACE_REDIRECT_TO_CLIENT(VGUIMODULELOADER_INTERFACE_VERSION)