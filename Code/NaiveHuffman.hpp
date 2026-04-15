// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


// This is not a good implementation. Don't use it.
// The intent is to be the naive, obvious approach.
// It can then be used as a baseline to verify better, more clever implementations.
// It can verify that those implementations produce correct results and outperform the naive approach.


#ifndef MAXCOMPRESSION_NAIVEHUFFMAN_HPP
#define MAXCOMPRESSION_NAIVEHUFFMAN_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace maxCompression {


	// The returned vector contains the count of occurrences at a given symbol (value).
	// The input is of type uint16_t because deflate will compress values outside a byte range.
	// This allows it to insert special codes.
	// A second function with input of type uint8_t is provided just to test the core concept.
	std::vector<uint32_t> CountOccurrences(std::span<uint8_t> uncompressed_data) noexcept;
	std::vector<uint32_t> CountOccurrences(std::span<uint16_t> uncompressed_data) noexcept;


	// Every unique byte found in the uncompressed data will have one of these nodes created.
	// Their counts are set to the occurrances of that given byte.
	// Then, they are all put in a binary tree where the parent's |count_| is the sum of its two children.
	// For these parent nodes, |value_| will not be set.
	struct NaiveHuffmanNode {

		explicit NaiveHuffmanNode(uint32_t count) noexcept;
		explicit NaiveHuffmanNode(uint32_t count, uint16_t value) noexcept;
		NaiveHuffmanNode(NaiveHuffmanNode&& rhs) noexcept;

		uint32_t count_; // The occurances of |value_| in the uncompressed string
		// Or, if |value_|

		std::optional<uint16_t> value_ = std::nullopt; // The byte found in the uncompressed data

		// Binary tree elements
		std::unique_ptr<NaiveHuffmanNode> left_ = nullptr;
		std::unique_ptr<NaiveHuffmanNode> right_ = nullptr;

	};
	std::unique_ptr<NaiveHuffmanNode> CreateHuffmanTree(std::span<uint32_t> occurrences) noexcept;


	// Canonical Huffman codes are an efficient way to store the tree.
	// It stores a list of how many nodes share the same bit length
	// and the symbols in order of ascending bit length (subsorted by ascending symbol value).
	// This ends up changing the shape of the tree but preserves its important properties.
	// Additionally, it allows decoding without *really* rebuilding the tree.
	// We can look at say 6 bits in the bitstream. If their value is less than
	// the first entry of the 6-bit codes, it must be a 7-bit or longer code.
	// This operation can be done in a SIMD way, as well:
	// One lane tries 4-bit, one tries 5-bit, etc. The first match tells us the bit length to consider.
	struct CanonicalHuffmanCode {

		explicit CanonicalHuffmanCode(std::vector<uint8_t> code_lengths, std::vector<uint16_t> symbols) noexcept;

		std::vector<uint8_t> code_lengths_;
		std::vector<uint16_t> symbols_;

	};
	CanonicalHuffmanCode GetCanonicalHuffmanCodes(const NaiveHuffmanNode* root) noexcept;


	struct SymbolEncoding {

		SymbolEncoding() noexcept;
		explicit SymbolEncoding(uint32_t traversal_path, uint8_t code_length) noexcept;

		uint32_t traversal_path_;
		uint8_t code_length_;

	};
	std::array<SymbolEncoding, 256> ReconstituteSymbolEncodings(const CanonicalHuffmanCode& canonical_huffman_codes) noexcept;


	struct CompressResult {

		explicit CompressResult(std::vector<uint8_t> compressed_buffer, uint8_t leftover_bits) noexcept;

		std::vector<uint8_t> compressed_buffer_;
		uint8_t leftover_bits_;

	};
	CompressResult CompressHuffman(std::span<uint8_t> uncompressed_buffer, const std::array<SymbolEncoding, 256>& symbol_encodings) noexcept;


	std::unique_ptr<NaiveHuffmanNode> ReconstituteHuffmanTree(const std::array<SymbolEncoding, 256>& symbol_encodings) noexcept;


	std::vector<uint8_t> DecompressHuffman(const CompressResult& compress_result, const NaiveHuffmanNode* root) noexcept;

	std::vector<uint8_t> DecompressHuffman(const CompressResult& compress_result, const CanonicalHuffmanCode& canonical_huffman_codes) noexcept;


}; // namespace maxCompression

#endif // #ifndef MAXCOMPRESSION_NAIVEHUFFMAN_HPP