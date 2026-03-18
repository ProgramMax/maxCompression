// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAXCOMPRESSION_BITREADER_HPP
#define MAXCOMPRESSION_BITREADER_HPP

#include <cstdint>
#include <span>

namespace maxCompression {

	class BitReader {
	public:

		explicit BitReader(const std::span<uint8_t>* buffer) noexcept;

		uint8_t ReadBit() noexcept;

	private:

		const std::span<uint8_t>* buffer_;
		size_t byte_index_;
		uint8_t bits_read_this_byte_;

	};

} // namespace maxCompression

#endif // #ifndef MAXCOMPRESSION_BITREADER_HPP
