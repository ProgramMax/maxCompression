// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "NaiveZlib.hpp"
#include "NaiveDeflate.hpp""

namespace maxCompression {

	std::vector<uint8_t> DecompressZlib(const std::span<uint8_t>& compressed_buffer) noexcept {
		// The Zlib format is defined in https://www.rfc-editor.org/rfc/rfc1950

		// The first nybble is the window size, 2^(X+8).
		// So 0 is 256, 7 is 32k. The value 8 is not used.

		// The second nybble is the compression method.
		// Only Deflate (8) is used.

		// The next two bits (the high bits of the second byte) indicate the rough compression level.
		// So 0 is fast and 3 is slow.

		// The next bit indicates a predefined dictionary or not.
		// This is usually 0.

		// The following 5 bits (the remaining bits in the second byte) are a checksum of everything so far.

		// Since the window size and the compression level are the only values that typically change, that
		// produces a grid of the two byte combinations:
		//      level: 0       1       2       3
		//window size:
		//     0      08 1D   08 5B   08 99   08 D7
		//     1      18 19   18 57   18 95   18 D3
		//     2      28 15   28 53   28 91   28 CF
		//     3      38 11   38 4F   38 8D   38 CB
		//     4      48 0D   48 4B   48 89   48 C7
		//     5      58 09   58 47   58 85   58 C3
		//     6      68 05   68 43   68 81   68 DE
		//     7      78 01   78 5E   78 9C   78 DA

		// That said, the window size is almost always 7, which is the maximum.
		// So the bottom row is likely the only pair of bytes to worry about.


		// Get the window size from the first nybble
		auto window_size = uint16_t{0};
		switch ((compressed_buffer[0] >> 4) & 0xF) {
			case 0:
				window_size = 256;
				break;
			case 1:
				window_size = 512;
				break;
			case 2:
				window_size = 1024;
				break;
			case 3:
				window_size = 2 * 1024;
				break;
			case 4:
				window_size = 4 * 1024;
				break;
			case 5:
				window_size = 8 * 1024;
				break;
			case 6:
				window_size = 16 * 1024;
				break;
			case 7:
				window_size = 32 * 1024;
				break;
			default:
				// Error
				break;
		}

		// Get the compression method from the second nybble
		switch (compressed_buffer[0] & 0xF) {
			case 8:
				// Deflate
				break;
			default:
				// error
				break;
		}

		// Pass the buffer over to Deflate, removing the first 2 bytes (header) and the last 4 bytes (Adler-32)
		return NaiveDeflateDecompress(compressed_buffer.subspan(2, compressed_buffer.size() - 6));
	}

} // namespace maxCompression
