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
	// This function only tests at the start of |start|.
	// Call it multiple times to test each byte within the sliding window, with |start+1| for example.
	uint16_t GetMatchLength(const std::span<uint8_t>& input_buffer, size_t start, uint16_t length) noexcept {
		auto match_length = uint16_t{0};

		auto input_to_be_processed_start = start + length;
		auto search_length = input_buffer.size() - input_to_be_processed_start;
		for (auto i = size_t{0}; i < search_length; i++) {
			if (input_buffer[start + (i % length)] != input_buffer[input_to_be_processed_start + i]) {
				// Does not match.
				break;
			}

			match_length++;
		}

		return match_length;
	}

	maxCompression::DistanceAndLength GetLongestMatch(const std::span<uint8_t>& input_buffer, size_t sliding_window_start, uint16_t sliding_window_length) noexcept {
		auto longest_match_start = size_t{0};
		auto longest_match_length = uint16_t{0};

		// Search each starting index inside the sliding window
		for (auto i = uint16_t{0}; i < sliding_window_length; i++) {
			auto match_length = GetMatchLength(input_buffer, sliding_window_start + i, static_cast<uint16_t>(sliding_window_length - i));
			if (match_length > longest_match_length) {
				longest_match_start = i;
				longest_match_length = match_length;
			}
		}

		// TODO: I am reading conflicting examples where distance is backwards from the current processed byte or forwards from the start of the sliding window
		return maxCompression::DistanceAndLength{static_cast<uint16_t>(std::move(longest_match_start)), std::move(longest_match_length)};
	}

} // anonymous namespace

namespace maxCompression {

	DistanceAndLength::DistanceAndLength(uint16_t distance, uint16_t length) noexcept
		: distance_(std::move(distance))
		, length_(std::move(length))
	{}

	std::vector<Segment> LZ77Compress(const std::span<uint8_t>& input_buffer, uint16_t sliding_window_size, uint8_t minimum_match_length) noexcept {
		auto compressed_buffer = std::vector<Segment>{};
		auto last_match_end = size_t{0};
		auto sliding_window_start = size_t{0};
		auto sliding_window_length = uint16_t{0};

		auto input_buffer_size = input_buffer.size();
		for (auto i = size_t{0}; i < input_buffer_size; i++) {
			auto longest_match = GetLongestMatch(input_buffer, sliding_window_start, sliding_window_length);
			if (sliding_window_length == sliding_window_size) {
				sliding_window_start++;
			} else {
				sliding_window_length++;
			}

			if (longest_match.length_ < minimum_match_length) {
				continue;
			}

			if (longest_match.distance_ != last_match_end + i) {
				// Add unmatched span
				compressed_buffer.emplace_back(input_buffer.subspan(last_match_end, i - last_match_end));
			}
			compressed_buffer.emplace_back(std::move(longest_match));

			last_match_end = i + longest_match.length_;
			sliding_window_length += longest_match.length_ - 1; // -1 because we already incremented it above
			// Because matches may go past |i| into the rest of the |input_buffer|, skip |i| forward
			i = last_match_end - 1; // -1 because we're about to increment it in the loop
		}

		if (last_match_end != input_buffer_size) {
			compressed_buffer.emplace_back(input_buffer.subspan(last_match_end, input_buffer_size - last_match_end));
		}

		return compressed_buffer;
	}

} // namespace maxCompression