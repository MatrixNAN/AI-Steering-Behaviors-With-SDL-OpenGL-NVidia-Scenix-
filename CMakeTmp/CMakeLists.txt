PROJECT(DetermineSoundLibs)
        INCLUDE_DIRECTORIES(/usr/include/SDL /usr/include/SDL)
        ADD_EXECUTABLE(DetermineSoundLibs DetermineSoundLibs.c)
        TARGET_LINK_LIBRARIES(DetermineSoundLibs  "/usr/lib/libSDL_sound.so" "/usr/lib/libSDLmain.a" "/usr/lib/libSDL.so" "-lpthread")