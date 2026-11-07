#include "md5/md5.h"

#include <assert.h>
#include <string.h>

static const uint32_t MD5_CONSTANTS[64] = {
    0xd76aa478u, 0xe8c7b756u, 0x242070dbu, 0xc1bdceeeu,
    0xf57c0fafu, 0x4787c62au, 0xa8304613u, 0xfd469501u,
    0x698098d8u, 0x8b44f7afu, 0xffff5bb1u, 0x895cd7beu,
    0x6b901122u, 0xfd987193u, 0xa679438eu, 0x49b40821u,
    0xf61e2562u, 0xc040b340u, 0x265e5a51u, 0xe9b6c7aau,
    0xd62f105du, 0x02441453u, 0xd8a1e681u, 0xe7d3fbc8u,
    0x21e1cde6u, 0xc33707d6u, 0xf4d50d87u, 0x455a14edu,
    0xa9e3e905u, 0xfcefa3f8u, 0x676f02d9u, 0x8d2a4c8au,
    0xfffa3942u, 0x8771f681u, 0x6d9d6122u, 0xfde5380cu,
    0xa4beea44u, 0x4bdecfa9u, 0xf6bb4b60u, 0xbebfbc70u,
    0x289b7ec6u, 0xeaa127fau, 0xd4ef3085u, 0x04881d05u,
    0xd9d4d039u, 0xe6db99e5u, 0x1fa27cf8u, 0xc4ac5665u,
    0xf4292244u, 0x432aff97u, 0xab9423a7u, 0xfc93a039u,
    0x655b59c3u, 0x8f0ccc92u, 0xffeff47du, 0x85845dd1u,
    0x6fa87e4fu, 0xfe2ce6e0u, 0xa3014314u, 0x4e0811a1u,
    0xf7537e82u, 0xbd3af235u, 0x2ad7d2bbu, 0xeb86d391u
};

static const uint8_t MD5_SHIFTS[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static void __md5_transform( uint32_t _state[4], const uint8_t _block[64] )
{
    uint32_t words[16];

    /* Decode bytes explicitly so unaligned input and either host byte order work. */
    for( uint32_t i = 0; i != 16; ++i )
    {
        size_t offset = (size_t)i * 4;
        words[i] = (uint32_t)_block[offset]
            | ((uint32_t)_block[offset + 1] << 8)
            | ((uint32_t)_block[offset + 2] << 16)
            | ((uint32_t)_block[offset + 3] << 24);
    }

    uint32_t a = _state[0];
    uint32_t b = _state[1];
    uint32_t c = _state[2];
    uint32_t d = _state[3];

    for( uint32_t i = 0; i != 64; ++i )
    {
        uint32_t f;
        uint32_t index;

        if( i < 16 )
        {
            f = (b & c) | (~b & d);
            index = i;
        }
        else if( i < 32 )
        {
            f = (d & b) | (~d & c);
            index = (5 * i + 1) & 15u;
        }
        else if( i < 48 )
        {
            f = b ^ c ^ d;
            index = (3 * i + 5) & 15u;
        }
        else
        {
            f = c ^ (b | ~d);
            index = (7 * i) & 15u;
        }

        uint32_t value = a + f + MD5_CONSTANTS[i] + words[index];
        uint8_t shift = MD5_SHIFTS[i];
        uint32_t rotated = (value << shift) | (value >> (32 - shift));
        a = d;
        d = c;
        c = b;
        b += rotated;
    }

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
}

void md5_data( const void * _data, size_t _size, uint8_t * const _md5, size_t _md5size )
{
    assert( _data != NULL || _size == 0 );
    assert( _md5 != NULL || _md5size == 0 );

    if( _md5size == 0 )
    {
        return;
    }

    const uint8_t * data = (const uint8_t *)_data;
    size_t remaining = _size;
    uint32_t state[4] = {0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u};

    while( remaining >= 64 )
    {
        __md5_transform( state, data );
        data += 64;
        remaining -= 64;
    }

    uint8_t tail[128] = {0};

    if( remaining != 0 )
    {
        memcpy( tail, data, remaining );
    }

    tail[remaining] = 0x80;
    size_t paddedSize = remaining < 56 ? 64 : 128;

    /* MD5 stores the message length modulo 2^64 in little-endian order. */
    uint64_t bitCount = (uint64_t)_size * UINT64_C(8);

    for( size_t i = 0; i != 8; ++i )
    {
        tail[paddedSize - 8 + i] = (uint8_t)(bitCount >> (i * 8));
    }

    __md5_transform( state, tail );

    if( paddedSize == 128 )
    {
        __md5_transform( state, tail + 64 );
    }

    size_t digestSize = _md5size < MD5_DIGEST_SIZE ? _md5size : MD5_DIGEST_SIZE;

    for( size_t i = 0; i != digestSize; ++i )
    {
        _md5[i] = (uint8_t)(state[i / 4] >> ((i % 4) * 8));
    }
}
