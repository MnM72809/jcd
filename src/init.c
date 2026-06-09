#include <stdio.h>

#include "init.h"


void init()
{
    printf("j() {\n"
           "  local target_dir\n"
           "  target_dir=$(jcd cd \"$@\")\n"
           "  if [ $? -eq 0 ] && [ -n \"$target_dir\" ]; then\n"
           "    builtin cd \"$target_dir\"\n"
           "  fi\n"
           "}\n");
}
