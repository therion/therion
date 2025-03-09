#include "thlog.h"
#include "thlogfile.h"

void thlog(std::string_view msg)
{
  get_thlogfile().print(msg);
}
