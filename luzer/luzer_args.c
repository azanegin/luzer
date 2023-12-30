/*
 * SPDX-License-Identifier: ISC
 *
 * Copyright 2022-2023, Sergey Bronnikov
 */

#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "luzer_args.h"
#include "macros.h"

#define ENV_NOT_USE_CLI_ARGS "LUZER_NOT_USE_CLI_ARGS_FOR_LF"

#define LUA_CORPUS_FLAG "-corpus"

/* Structure for convenient argument parsing. */
struct Luzer_args {
	char **argv;
	int argc;
};

NO_SANITIZE static bool
is_flag_in_args(struct Luzer_args *f_args, const char *key) {
	if (!f_args || !f_args->argv || f_args->argc <= 0)
		return false;
	char search_flag[strlen(key) + 3];
	snprintf(search_flag, strlen(key) + 3, "-%s=", key);
	for (int i = 0; i <= f_args->argc; i++) {
		if (strncmp(f_args->argv[i], search_flag, strlen(search_flag)) == 0)
			return true;
	}
	return false;
}

NO_SANITIZE static int
get_args_from_cli(lua_State *L, struct Luzer_args *cli_args) {
	lua_getglobal(L, "arg");

	cli_args->argv = malloc(1 * sizeof(char*));
	if (!cli_args->argv)
		return -1;

	lua_pushnil(L);

        char* not_use_cli_args = getenv(ENV_NOT_USE_CLI_ARGS);
	cli_args->argc = 0;
	while (lua_next(L, -2) != 0) {
		const char *value = lua_tostring(L, -1);
		const int key = lua_tointeger(L, -2);
		lua_pop(L, 1);
                if (key < 0)
                        continue;

                const char *arg = strdup(value);
                if (!arg)
                        return -1;

                if (key > 0 && (not_use_cli_args == NULL || !strncmp(not_use_cli_args, "0", 1))) {
			cli_args->argc++;
                        char **argvp = realloc(cli_args->argv, sizeof(char*) * (cli_args->argc + 1));
                        if (argvp == NULL)
                                return -1;
                        cli_args->argv = argvp;
                        cli_args->argv[cli_args->argc] = (char*)arg;
                } else {
                        cli_args->argv[0] = (char*)arg;
                }
	}
	lua_pop(L, 1);
	return 0;
}

NO_SANITIZE static int
get_args_from_table(lua_State *L, struct Luzer_args *table_args, struct Luzer_args *cli_args) {
        if (lua_istable(L, -1) == 0)
		return -2;

	lua_pushnil(L);

	/* Processing a table with options. */
	table_args->argc = 0;
        while (lua_next(L, -2) != 0) {
		const char *key = lua_tostring(L, -2);
		const char *value = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (is_flag_in_args(cli_args, key))
			continue;

		if (table_args->argc > 0) {
			char **argvp = realloc(table_args->argv, sizeof(char*) * (table_args->argc + 1));
			if (argvp == NULL)
				return -1;
			table_args->argv = argvp;
		} else {
			table_args->argv = malloc(1 * sizeof(char*));
			if (!table_args->argv)
				return -1;
		}
		size_t arg_len = strlen(key) + strlen(value) + 3;
		char *arg = calloc(arg_len, sizeof(char));
		if (!arg)
			return -1;
		snprintf(arg, arg_len, "-%s=%s", key, value);
		table_args->argv[table_args->argc] = arg;
		table_args->argc++;
	}
	lua_pop(L, 1);
	return 0;
}

char *corpus_path;

NO_SANITIZE static int
merge_args(struct Luzer_args *cli_args, struct Luzer_args *table_args, struct Luzer_args *total_args) {
	total_args->argc = (cli_args->argc + table_args->argc + 1);
	total_args->argv = malloc(sizeof(char*) * (total_args->argc + 1));
	if (!cli_args->argv)
		return -1;

	/* Program name on zero index. */
	total_args->argv[0] = cli_args->argv[0];

	int cur_pos_arg = 1;
	char *corpus_path = NULL;
	for (int i = 0; i < table_args->argc; i++) {
		if (strncmp(table_args->argv[i], LUA_CORPUS_FLAG, strlen(LUA_CORPUS_FLAG)) == 0) {
			int corpus_path_len = strlen(table_args->argv[i]) - strlen(LUA_CORPUS_FLAG);
			corpus_path = malloc(corpus_path_len * sizeof(char*));
                        if (!corpus_path)
                                return -1;
			memcpy(corpus_path, &table_args->argv[i][strlen(LUA_CORPUS_FLAG) + 1], corpus_path_len);
			free(table_args->argv[i]);
                        table_args->argv[i] = NULL;
		} else {
			total_args->argv[cur_pos_arg] = table_args->argv[i];
			cur_pos_arg++;
		}
	}
	for (int i = 1; i <= cli_args->argc; i++) {
		total_args->argv[cur_pos_arg] = cli_args->argv[i];
		cur_pos_arg++;
	}
	if (corpus_path) {
		total_args->argv[cur_pos_arg] = corpus_path;
		cur_pos_arg++;
	}
	total_args->argv[total_args->argc] = NULL;

        if (table_args->argv)
	        free(table_args->argv);

        if (cli_args->argv)
	        free(cli_args->argv);

	return 0;
}

NO_SANITIZE static void 
free_args(struct Luzer_args args) {
        if (!args.argv)
                return;
        for (int i = 0; i < args.argc; i++) {
                if (args.argv[i])
                        free(args.argv[i]);
        }
        free(args.argv);
}

NO_SANITIZE int
get_fuzz_args(lua_State *L, char ***argv, int *argc) {
        struct Luzer_args total_args = { .argv = NULL, .argc = 0};
        struct Luzer_args cli_args = { .argv = NULL, .argc = 0 };
	struct Luzer_args table_args = { .argv = NULL, .argc = 0 };

        int result = -1;
        result = get_args_from_cli(L, &cli_args);
        if (result != 0) {
                free_args(cli_args);
                luaL_error(L, "failed parsing fuzz args. not enough memory");
        }

	/* If flag in cli and lua is duplicated, then flag from lua is ignored. */
	result = get_args_from_table(L, &table_args, &cli_args);
        if (result != 0) {
                free_args(table_args);
                free_args(cli_args);
                if (result == -2)
                        luaL_error(L, "failed parsing fuzz args. last argument is not a table");
                luaL_error(L, "failed parsing fuzz args. not enough memory");
        }

	result = merge_args(&cli_args, &table_args, &total_args);
        if (result != 0) {
                free_args(table_args);
                free_args(cli_args);
                free_args(total_args);
                luaL_error(L, "failed parsing fuzz args. not enough memory");
        }

        *argv = total_args.argv;
        *argc = total_args.argc;

#ifdef DEBUG
	char **p = *argv;
	while(*p++) {
		if (*p)
			DEBUG_PRINT("libFuzzer arg - '%s'\n", *p);
	}
#endif /* DEBUG */

        return 0;
}
