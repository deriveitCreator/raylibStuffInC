emsdk activate latest
emcc -c raylib/src/rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raylib/src/rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raylib/src/rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raylib/src/rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raylib/src/rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raylib/src/utils.c -Os -Wall -DPLATFORM_WEB
emcc -c raylib/src/raudio.c -Os -Wall -DPLATFORM_WEB
emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o
emcc -s EXPORTED_FUNCTIONS=_free,_malloc,_main -o game.html game.c -Os -Wall ./libraylib.a -I. -Iraylib/src -L. -s ASYNCIFY -s USE_GLFW=3 -DPLATFORM_WEB --preload-file resources -s ALLOW_MEMORY_GROWTH=1 -sFORCE_FILESYSTEM=1 -s ASSERTIONS=1 -s STACK_SIZE=10485760 -s EXPORTED_RUNTIME_METHODS=HEAPF32,ccall
