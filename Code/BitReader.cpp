// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "BitReader.hpp"

#include <utility>

namespace maxCompression {

	BitReader::BitReader(const std::span<uint8_t>* buffer) noexcept
		: buffer_(std::move(buffer))
		, byte_index_(0)
		, bits_read_this_byte_(0)
	{}
	
	uint8_t BitReader::ReadBit() noexcept {
		auto bit = ((*buffer_)[byte_index_] >> bits_read_this_byte_) & 1;

		if (bits_read_this_byte_++ == 7) {
			byte_index_++;
			bits_read_this_byte_ = 0;
		}

		return bit;
	}

	uint8_t BitReader::ReadNBitsMSBFirst(uint8_t bits_to_read) noexcept {
		auto bits_left_this_byte = 8 - bits_read_this_byte_;

		// Try a simple shift & mask if available.
		if (bits_left_this_byte >= bits_to_read) {
			auto mask = uint8_t{0b1111'1111};
			mask <<= bits_read_this_byte_ + bits_to_read;
			mask = ~mask;

			auto masked_value = (*buffer_)[byte_index_] & mask;
			masked_value >>= bits_read_this_byte_;

			bits_read_this_byte_ += bits_to_read;
			if (bits_read_this_byte_ == 8) {
				byte_index_++;
				bits_read_this_byte_ = 0;
			}

			return masked_value;
		}

		// The value spans a byte boundary. We need combine both parts.
		auto low_part = (*buffer_)[byte_index_] >> bits_read_this_byte_;

		byte_index_++;
		auto bits_to_read_next_byte = bits_to_read - bits_left_this_byte;
		auto high_mask = ~(uint8_t{0b1111'1111} << bits_to_read_next_byte);
		auto high_part = (*buffer_)[byte_index_] & high_mask;

		// Shift the high part into place before combining
		high_part <<= bits_left_this_byte;
		bits_read_this_byte_ = bits_to_read_next_byte;



		return high_part | low_part;
	}

	uint8_t BitReader::ReadNybble() noexcept {
		auto first_bit  = ReadBit();
		auto second_bit = ReadBit();
		auto third_bit  = ReadBit();
		auto fourth_bit = ReadBit();

		return (fourth_bit << 3) |
		       (third_bit  << 2) |
		       (second_bit << 1) |
		       (first_bit  << 0);
	}

	uint8_t BitReader::Read8BitsLSBFirst() noexcept {
		// TODO: For now, I'm going to assume we're already aligned on a byte boundary.
		// I should probably make functions with & without that assumption.

		return (*buffer_)[byte_index_++];
	}

	uint16_t BitReader::Read16BitsLSBFirstLittleEndian() noexcept {
		// TODO: For now, I'm going to assume we're already aligned on a byte boundary.
		// I should probably make functions with & without that assumption.

		auto low_byte  = (*buffer_)[byte_index_++];
		auto high_byte = (*buffer_)[byte_index_++];

		return static_cast<uint16_t>((high_byte << 8) | low_byte);
	}

	void BitReader::SkipBits(uint32_t bits_to_skip) noexcept {
		byte_index_ += bits_to_skip / 8;
		bits_read_this_byte_ += bits_to_skip % 8;

		if (bits_read_this_byte_ >= 8) {
			byte_index_++;
			bits_read_this_byte_ %= 8;
		}
	}

	void BitReader::SkipToNextByteBoundary() noexcept {
		if (bits_read_this_byte_ == 0) {
			return;
		}

		byte_index_++;
		bits_read_this_byte_ = 0;
	}

} // namespace maxCompression