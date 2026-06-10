#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "init.h"

void init(int argc, char *argv[])
{
    char *alias = "j";
    if (argc > 2)
    {
        int opt;
        char *back = "";
        while ((opt = getopt(argc, argv, "a:")) != -1)
        {
            switch (opt)
            {
            case 'a': // Alias
                alias = optarg;
                break;
            default:
                fprintf(stderr, "Unknown option: %s\n", optarg);
                exit(1);
            }
        }
    }

    printf("%s() {\n"
           "  if ! command -v jcd >/dev/null 2>&1; then\n"
           "    echo \"error: 'jcd' binary not found in PATH.\" >&2\n"
           "    return 1\n"
           "  fi\n"
           "\n"
           "  local target_dir\n"
           "  target_dir=$(jcd cd \"$@\")\n"
           "  if [ $? -eq 0 ] && [ -n \"$target_dir\" ]; then\n"
           "    builtin cd \"$target_dir\"\n"
           "  fi\n"
           "}\n",
           alias);
}
