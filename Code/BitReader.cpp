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

} // namespace maxCompression