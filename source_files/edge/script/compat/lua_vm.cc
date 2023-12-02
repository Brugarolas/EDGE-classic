
#include <algorithm>
#include "lua_compat.h"
#include "i_system.h"
#include "str_util.h"
#include "w_files.h"
#include "con_var.h"
#include "../lua_debugger.h"

// Enable Lua debugging
DEF_CVAR(lua_debug, "0", CVAR_ROM)

static void LUA_GetRequirePackPath(const char *name, std::string &out)
{
    std::string require_name(name);
    std::replace(require_name.begin(), require_name.end(), '.', '/');
    out = epi::STR_Format("scripts/lua/%s.lua", require_name.c_str());
}

static int LUA_PackLoader(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    std::string pack_name;
    LUA_GetRequirePackPath(name, pack_name);

    epi::file_c *file = W_OpenPackFile(pack_name);

    if (!file)
    {
        I_Warning("LUA: %s.lua: NOT FOUND\n", name);
        return 0;
    }

    // TODO: Fix me ReadText is copying string on return, and should be taking a reference
    std::string source = file->ReadText();

    delete file;

    int result = luaL_dostring(L, source.c_str());
    if (result != LUA_OK)
    {
        I_Warning("LUA: %s.lua: %s\n", name, lua_tostring(L, -1));
        lua_pop(L, 1);
        return 0;
    }

    return 1;
}

static int LUA_PackSearcher(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    std::string pack_name;
    LUA_GetRequirePackPath(name, pack_name);

    if (W_CheckPackForName(pack_name) == -1)
    {
        I_Warning("LUA: Unable to load file %s", pack_name.c_str());
        return 0;
    }

    lua_pushcfunction(L, LUA_PackLoader);
    lua_pushstring(L, name);
    return 2;
}

static int LUA_MsgHandler(lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL)
    {                                            /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)      /* that produces a string? */
            return 1;                            /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1); /* append a standard traceback */
    return 1;                     /* return the traceback */
}

void LUA_DoString(lua_State *L, const char *filename, const char *source)
{
    int top = lua_gettop(L);
    int ret = luaL_loadbuffer(L, source, strlen(source), (std::string("@") + filename).c_str());

    if (ret != LUA_OK)
    {
        I_Warning("LUA: Error compiling %s\n", filename ? filename : "???");
        I_Warning("LUA: %s\n", lua_tostring(L, -1));
        lua_settop(L, top);
        return;
    }

    int base = lua_gettop(L);             // function index
    lua_pushcfunction(L, LUA_MsgHandler); // push message handler */
    lua_insert(L, base);                  // put it under function and args */

    ret = lua_pcall(L, 0, 0, base);

    if (ret != LUA_OK)
    {
        std::string error(lua_tostring(L, -1));
        std::replace(error.begin(), error.end(), '\t', '>');
        std::vector<std::string> lines = epi::STR_SepStringVector(error, '\n');

        I_Warning("LUA: Error in %s\n", filename ? filename : "???");
        for (auto &line : lines)
        {
            I_Warning("LUA: %s\n", line.c_str());
        }
    }

    lua_settop(L, top);
}

void LUA_DoFile(lua_State *L, const std::string &name)
{
    epi::file_c *file = W_OpenPackFile(name);

    if (file)
    {
        std::string source = file->ReadText();
        int         top    = lua_gettop(L);
        int         ret    = luaL_dostring(L, source.c_str());
        if (ret != LUA_OK)
        {
            I_Warning("LUA: %s\n", lua_tostring(L, -1));
        }

        lua_settop(L, top);
        delete file;
    }
    else
    {
        I_Warning("LUA: LUA_DoFile unable to open file %s\n", name.c_str());
    }
}

// NOP dbg() for when debugger is disabled and someone has left some breakpoints in code
static bool dbg_nop_warn = false;
static int  LUA_DbgNOP(lua_State *L)
{
    if (!dbg_nop_warn)
    {
        dbg_nop_warn = true;
        I_Warning("LUA: dbg() called without lua_debug being set.  Please check that a stray dbg call didn't get left "
                  "in source.");
    }
    return 0;
}

void LUA_CallGlobalFunction(lua_State *L, const char *function_name)
{
    int top = lua_gettop(L);
    lua_getglobal(L, function_name);
    int status = 0;
    if (lua_debug.d)
    {
        status = dbg_pcall(L, 0, 0, 0);
    }
    else
    {
        int base = lua_gettop(L);             // function index
        lua_pushcfunction(L, LUA_MsgHandler); // push message handler */
        lua_insert(L, base);                  // put it under function and args */

        status = lua_pcall(L, 0, 0, base);
    }

    if (status != LUA_OK)
    {
        std::string error(lua_tostring(L, -1));
        std::replace(error.begin(), error.end(), '\t', '>');
        std::vector<std::string> lines = epi::STR_SepStringVector(error, '\n');

        I_Warning("LUA: Error running global function %s\n", function_name);
        for (auto &line : lines)
        {
            I_Warning("LUA: %s\n", line.c_str());
        }
    }

    lua_settop(L, top);
}

static int LUA_Sandbox_Warning(lua_State *L)
{
    const char *function_name = luaL_checkstring(L, lua_upvalueindex(1));

    I_Warning("LUA: Called sandboxed function %s\n", function_name);

    return 0;
}

static void LUA_Sandbox_Module(lua_State *L, const char *module_name, const char **functions)
{
    int i = 0;
    lua_getglobal(L, module_name);
    while (const char *function_name = functions[i++])
    {
        lua_pushfstring(L, "%s.%s", module_name, function_name);
        lua_pushcclosure(L, LUA_Sandbox_Warning, 1);
        lua_setfield(L, -2, function_name);
    }
    lua_pop(L, 1);
}

static void LUA_Sandbox(lua_State *L)
{
    // clear out search path and loadlib
    lua_getglobal(L, "package");
    lua_pushnil(L);
    lua_setfield(L, -2, "loadlib");
    lua_pushnil(L);
    lua_setfield(L, -2, "searchpath");
    // pop package off stack
    lua_pop(L, 1);

    const char *os_functions[] = {"execute", "exit", "getenv", "remove", "rename", "setlocale", "tmpname", NULL};
    LUA_Sandbox_Module(L, "os", os_functions);
}

lua_State *LUA_CreateVM()
{
    // we could specify a lua allocator, which would be a good idea to hook up to a debug allocator
    // library for tracing l = lua_newstate(lua_Alloc alloc, nullptr);

    lua_State *L = luaL_newstate();

    /*
    ** these libs are loaded by lua.c and are readily available to any Lua
    ** program
    */
    const luaL_Reg loadedlibs[] = {{LUA_GNAME, luaopen_base},
                                   {LUA_LOADLIBNAME, luaopen_package},
                                   {LUA_OSLIBNAME, luaopen_os},
                                   {LUA_IOLIBNAME, luaopen_io},
                                   {LUA_DBLIBNAME, luaopen_debug},
                                   {LUA_COLIBNAME, luaopen_coroutine},
                                   {LUA_TABLIBNAME, luaopen_table},
                                   {LUA_STRLIBNAME, luaopen_string},
                                   {LUA_MATHLIBNAME, luaopen_math},
                                   {LUA_UTF8LIBNAME, luaopen_utf8},
                                   {NULL, NULL}};

    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++)
    {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1); /* remove lib */
    }

    // replace searchers with only preload and custom searcher
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");
    lua_newtable(L);
    lua_geti(L, -2, 1);
    lua_seti(L, -2, 1);
    lua_pushcfunction(L, LUA_PackSearcher);
    lua_seti(L, -2, 2);
    lua_setfield(L, -3, "searchers");
    // pop package and searchers off stack
    lua_pop(L, 2);

    LUA_Sandbox(L);

    if (lua_debug.d)
    {
        dbg_setup_default(L);
    }
    else
    {
        lua_pushcfunction(L, LUA_DbgNOP);
        lua_setglobal(L, "dbg");
    }

    SYS_ASSERT(!lua_gettop(L));

    return L;
}
