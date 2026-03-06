// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "NaiveLZ77.hpp"

#include <utility>

namespace {

	// Returns 0 if there is no match. Otherwise, returns the length of the match.
	//
	// Note: This is a LL77-style match, meaning it can continue matching beyond the sliding window's end.
	// For example, take the buffer: |blah |blah blah blah
	// (where || indicates the sliding window position).
	// The "blah " inside the sliding window matches the next two "blah "s.
	// The match length is longer than the sliding window because as the match is copied into the decompressed buffer, it continues to be a match.
	//
	// This function only tests at the start of |sliding_window_start|.
	// Call it multiple times to test each byte within the sliding window, with |sliding_window_start+1| for example.
	uint16_t GetMatchLength(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_start, uint16_t sliding_window_length) noexcept {
		auto match_length = uint16_t{0};

		auto input_to_be_processed_start = sliding_window_start + sliding_window_length;
		auto search_length = input_buffer.size() - input_to_be_processed_start;
		for (auto i = uint16_t{0}; i < search_length; i++) {
			if (input_buffer[sliding_window_start + (i % sliding_window_length)] != input_buffer[input_to_be_processed_start + i]) {
				// Does not match.
				break;
			}

			match_length++;
		}

		return match_length;
	}

	maxCompression::DistanceAndLength GetLongestMatch(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_start, uint16_t sliding_window_length) noexcept {
		auto longest_match_start = uint16_t{0};
		auto longest_match_length = uint16_t{0};

		for (auto i = sliding_window_start; i < sliding_window_length; i++) {
			auto match_length = GetMatchLength(input_buffer, sliding_window_start + i, sliding_window_length - i);
			if (match_length > longest_match_length) {
				longest_match_start = i;
				longest_match_length = match_length;
			}
		}

		return maxCompression::DistanceAndLength{static_cast<uint16_t>(sliding_window_start + sliding_window_length - longest_match_start), std::move(longest_match_length)};
	}

} // anonymous namespace

namespace maxCompression {

	DistanceAndLength::DistanceAndLength(uint16_t distance, uint16_t length) noexcept
		: distance_(std::move(distance))
		, length_(std::move(length))
	{}

	std::vector<DistanceAndLength> LZ77Compress(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_size) noexcept {
		auto compressed_buffer = std::vector<DistanceAndLength>{};
		auto sliding_window_start = uint16_t{0};
		auto sliding_window_length = uint16_t{6};

		compressed_buffer.emplace_back(GetLongestMatch(input_buffer, sliding_window_start, sliding_window_length));

		return compressed_buffer;
	}

} // namespace maxCompression