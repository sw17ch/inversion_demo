#include "greatest.h"
#include "bufferer.h"
#include "defines.h"

#include <stdint.h>
#include <unistd.h>

#define NORM_INIT(HDL) buf_init(&(HDL), buffer, sizeof(buffer), \
    term, TERM_LEN, test_write, flush_buffer)

#define MULT_INIT(HDL) buf_init(&(HDL), buffer, sizeof(buffer), \
    mult_term, MULT_TERM_LEN, test_write, flush_buffer)

#define CUST_BLEN_INIT(HDL, BLEN) buf_init(&(HDL), buffer, (BLEN), \
    term, TERM_LEN, test_write, flush_buffer)

#define CUST_TLEN_INIT(HDL, TLEN) buf_init(&(HDL), buffer, (TLEN), \
    term, TERM_LEN, test_write, flush_buffer)

struct handle h;
uint8_t buffer[64];
uint8_t term[] = "$";
const size_t TERM_LEN = 1;
uint8_t mult_term[] = "^@";
const size_t MULT_TERM_LEN = 2;

size_t flush_pos = 0;
uint8_t flush_buffer[128];

/* Test Helper Functions */
static void setup(void * p) {
  IGNORE(p);

  OBJ_ZERO(h);
  OBJ_ZERO(buffer);
  OBJ_ZERO(flush_buffer);
  OBJ_ZERO(flush_pos);
}

/* Callbacks for the write function. */
static enum status test_bad_write(void * param, const void * src, size_t len) {
  IGNORE(param);
  IGNORE(src);
  IGNORE(len);

  return status_err;
}

static enum status test_write(void * param, const void * src, size_t len) {
  uint8_t * b = param;

  memcpy(&b[flush_pos], src, len);
  flush_pos += len;

  return status_ok;
}

/* Test cases. */
TEST init_works(void) {
  enum status s = NORM_INIT(h);

  ASSERT_EQ(status_ok, s);

  ASSERT_EQ(flush_buffer, h.write_param);
  ASSERT_EQ(test_write, h.write);

  ASSERT_EQ(buffer, h.backing);
  ASSERT_EQ(sizeof(buffer), h.backing_len);

  ASSERT_EQ(term, h.term);
  ASSERT_EQ(TERM_LEN, h.term_len);

  ASSERT_EQ(0, h.pos);

  PASS();
}

TEST init_catches_length_issues(void) {
  ASSERT_EQ(
      status_err,
      CUST_BLEN_INIT(h, 0));

  ASSERT_EQ(
      status_err,
      CUST_TLEN_INIT(h, 0));

  PASS();
}

TEST write_works(void) {
  enum status s;

  s = NORM_INIT(h);

  ASSERT_EQ(status_ok, s);

  char b[] = "hello";
  s = buf_write(&h, b, sizeof(b));

  ASSERT_EQ(status_ok, s);
  ASSERT_STR_EQ("hello", (char *)buffer);

  PASS();
}

TEST write_flushes_when_out_of_space(void) {
  enum status s;

  s = CUST_BLEN_INIT(h, 5);

  ASSERT_EQ(status_ok, s);

  char b[] = "hello me";
  s = buf_write(&h, b, sizeof(b));

  ASSERT_EQ(status_ok, s);
  ASSERT_STR_EQ("hello", (char *)flush_buffer);
  ASSERT_STR_EQ(" me", (char *)buffer);

  PASS();
}

TEST write_flushes_on_terminator(void) {
  enum status s;
  
  s = NORM_INIT(h);

  ASSERT_EQ(status_ok, s);

  char b[] = "hello$ me";
  s = buf_write(&h, b, sizeof(b));

  ASSERT_STR_EQ("hello$", (char *)flush_buffer);
  ASSERT_STR_EQ(" me", (char *)buffer);

  PASS();
}

TEST write_flushes_on_multi_char_term(void) {
  enum status s;
  
  s = MULT_INIT(h);

  ASSERT_EQ(status_ok, s);

  char b[] = "hello^me^@how are you?";
  s = buf_write(&h, b, sizeof(b));

  ASSERT_STR_EQ("hello^me^@", (char *)flush_buffer);
  ASSERT_STR_EQ("how are you?", (char *)buffer);

  PASS();
}

TEST write_flushes_multiple_times(void) {
  enum status s;
  
  s = NORM_INIT(h);

  ASSERT_EQ(status_ok, s);

  char b[] = "hello$me$how$are$you?";
  s = buf_write(&h, b, sizeof(b));
  ASSERT_STR_EQ("hello$me$how$are$", (char *)flush_buffer);
  ASSERT_STR_EQ("you?", (char *)buffer);

  PASS();
}

TEST write_flushes_with_multiple_writes(void) {
  enum status s;
  
  s = NORM_INIT(h);

  ASSERT_EQ(status_ok, s);

  char b_0[] = "hello me";
  char b_1[] = "$";
  char b_2[] = "how are you?";
  char b_3[] = "$stuff";

  ASSERT_EQ(status_ok, buf_write(&h, b_0, sizeof(b_0) - 1));
  ASSERT_EQ(status_ok, buf_write(&h, b_1, sizeof(b_1) - 1));
  ASSERT_EQ(status_ok, buf_write(&h, b_2, sizeof(b_2) - 1));
  ASSERT_EQ(status_ok, buf_write(&h, b_3, sizeof(b_3) - 1));

  ASSERT_STR_EQ("hello me$how are you?$", (char *)flush_buffer);
  ASSERT_STR_EQ("stuff", (char *)buffer);

  PASS();
}

TEST close_works(void) {
  char b[] = "hello";

  ASSERT_EQ(status_ok, NORM_INIT(h));
  ASSERT_EQ(status_ok, buf_write(&h, b, sizeof(b)));

  ASSERT_STR_EQ("", (char *)flush_buffer);
  ASSERT_STR_EQ("hello", (char *)buffer);

  ASSERT_EQ(status_ok, buf_close(&h));

  ASSERT_STR_EQ("hello", (char *)flush_buffer);

  PASS();
};

TEST write_handles_failed_flush(void) {
  char b[] = "hello$";
  enum status s = buf_init(&h, buffer, sizeof(buffer),
      term, TERM_LEN, test_bad_write, flush_buffer);

  ASSERT_EQ(status_ok, s);
  ASSERT_EQ(status_err, buf_write(&h, b, sizeof(b)));

  PASS();
}

/* Test suite. */
SUITE(bufferer) {
  SET_SETUP(setup, NULL);

  RUN_TEST(init_works);
  RUN_TEST(init_catches_length_issues);

  RUN_TEST(write_works);
  RUN_TEST(write_flushes_when_out_of_space);
  RUN_TEST(write_flushes_on_terminator);
  RUN_TEST(write_flushes_on_multi_char_term);
  RUN_TEST(write_flushes_multiple_times);
  RUN_TEST(write_flushes_with_multiple_writes);
  RUN_TEST(write_handles_failed_flush);

  RUN_TEST(close_works);
}

GREATEST_MAIN_DEFS();

int main(int argc, char * argv[]) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(bufferer);
  GREATEST_MAIN_END();
}
