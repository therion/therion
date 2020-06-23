#define CATCH_CONFIG_RUNNER
#include "extern/catch2/catch.hpp"
#include "thinit.h"
#include "thproj.h"

int main( int argc, char* argv[] ) {

  thini.set_proj_lib_path();

  int result = Catch::Session().run( argc, argv );

  return result;
}
