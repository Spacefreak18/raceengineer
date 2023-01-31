set(gameloop_source_files
  gameloop.c
  gameloop.h
  metricmapper.c
  metricmapper.h
  singleshot.c
  singleshot.h
)

add_library(gameloop STATIC ${gameloop_source_files})

target_compile_definitions(gameloop PRIVATE SIMMAP_ALL)
