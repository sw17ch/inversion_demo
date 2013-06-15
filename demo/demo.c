#include <bufferer.h>
#include <defines.h>

#include <unistd.h>
#include <stdio.h>

static enum status output(
    void * fh,
    const void * buf,
    size_t len);

int main(int argc, char * argv[]) {
  IGNORE(argc);
  IGNORE(argv);


  uint8_t backing[64];
  char term[] = "\n";
  const size_t TERM_LEN = 1;
  struct handle hdl;
  enum status s;

  s = buf_init(&hdl, backing, sizeof(backing), term, TERM_LEN, output, stdout);
  if (status_ok != s) {
    fprintf(stderr, "ERROR: Unable to initialize.\n");
  }

  char str[] = "hello\nworld\nhow are you?";
  s = buf_write(&hdl, str, sizeof(str));
  if (status_ok != s) {
    fprintf(stderr, "ERROR: Unable to write.\n");
  }

  s = buf_close(&hdl);
  if (status_ok != s) {
    fprintf(stderr, "ERROR: Unable to close.\n");
  }

  printf("\n\nDone.\n");

  return 0;
}

static enum status output(
    void * param,
    const void * buf,
    size_t len) {

  FILE * fd = param;
  fwrite(buf, 1, len, fd);

  return status_ok;
}
