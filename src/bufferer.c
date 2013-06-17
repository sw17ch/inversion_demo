#include <bufferer.h>
#include <defines.h>

#include <string.h>

#include <stdio.h>
#include <stdbool.h>

#define BUF_REM(HDL) (hdl->pos >= hdl->backing_len ? 0 : hdl->backing_len - hdl->pos)
#define BUF_POS(HDL) (&hdl->backing[hdl->pos])

/* Indicates whether or not the buffered memory currently points to a
 * terminator. */
static bool has_term(struct handle * hdl);

/* Writes all data in the buffer and resets the positioning information. */
static enum status flush(struct handle * hdl);

enum status buf_init(
    struct handle * hdl,
    void * backing, size_t backing_len,
    void * term, size_t term_len,
    write_fn * write,
    void * write_param) {
  if (backing_len <= 0 || term_len <= 0) {
    return status_err;
  }

  hdl->write = write;
  hdl->write_param = write_param;

  hdl->backing = backing;
  hdl->backing_len = backing_len;

  hdl->term = term;
  hdl->term_len = term_len;

  hdl->pos = 0;

  return status_ok;
}

enum status buf_write(
    struct handle * hdl,
    void * data,
    size_t data_len) {
  size_t copy_pos = 0;

  for (size_t i = 0; i < data_len; i++) {
    if (0 == BUF_REM(hdl) || has_term(hdl)) {
      if (status_err == flush(hdl)) {
        return status_err;
      }
    }

    *BUF_POS(hdl) = ((uint8_t*)data)[copy_pos];
    hdl->pos += 1;
    copy_pos += 1;
  }

  return status_ok;
}

enum status buf_close(
    struct handle * hdl) {
  return flush(hdl);
}

static bool has_term(struct handle * hdl) {
  if (hdl->pos < hdl->term_len) {
    return false;
  }

  void * buf = &hdl->backing[hdl->pos - hdl->term_len];

  if (0 != memcmp(buf, hdl->term, hdl->term_len)) {
    return false;
  }

  return true;
}

static enum status flush(struct handle * hdl) {
  if (status_err == hdl->write(hdl->write_param, hdl->backing, hdl->pos)) {
    return status_err;
  } else {
    memset(hdl->backing, 0, hdl->backing_len);
    hdl->pos = 0;

    return status_ok;
  }
}
