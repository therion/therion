#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "thinit.h"
#include "thproj.h"

int main( int argc, char* argv[] ) {

  thini.set_proj_lib_path();

  printf("utest: using Proj %s, compiled against %s\n", thcs_get_proj_version().c_str(),
                                                        thcs_get_proj_version_headers().c_str());

  int result = Catch::Session().run( argc, argv );

  return result;
}
