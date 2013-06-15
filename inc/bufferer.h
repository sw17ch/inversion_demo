#ifndef bufferer_h
#define bufferer_h

#include <stddef.h>
#include <stdint.h>

enum status {
  status_ok,
  status_err,
};

typedef enum status (write_fn)(void * param, const void * buf, size_t len);

struct handle {
  void * write_param;
  write_fn * write;

  uint8_t * backing;
  size_t backing_len;

  const uint8_t * term;
  size_t term_len;

  size_t pos;
};

enum status buf_init(
    struct handle * hdl,
    void * backing, size_t backing_len,
    void * term, size_t term_len,
    write_fn * write,
    void * write_param);

enum status buf_write(
    struct handle * hdl,
    void * data,
    size_t data_len);

enum status buf_close(
    struct handle * hdl);

#endif /* bufferer_h */
