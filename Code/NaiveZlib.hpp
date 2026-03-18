// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAXCOMPRESSION_NAIVEZLIB_HPP
#define MAXCOMPRESSION_NAIVEZLIB_HPP

#include <cstdint>
#include <span>
#include <vector>

// Zlib is probably most known as being a library.
// But it is also bitstream header & footer, wrapping Deflate.
// This code is for interacting with a zlib bitstream.

// It is defined by RFC 1950: https://www.rfc-editor.org/rfc/rfc1950

namespace maxCompression {

	std::vector<uint8_t> DecompressZlib(const std::span<uint8_t>& compressed_buffer) noexcept;


}; // namespace maxCompression

#endif // #ifndef MAXCOMPRESSION_NAIVEZLIB_HPP