#include <stdio.h>

#include "init.h"


void init()
{
    printf("j() {\n"
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
           "}\n");
}

