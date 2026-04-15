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

		// Reads the least-significant bit from the buffer.
		uint8_t ReadBit() noexcept;

		uint8_t ReadNBitsMSBFirst(uint8_t bits_to_read) noexcept;

		// Reads a nybble preserving the order.
		// This is different from simply reading 4 bits in a row and shifting between each one.
		// That would turn 1234 into 4321.
		uint8_t ReadNybble() noexcept;

		uint8_t Read8BitsLSBFirst() noexcept;
		uint16_t Read16BitsLSBFirstLittleEndian() noexcept;

		void SkipBits(uint32_t bits_to_skip) noexcept;
		void SkipToNextByteBoundary() noexcept;

	private:

		const std::span<uint8_t>* buffer_;
		size_t byte_index_;
		uint8_t bits_read_this_byte_;

	};

} // namespace maxCompression

#endif // #ifndef MAXCOMPRESSION_BITREADER_HPP
