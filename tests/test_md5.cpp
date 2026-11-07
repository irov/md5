#include "md5/md5.hpp"

#include <cstdlib>
#include <cstring>

int main()
{
    const uint8_t expected[MD5_DIGEST_SIZE] = {
        0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
        0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
    };
    uint8_t digest[MD5_DIGEST_SIZE];
    md5_data( "abc", 3, digest, sizeof( digest ) );

    int result = std::memcmp( digest, expected, sizeof( digest ) );

    if( result != 0 )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
