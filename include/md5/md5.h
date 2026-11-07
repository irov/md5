#ifndef MD5_MD5_H_
#define MD5_MD5_H_

#include <stddef.h>
#include <stdint.h>

#define MD5_DIGEST_SIZE 16

/* Writes min(_md5size, MD5_DIGEST_SIZE) digest bytes.
 * _data may be NULL when _size is zero.
 * _md5 may be NULL when _md5size is zero.
 */
void md5_data( const void * _data, size_t _size, uint8_t * const _md5, size_t _md5size );

#endif
