#include <uuid/uuid.h>

int
main (int   argc,
      char**argv)
{
  char  buffer[37];
  uuid_t out;
  uuid_clear         (out);
  uuid_generate_time (out);
  uuid_unparse       (out, buffer);

  return 0;
}
