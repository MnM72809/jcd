cmake_minimum_required(VERSION 3.10)
project(jcd C)

set(CMAKE_C_STANDARD 11)

add_executable(jcd src/main.c
                   src/obj_manipulation.c
			   	   src/algorithms.c
			       src/init.c)

install(TARGETS jcd DESTINATION bin)

# Must be at the very end of CMakeLists.txt
set(CPACK_PACKAGE_NAME "jcd")
set(CPACK_PACKAGE_VENDOR "moriaan")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "moriaan.michiels@gmail.com") 
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_GENERATOR "DEB;RPM;TGZ") # Tells CPack what to build
include(CPack)
