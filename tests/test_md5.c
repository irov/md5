#include "md5/md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int __md5_check_digest( const char * _name, const void * _data, size_t _size, const char * _expected )
{
    uint8_t digest[MD5_DIGEST_SIZE];
    md5_data( _data, _size, digest, sizeof( digest ) );

    static const char digits[] = "0123456789abcdef";
    char hex[MD5_DIGEST_SIZE * 2 + 1];

    for( size_t i = 0; i != sizeof( digest ); ++i )
    {
        hex[i * 2] = digits[digest[i] >> 4];
        hex[i * 2 + 1] = digits[digest[i] & 15];
    }

    hex[sizeof( hex ) - 1] = '\0';

    if( strcmp( hex, _expected ) != 0 )
    {
        fprintf( stderr, "%s: got %s, expected %s\n", _name, hex, _expected );
        return 1;
    }

    return 0;
}

int main( void )
{
    static const struct {
        const char * input;
        const char * expected;
    } vectors[] = {
        {"", "d41d8cd98f00b204e9800998ecf8427e"},
        {"a", "0cc175b9c0f1b6a831c399e269772661"},
        {"abc", "900150983cd24fb0d6963f7d28e17f72"},
        {"message digest", "f96b697d7cb7938d525a2f31aaf161d0"},
        {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "d174ab98d277d9f5a5611c2c9f419d9f"},
        {"12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a"}
    };
    for( size_t i = 0; i != sizeof( vectors ) / sizeof( vectors[0] ); ++i )
    {
        size_t length = strlen( vectors[i].input );
        if( __md5_check_digest( "RFC 1321", vectors[i].input, length, vectors[i].expected ) != 0 )
        {
            return EXIT_FAILURE;
        }
    }

    uint8_t storage[4097];

    for( size_t i = 0; i != sizeof( storage ); ++i )
    {
        storage[i] = (uint8_t)(i - 1);
    }

    static const struct {
        size_t size;
        const char * expected;
    } boundaries[] = {
        {0, "d41d8cd98f00b204e9800998ecf8427e"},
        {1, "93b885adfe0da089cdf634904fd59f71"},
        {55, "6912ee65fff2d9f9ce2508cddf8bcda0"},
        {56, "51fdd1acda72405dfdfa03fcb85896d7"},
        {57, "5320ef4c17ef34a0cf2db763338d25eb"},
        {63, "48a6295221902e8e0938f773a7185e72"},
        {64, "b2d3f56bc197fd985d5965079b5e7148"},
        {65, "8bd7053801c768420faf816fadba971c"},
        {119, "1c772251899a7ff007400b888d6b2042"},
        {120, "b7ba1efc6022e9ed272f00b8831e26e6"},
        {121, "b0b2d719a838db877b6d6571a39a1cdc"},
        {127, "8402b21e7bc7906493bae0dac017f1f9"},
        {128, "37eff01866ba3f538421b30b7cbefcac"},
        {129, "46f986692847558fc38b0cece591c20f"},
        {1024, "b2ea9f7fcea831a4a63b213f41a8855b"},
        {4096, "2bcd3c4de20c918e19fab5c36249c70d"},
    };

    for( size_t i = 0; i != sizeof( boundaries ) / sizeof( boundaries[0] ); ++i )
    {
        if( __md5_check_digest( "padding / unaligned binary", storage + 1, boundaries[i].size, boundaries[i].expected ) != 0 )
        {
            return EXIT_FAILURE;
        }
    }

    for( size_t i = 0; i != sizeof( storage ); ++i )
    {
        if( storage[i] != (uint8_t)(i - 1) )
        {
            fprintf( stderr, "input was modified\n" );
            return EXIT_FAILURE;
        }
    }

    if( __md5_check_digest( "null empty input", NULL, 0, "d41d8cd98f00b204e9800998ecf8427e" ) != 0 )
    {
        return EXIT_FAILURE;
    }

    md5_data( NULL, 0, NULL, 0 );

    char * million = (char *)malloc( 1000000 );
    if( million == NULL )
    {
        return EXIT_FAILURE;
    }

    memset( million, 'a', 1000000 );
    if( __md5_check_digest( "million a", million, 1000000, "7707d6ae4e027c70eea2a935c2296f21" ) != 0 )
    {
        free( million );
        return EXIT_FAILURE;
    }
    free( million );

    static const uint8_t abc[MD5_DIGEST_SIZE] = {
        0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
        0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
    };
    static const size_t capacities[] = {0, 1, 15, 16, 17, 32};

    for( size_t i = 0; i != sizeof( capacities ) / sizeof( capacities[0] ); ++i )
    {
        uint8_t guarded[40];
        memset( guarded, 0xa5, sizeof( guarded ) );
        md5_data( "abc", 3, guarded + 1, capacities[i] );

        size_t count = capacities[i] < MD5_DIGEST_SIZE ? capacities[i] : MD5_DIGEST_SIZE;

        for( size_t j = 0; j != sizeof( guarded ); ++j )
        {
            uint8_t expected = j > 0 && j <= count ? abc[j - 1] : 0xa5;
            if( guarded[j] != expected )
            {
                fprintf( stderr, "output capacity / canary mismatch\n" );
                return EXIT_FAILURE;
            }
        }
    }

    puts( "MD5: RFC vectors, padding boundaries, unaligned binary, million a and output guards passed" );
    return EXIT_SUCCESS;
}
