#include "gpushwoosh.h"
#include "gideros.h"
#include <glog.h>

#include <android/log.h>

// some Lua helper functions
#ifndef abs_index
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif

static void luaL_newweaktable(lua_State *L, const char *mode)
{
	lua_newtable(L);			// create table for instance list
	lua_pushstring(L, mode);
	lua_setfield(L, -2, "__mode");	  // set as weak-value table
	lua_pushvalue(L, -1);             // duplicate table
	lua_setmetatable(L, -2);          // set itself as metatable
}

static void luaL_rawgetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_rawget(L, idx);
}

static const char *PUSH_RECEIVE = "pushReceive";
static const char *REGISTER = "register";
static const char *UNREGISTER = "unregister";
static const char *REGISTER_ERROR = "registerError";
static const char *UNREGISTER_ERROR = "unregisterError";

static void luaL_rawsetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

static char keyWeak = ' ';

class PushWoosh : public GEventDispatcherProxy
{
public:
	PushWoosh(lua_State *L) : L(L)
	{
		gpushwoosh_init();
		gpushwoosh_addCallback(callback_s, this);
	}

	~PushWoosh()
	{
		gpushwoosh_removeCallback(callback_s, this);
		gpushwoosh_cleanup();
	}

	void setId(const char* appId, const char* senderId)
	{
		gpushwoosh_setId(appId, senderId);
	}

	void startTrackingGeoPushes()
	{
		gpushwoosh_startTrackingGeoPushes();
	}

	void stopTrackingGeoPushes()
	{
		gpushwoosh_stopTrackingGeoPushes();
	}

	void unregister()
	{
		gpushwoosh_unregister();
	}

	const char* getCustomData()
	{
		return gpushwoosh_getCustomData();
	}

	void setMultiNotificationMode()
	{
		gpushwoosh_setMultiNotificationMode();
	}

	void setSimpleNotificationMode()
	{
		gpushwoosh_setSimpleNotificationMode();
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		static_cast<PushWoosh*>(udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		dispatchEvent(type, event);
	}

	void dispatchEvent(int type, void *event)
	{
		luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
		luaL_rawgetptr(L, -1, this);

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			return;
		}

		lua_getfield(L, -1, "dispatchEvent");

		lua_pushvalue(L, -2);

		lua_getglobal(L, "Event");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);

		switch (type)
		{
		case GPUSHWOOSH_PUSH_RECEIVE_EVENT:
			lua_pushstring(L, PUSH_RECEIVE);
			break;
		case GPUSHWOOSH_REGISTER_EVENT:
			lua_pushstring(L, REGISTER);
			break;
		case GPUSHWOOSH_UNREGISTER_EVENT:
			lua_pushstring(L, UNREGISTER);
			break;
		case GPUSHWOOSH_REGISTER_ERROR_EVENT:
			lua_pushstring(L, REGISTER_ERROR);
			break;
		case GPUSHWOOSH_UNREGISTER_ERROR_EVENT:
			lua_pushstring(L, UNREGISTER_ERROR);
			break;
		}

		lua_call(L, 1, 1);

		if (type == GPUSHWOOSH_PUSH_RECEIVE_EVENT)
		{
			gpushwoosh_PushReceiveEvent *event2 = (gpushwoosh_PushReceiveEvent*) event;

			lua_pushstring(L, event2->title);
			lua_setfield(L, -2, "title");
		}

		lua_call(L, 2, 0);

		lua_pop(L, 2);
	}

private:
	lua_State *L;
	bool initialized_;
};

static int destruct(lua_State* L)
{
	void *ptr =*(void**)lua_touserdata(L, 1);
	GReferenced* object = static_cast<GReferenced*>(ptr);
	PushWoosh *pushwoosh = static_cast<PushWoosh*>(object->proxy());

	pushwoosh->unref();

	return 0;
}

static PushWoosh *getInstance(lua_State *L, int index)
{
	GReferenced *object = static_cast<GReferenced*>(g_getInstance(L, "PushWoosh", index));
	PushWoosh *pushwoosh = static_cast<PushWoosh*>(object->proxy());

	return pushwoosh;
}

static int setId(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	const char *appId = lua_tostring(L, 2);
	const char *senderId = lua_tostring(L, 3);

	pushwoosh->setId(appId, senderId);

	return 0;
}

static int startTrackingGeoPushes(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	pushwoosh->startTrackingGeoPushes();

	return 0;
}

static int stopTrackingGeoPushes(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	pushwoosh->stopTrackingGeoPushes();

	return 0;
}

static int unregister(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	pushwoosh->unregister();

	return 0;
}

static int getCustomData(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	lua_pushstring(L, pushwoosh->getCustomData());

	return 0;
}

static int setMultiNotificationMode(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	pushwoosh->setMultiNotificationMode();

	return 0;
}

static int setSimpleNotificationMode(lua_State *L)
{
	PushWoosh *pushwoosh = getInstance(L, 1);

	pushwoosh->setSimpleNotificationMode();

	return 0;
}

static int loader(lua_State *L)
{
	const luaL_Reg functionList[] = {
		{"setId", setId},
		{"startTrackingGeoPushes", startTrackingGeoPushes},
		{"stopTrackingGeoPushes", stopTrackingGeoPushes},
		{"unregister", unregister},
		{"getCustomData", getCustomData},
		{"setMultiNotificationMode", setMultiNotificationMode},
		{"setSimpleNotificationMode", setSimpleNotificationMode},
		{NULL, NULL}
	};

    g_createClass(L, "PushWoosh", "EventDispatcher", NULL, destruct, functionList);

    // create a weak table in LUA_REGISTRYINDEX that can be accessed with the address of keyWeak
	luaL_newweaktable(L, "v");
	luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);

    lua_getglobal(L, "Event");
	lua_pushstring(L, PUSH_RECEIVE);
	lua_setfield(L, -2, "PUSH_RECEIVE");
	lua_pushstring(L, REGISTER);
	lua_setfield(L, -2, "REGISTER");
	lua_pushstring(L, UNREGISTER);
	lua_setfield(L, -2, "UNREGISTER");
	lua_pushstring(L, REGISTER_ERROR);
	lua_setfield(L, -2, "REGISTER_ERROR");
	lua_pushstring(L, UNREGISTER_ERROR);
	lua_setfield(L, -2, "UNREGISTER_ERROR");
	lua_pop(L, 1);

	PushWoosh *pushwoosh = new PushWoosh(L);
	g_pushInstance(L, "PushWoosh", pushwoosh->object());

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, pushwoosh);
	lua_pop(L, 1);

	lua_pushvalue(L, -1);
	lua_setglobal(L, "pushwoosh");

    return 1;
}

static void g_initializePlugin(lua_State *L)
{
    lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	lua_pushcfunction(L, loader);
	lua_setfield(L, -2, "pushwoosh");

	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State *L)
{

}

REGISTER_PLUGIN("PushWoosh", "2012.12")
