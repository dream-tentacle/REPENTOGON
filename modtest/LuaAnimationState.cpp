#include <lua.hpp>

#include "IsaacRepentance.h"
#include "LuaCore.h"
#include "HookSystem.h"

static int Lua_EntityGetAnimationState(lua_State* L)
{
	Entity* entity = lua::GetUserdata<Entity*>(L, 1, lua::Metatables::ENTITY, "Entity");
	if (!entity->IsAnimationExist()) {
		lua_pushnil(L);
	}
	else {
		AnimationState** toLua = (AnimationState**)lua_newuserdata(L, sizeof(AnimationState*));
		*toLua = entity->GetAnimationState();
		luaL_setmetatable(L, lua::metatables::AnimationStateMT);
	}
	return 1;
}

static int Lua_EntityGetOverlayAnimationState(lua_State* L)
{
	Entity* entity = lua::GetUserdata<Entity*>(L, 1, lua::Metatables::ENTITY, "Entity");
	if (!entity->IsOverlayAnimationExist()) {
		lua_pushnil(L);
	}
	else {
		AnimationState** toLua = (AnimationState**)lua_newuserdata(L, sizeof(AnimationState*));
		*toLua = entity->GetOverlayAnimationState();
		luaL_setmetatable(L, lua::metatables::AnimationStateMT);
	}
	return 1;
}

static int Lua_AnimationStateGetNullFrame(lua_State* L)
{
	AnimationState* animationState = *lua::GetUserdata<AnimationState**>(L, 1, lua::metatables::AnimationStateMT);
	const char* nullLayerName = luaL_checkstring(L, 2);
	NullFrame* toLua = animationState->GetNullFrame(nullLayerName);
	if (toLua == nullptr) {
		lua_pushnil(L);
		return 1;
	}
	NullFrame** luaNullFrame = (NullFrame**)lua_newuserdata(L, sizeof(NullFrame*));
	*luaNullFrame = toLua;
	luaL_setmetatable(L, lua::metatables::NullFrameMT);
	/*luaNullFrame = animationData->GetNullFrame(nullLayerName);
	if (*luaNullFrame == NULL) {
		lua_pushnil(L);
	}
	else {
		luaL_setmetatable(L, NullFrameMT);
	}
	*/
	return 1;
}

static int Lua_AnimationStateIsEventTriggered(lua_State* L)
{
	AnimationState* animationState = *lua::GetUserdata<AnimationState**>(L, 1, lua::metatables::AnimationStateMT);
	const char* eventName = luaL_checkstring(L, 2);
	lua_pushboolean(L, animationState->IsEventTriggered(eventName));
	return 1;
}

static void RegisterAnimationState(lua_State* L) {
	lua::PushMetatable(L, lua::Metatables::ENTITY);
	lua_pushstring(L, "GetAnimationState");
	lua_pushcfunction(L, Lua_EntityGetAnimationState);
	lua_rawset(L, -3);
	lua_pop(L, 1);

	lua::PushMetatable(L, lua::Metatables::ENTITY);
	lua_pushstring(L, "GetOverlayAnimationState");
	lua_pushcfunction(L, Lua_EntityGetOverlayAnimationState);
	lua_rawset(L, -3);
	lua_pop(L, 1);

	luaL_newmetatable(L, lua::metatables::AnimationStateMT);
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_Reg funcs[] = {
		{ "GetNullFrame", Lua_AnimationStateGetNullFrame },
		{ "IsEventTriggered", Lua_AnimationStateIsEventTriggered},
		{ NULL, NULL }
	};

	luaL_setfuncs(L, funcs, 0);

	lua_pop(L, 1);
}

HOOK_METHOD(LuaEngine, RegisterClasses, () -> void) {
	super();
	lua_State* state = g_LuaEngine->_state;
	lua::LuaStackProtector protector(state);
	RegisterAnimationState(state);
}