cmake_minimum_required(VERSION 3.10.2)
project("madamesh")

# Recherche de libsodium (doit être compilée pour Android ou incluse en tant que prebuilt)
# Pour ce prototype, on se concentre sur le lien entre le moteur et JNI.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp
             mesh_engine.cpp )

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
