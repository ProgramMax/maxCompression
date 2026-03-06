// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAXCOMPRESSION_NAIVELZ77_HPP
#define MAXCOMPRESSION_NAIVELZ77_HPP

#include <cstdint>
#include <span>
#include <variant>
#include <vector>

namespace maxCompression {

	struct DistanceAndLength {

		explicit DistanceAndLength(uint16_t distance, uint16_t length) noexcept;

		uint16_t distance_; // from the start of the sliding window
		uint16_t length_;

	};

	// All forms of Lempel-Ziv compressions work on a series of untouched spans and references.
	// But different forms will represent those differently in the compressed bitstream.
	// So expose the series of spans and references for the different forms to use in their own way.
	using Segment = std::variant<std::span<uint8_t>, DistanceAndLength>;

	std::vector<Segment> LempelZivCompress(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_size, uint8_t minimum_match_length) noexcept;

}

#endif // #ifndef MAXCOMPRESSION_NAIVELZ77_HPP