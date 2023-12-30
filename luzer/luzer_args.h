#ifndef LUZER_ARGS_H_
#define LUZER_ARGS_H_

int get_fuzz_args(lua_State *L, char ***argv, int *argc);

char *corpus_path;

#endif  // LUZER_ARGS_H_

