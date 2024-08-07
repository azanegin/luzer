#ifndef LUZER_H_
#define LUZER_H_

#ifdef __cplusplus
extern "C" {
#endif
lua_State *get_global_lua_state(void);
int luaL_mutate(lua_State *L);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // LUZER_H_
