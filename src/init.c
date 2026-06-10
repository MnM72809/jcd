#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "init.h"

char *add_env_var(char *env_vars, const char *prefix, char *value, const char SUFFIX)
{
    size_t cur = env_vars ? strlen(env_vars) : 0;
    size_t add = strlen(prefix) + strlen(value ? value : "") + 1; // suffix
    char *tmp = realloc(env_vars, cur + add + 1);
    if (!tmp)
    {
        perror("realloc");
        free(env_vars);
        exit(1);
    }
    env_vars = tmp;
    snprintf(env_vars + cur, add + 1, "%s%s%c", prefix, value ? value : "", SUFFIX);
    return env_vars;
}

void init(int argc, char *argv[])
{
    char *alias = "j";
    char *env_vars = NULL;
    if (argc > 2)
    {
        int opt;
        while ((opt = getopt(argc, argv, "a:b:")) != -1)
        {
            switch (opt)
            {
            case 'a': // Alias
                alias = optarg;
                break;
            case 'b': // Back str
            {
                const char *prefix = "export JCD_BACK=\"";
                const char SUFFIX = '\"';
                env_vars = add_env_var(env_vars, prefix, optarg, SUFFIX);
                break;
            }
            default:
                fprintf(stderr, "Unknown option: %d\n", opt);
                exit(1);
            }
        }
    }

    printf("%s() {\n"
           "  if ! command -v jcd >/dev/null 2>&1; then\n"
           "    echo \"error: 'jcd' binary not found in PATH.\" >&2\n"
           "    return 1\n"
           "  fi\n"
           "%s\n"
           "  local target_dir\n"
           "  target_dir=$(jcd cd \"$@\")\n"
           "  if [ $? -eq 0 ] && [ -n \"$target_dir\" ]; then\n"
           "    builtin cd \"$target_dir\"\n"
           "  fi\n"
           "}\n",
           alias, env_vars ? env_vars : "");

    if (env_vars)
        free(env_vars);
}
