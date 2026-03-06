// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAXCOMPRESSION_NAIVELZ77_HPP
#define MAXCOMPRESSION_NAIVELZ77_HPP

#include <cstdint>
#include <span>
#include <vector>

namespace maxCompression {

	struct DistanceAndLength {

		explicit DistanceAndLength(uint16_t distance, uint16_t length) noexcept;

		uint16_t distance_;
		uint16_t length_;

	};

	std::vector<DistanceAndLength> LZ77Compress(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_size) noexcept;

}

#endif // #ifndef MAXCOMPRESSION_NAIVELZ77_HPP