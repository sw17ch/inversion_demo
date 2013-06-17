#ifndef bufferer_h
#define bufferer_h

#include <stddef.h>
#include <stdint.h>

/* Indicates the status of different operations. */
enum status {
  status_ok,
  status_err,
};

/* The type of the function that the bufferer expects to have available when it
 * needs to flush data either because of size limitations or a terminating
 * string. */
typedef enum status (write_fn)(void * param, const void * buf, size_t len);

/* The handle used to control and track operations in the bufferer. */
struct handle {
  /* The write function and its user-defined parameter. */
  write_fn * write;
  void * write_param;

  /* The memory to use for buffering data. */
  uint8_t * backing;
  size_t backing_len;

  /* The terminator byte sequence that causes a flush of the buffer. */
  const uint8_t * term;
  size_t term_len;

  size_t pos;
};

/* Initialize a handle so that it can buffer data. */
enum status buf_init(
    struct handle * hdl,
    void * backing, size_t backing_len,
    void * term, size_t term_len,
    write_fn * write,
    void * write_param);

/* Write data to the buffer. */
enum status buf_write(
    struct handle * hdl,
    void * data,
    size_t data_len);

/* Close the buffer. */
enum status buf_close(
    struct handle * hdl);

#endif /* bufferer_h */
