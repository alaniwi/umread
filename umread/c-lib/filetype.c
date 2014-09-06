#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "umfileint.h"


/* values passed to valid_um_word2 and valid_pp_word1 could be 32 or
 * 64-bit.  Declare as longer of these two (int64_t), and shorter will be
 * accommodated also.
 */

static int valid_um_word2(int64_t val)
{
  /* second word should be 1,2 or 4, reflecting model ID in fixed length
     header */
  return (val == 1 || val == 2 || val == 4);
}

static int valid_pp_word1(int64_t val, int wsize)
{
  /* first word should be integer from Fortan representing length of header
     record */
  return (val == 64 * wsize || val == 128 * wsize);
}

int detect_file_type_(int fd, File_type *file_type)
{
  int32_t data4[4], data4s[4];
  int64_t data8[2], data8s[2];

  /* read and store first two integers according to suppositions 
   * of 4- or 8- byte, and native or swapped byte ordering
   */
  lseek(fd, 0, SEEK_SET);
  if(read(fd, data8, 16) != 16) return 1;

  memcpy(data4, data8, 16);

  memcpy(data4s, data8, 16);
  swap_bytes_sgl(data4s, 4);

  memcpy(data8s, data8, 16);
  swap_bytes_dbl(data8s, 2);

  /* UM ancillary files: test second word (the submodel ID - is this 1, 2 or
   * 4?)  (this should not give false +ve with PP file -- though takes
   * thinking through the various combinations of 32 / 64 bit to convince
   * yourself of this)
   *
   * If this test fails, test for PP file.  Here test first word, which should
   * be record length (put there by fortran).  Because it's the first word,
   * 64-bit little-endian could appear to be 32-bit little-endian, so a file
   * is not 32-bit PP if both the second and fourth words are zero.
   */
  if (valid_um_word2(data4[1]))
    {
      file_type->format = fields_file;
      file_type->byte_ordering = NATIVE_ORDERING;
      file_type->word_size = 4;
    }
  else if (valid_um_word2(data8[1]))
    {
      file_type->format = fields_file;
      file_type->byte_ordering = NATIVE_ORDERING;
      file_type->word_size = 8;
    }
  else if (valid_um_word2(data4s[1]))
    {
      file_type->format = fields_file;
      file_type->byte_ordering = REVERSE_ORDERING;
      file_type->word_size = 4;
    }
  else if (valid_um_word2(data8s[1]))
    {
      file_type->format = fields_file;
      file_type->byte_ordering = REVERSE_ORDERING;
      file_type->word_size = 8;
    }
  else if (valid_pp_word1(data4[0],4)
	   && !(data4[1] == 0 && data4[3] == 0))
    {
      file_type->format = plain_pp;
      file_type->byte_ordering = NATIVE_ORDERING;
      file_type->word_size = 4;
    }
  else if (valid_pp_word1(data8[0],8))
    {
      file_type->format = plain_pp;
      file_type->byte_ordering = NATIVE_ORDERING;
      file_type->word_size = 8;
    }
  else if (valid_pp_word1(data4s[0],4)
	   && !(data4s[1] == 0 && data4s[3] == 0))
    {
      file_type->format = plain_pp;
      file_type->byte_ordering = REVERSE_ORDERING;
      file_type->word_size = 4;
    }
  else if (valid_pp_word1(data8s[0],8))
    {
      file_type->format = plain_pp;
      file_type->byte_ordering = REVERSE_ORDERING;
      file_type->word_size = 8;
    }
  else
    {
      /* type not identified */
      return 1;
    }
  return 0;
}
