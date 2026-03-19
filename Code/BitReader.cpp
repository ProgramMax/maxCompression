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

	void BitReader::SkipBits(uint32_t bits_to_skip) noexcept {
		byte_index_ += bits_to_skip / 8;
		bits_read_this_byte_ += bits_to_skip % 8;

		if (bits_read_this_byte_ >= 8) {
			byte_index_++;
			bits_read_this_byte_ %= 8;
		}
	}

} // namespace maxCompression