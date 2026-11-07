# md5

A small, allocation-free MD5 implementation in portable C99, following
[RFC 1321](https://www.rfc-editor.org/rfc/rfc1321). It has no platform or crypto
framework dependencies. Input can be unaligned and can contain arbitrary bytes.

MD5 is provided for compatibility with existing formats and protocols. It is
not suitable for password storage, signatures, or other security guarantees.

```c
#include <md5/md5.h>

uint8_t digest[MD5_DIGEST_SIZE];
md5_data("abc", 3, digest, sizeof(digest));
```

Use `<md5/md5.hpp>` from C++ to import the API with C linkage.
`md5_data` writes up to 16 digest bytes, limited by the output capacity argument.
An empty input may use a null data pointer; a zero-capacity output may be null.
Other buffers must be valid for their supplied sizes. The output contains raw
bytes; hexadecimal formatting belongs to the caller.

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Tests cover the RFC vectors, padding boundaries, binary and unaligned input,
one million `a` characters, output capacity boundaries, and C++ linkage.

For embedding, set `MD5_EXTERNAL_BUILD=ON` before `add_subdirectory` and link
the `md5` target. This leaves compiler warning policy to the parent project.
Set `MD5_INSTALL=ON` to enable installation of the library and public header.

Released under the [Unlicense](LICENSE), like `irov/sha1`.
