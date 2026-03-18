// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


// This is not a good implementation. Don't use it.
// The intent is to be the naive, obvious approach.
// It can then be used as a baseline to verify better, more clever implementations.
// It can verify that those implementations produce correct results and outperform the naive approach.


#ifndef MAXCOMPRESSION_NAIVEDEFLATE_HPP
#define MAXCOMPRESSION_NAIVEDEFLATE_HPP

// Deflate is defined in RFC 1951: https://www.rfc-editor.org/rfc/rfc1951

#include <cstdint>
#include <span>
#include <vector>

namespace maxCompression {

	std::vector<uint8_t> NaiveDeflateCompress(const std::span<uint8_t>& uncompressed_buffer) noexcept;
	std::vector<uint8_t> NaiveDeflateDecompress(const std::span<uint8_t>& compressed_buffer) noexcept;

} // namespace maxCompression

#endif // #ifndef MAXCOMPRESSION_NAIVEDEFLATE_HPP
