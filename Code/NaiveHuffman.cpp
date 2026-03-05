// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "NaiveHuffman.hpp"

#include <algorithm>
#include <stack>
#include <utility>

namespace maxCompression {


	std::array<uint32_t, 256> CountOccurrences(std::span<uint8_t> uncompressed_data) noexcept {
		auto occurrences = std::array<uint32_t, 256>{};

		for (auto& byte : uncompressed_data) {
			occurrences[byte]++;
		}

		return occurrences;
	}


	NaiveHuffmanNode::NaiveHuffmanNode(uint32_t count) noexcept
		: count_(std::move(count))
	{}

	NaiveHuffmanNode::NaiveHuffmanNode(uint32_t count, uint8_t value) noexcept
		: count_(std::move(count))
		, value_(std::move(value))
	{}

	NaiveHuffmanNode::NaiveHuffmanNode(NaiveHuffmanNode&& rhs) noexcept = default;


	std::unique_ptr<NaiveHuffmanNode> CreateHuffmanTree(const std::array<uint32_t, 256>& occurrences) noexcept {
		// First, eliminate any non-zero count elements
		std::vector<std::unique_ptr<NaiveHuffmanNode>> nodes;

		for (auto i = int{0}; i < occurrences.size(); i++) {
			auto count = occurrences[i];

			if (count != 0) {
				nodes.emplace_back(std::make_unique<NaiveHuffmanNode>(count, i));
			}
		}


		// Then, sort those nodes by count ascending.
		auto predicate = [](const std::unique_ptr<NaiveHuffmanNode>& lhs, const std::unique_ptr<NaiveHuffmanNode>& rhs) noexcept {
			return lhs->count_ > rhs->count_;
		};

		std::sort(std::begin(nodes), std::end(nodes), predicate);


		// Merge the two nodes with the lowest counts.
		// Keep doing this until there is only one node left.
		while (nodes.size() > 1) {
			auto node_count = nodes.size();
			// This order fills child nodes right-to-left
			auto left = std::move(nodes[node_count - 1]);
			auto right = std::move(nodes[node_count - 2]);

			auto merged_node = std::make_unique<NaiveHuffmanNode>(left->count_ + right->count_);
			merged_node->left_ = std::move(left);
			merged_node->right_ = std::move(right);

			nodes.pop_back();
			nodes.pop_back();

			nodes.emplace_back(std::move(merged_node));

			// Sort the list of nodes again so the merged node goes where it belongs.
			std::sort(std::begin(nodes), std::end(nodes), predicate);
		}

		return std::move(nodes[0]);
	}

	CanonicalHuffmanCode::CanonicalHuffmanCode(std::vector<uint8_t> code_lengths, std::vector<uint8_t> symbols) noexcept
		: code_lengths_(std::move(code_lengths))
		, symbols_(std::move(symbols))
	{}

	CanonicalHuffmanCode GetCanonicalHuffmanCodes(const NaiveHuffmanNode* root) noexcept {
		struct SymbolAndLength {
			explicit SymbolAndLength(uint8_t symbol, uint8_t length) noexcept
				: symbol_(std::move(symbol))
				, length_(std::move(length))
			{}

			uint8_t symbol_;
			uint8_t length_;
		};

		std::vector<SymbolAndLength> used_symbols;


		std::stack<uint8_t> depths;
		std::stack<const NaiveHuffmanNode*> depth_first_stack;
		depths.push(0);
		depth_first_stack.push(root);

		while (!depth_first_stack.empty()) {
			auto current_node = depth_first_stack.top();
			depth_first_stack.pop();
			auto current_depth = depths.top();
			depths.pop();

			if (current_node->value_.has_value()) {
				used_symbols.emplace_back(current_node->value_.value(), current_depth);
			}
			if (current_node->left_) {
				depths.push(current_depth + 1);
				depth_first_stack.push(current_node->left_.get());
			}
			if (current_node->right_) {
				depths.push(current_depth + 1);
				depth_first_stack.push(current_node->right_.get());
			}
		}

		// Sort those symbols first by length, second by symbol value
		std::sort(std::begin(used_symbols), std::end(used_symbols), [](const SymbolAndLength& lhs, const SymbolAndLength& rhs) noexcept {
			if (lhs.length_ == rhs.length_)	{
				return lhs.symbol_ < rhs.symbol_;
			}

			return lhs.length_ < rhs.length_;
		});

		// Calculate how many symbols share a given bit length
		std::vector<uint8_t> bit_length_occurances;
		auto max_bit_length = used_symbols[used_symbols.size() - 1].length_;
		auto current_symbol_index = size_t{0};

		// Add zeros for all depths with no used symbols
		for (auto i = size_t{1}; i <= max_bit_length; i++) {
			// Add zeroes for empty rows
			bit_length_occurances.emplace_back(0);
			if (used_symbols[current_symbol_index].length_ != i) {
				continue;
			}

			// Count up how many occurances match this bit length
			while (current_symbol_index < used_symbols.size() && used_symbols[current_symbol_index].length_ == i) {
				bit_length_occurances[i - 1]++;
				current_symbol_index++;
			}
		}

		// Create a vector of just those symbol values, preserving the sorted order
		std::vector<uint8_t> symbols;
		for (auto& symbol : used_symbols) {
			symbols.emplace_back(symbol.symbol_);
		}

		return CanonicalHuffmanCode{std::move(bit_length_occurances), std::move(symbols)};
	}

	SymbolEncoding::SymbolEncoding() noexcept
		: traversal_path_(0)
		, code_length_(0)
	{}

	SymbolEncoding::SymbolEncoding(uint32_t traversal_path, uint8_t code_length) noexcept
		: traversal_path_(std::move(traversal_path))
		, code_length_(std::move(code_length))
	{}

	std::array<SymbolEncoding, 256> ReconstituteSymbolEncodings(const CanonicalHuffmanCode& canonical_huffman_codes) noexcept {
		std::array<SymbolEncoding, 256> symbol_encodings{};

		auto symbol_index = size_t{0};
		auto working_symbol_encoding = SymbolEncoding{}; 

		auto code_length_size = canonical_huffman_codes.code_lengths_.size();
		for (auto i = size_t{0}; i < code_length_size; i++) {
			// At each new code length size, first shift the working encoding left
			working_symbol_encoding.code_length_++;
			working_symbol_encoding.traversal_path_ <<= 1;

			auto symbols_at_this_code_length = canonical_huffman_codes.code_lengths_[i];
			for (auto j = size_t{0}; j < symbols_at_this_code_length; j++) {
				auto current_symbol = canonical_huffman_codes.symbols_[symbol_index++];
				symbol_encodings[current_symbol] = working_symbol_encoding;
				working_symbol_encoding.traversal_path_++;
			}
		}

		return symbol_encodings;
	}


	CompressResult::CompressResult(std::vector<uint8_t> compressed_buffer, uint8_t leftover_bits) noexcept
		: compressed_buffer_(std::move(compressed_buffer))
		, leftover_bits_(std::move(leftover_bits))
	{}


	CompressResult CompressHuffman(std::span<uint8_t> uncompressed_buffer, const std::array<SymbolEncoding, 256>& symbol_encodings) noexcept {
		auto compressed_buffer = std::vector<uint8_t>{};
		auto current_byte = uint8_t{0};
		auto current_byte_length = uint8_t{0};

		for (auto& current_symbol : uncompressed_buffer) {
			auto& symbol_encoding = symbol_encodings[current_symbol];

			uint8_t bits_available = 8 - current_byte_length;
			auto bits_to_consume = std::min(symbol_encoding.code_length_, bits_available);

			auto bits_to_add = symbol_encoding.traversal_path_ << (bits_available - bits_to_consume);
			auto remaining_bits = std::max(0, symbol_encoding.code_length_ - bits_to_consume);
			bits_to_add >>= remaining_bits;
			current_byte |= bits_to_add;
			current_byte_length += bits_to_consume;

			if (current_byte_length == 8) {
				compressed_buffer.emplace_back(std::move(current_byte));

				auto bits_left_over = symbol_encoding.code_length_ - bits_to_consume;
				auto bits_left_over_mask = uint8_t{0b1111'1111} >> (8 - bits_left_over);


				current_byte = (symbol_encoding.traversal_path_ & bits_left_over_mask) << (8 - bits_left_over);
				current_byte_length = bits_left_over;
			}
		}

		// Add final byte if there are bits used
		if (current_byte_length) {
			compressed_buffer.emplace_back(std::move(current_byte));
		}

		// TODO: Also return the remaining bits so they can be ignored when decompressing
		return CompressResult{std::move(compressed_buffer), static_cast<uint8_t>(8 - current_byte_length)};
	}

	std::unique_ptr<NaiveHuffmanNode> ReconstituteHuffmanTree(const std::array<SymbolEncoding, 256>& symbol_encodings) noexcept {
		// Constructing with the value 0 doesn't really matter.
		// We cannot reconstruct the count from the encodings.
		// This will be done any time we need to construct a node in this function.
		auto root = std::make_unique<NaiveHuffmanNode>(0);

		auto symbol_encoding_count = symbol_encodings.size();
		for (auto i = size_t{0}; i < symbol_encoding_count; i++) {
			auto& symbol_encoding = symbol_encodings[i];
			if (symbol_encoding.code_length_ == 0) {
				continue;
			}

			auto current_node = root.get();

			// Loop over the code, traversing down the tree (creating nodes if necessary)
			auto code_length = symbol_encoding.code_length_;
			for (auto j = code_length; j > 0; j--) {
				auto direction = (symbol_encoding.traversal_path_ >> (j - 1)) & 1;

				if (direction) {
					if (!current_node->right_) {
						current_node->right_ = std::make_unique<NaiveHuffmanNode>(0);
					}
					current_node = current_node->right_.get();
				} else {
					if (!current_node->left_) {
						current_node->left_ = std::make_unique<NaiveHuffmanNode>(0);
					}
					current_node = current_node->left_.get();
				}
			}

			current_node->value_ = static_cast<uint8_t>(i);
		}

		return root;
	}


	std::vector<uint8_t> DecompressHuffman(const CompressResult& compress_result, const NaiveHuffmanNode* root) noexcept {
		auto& compressed_buffer = compress_result.compressed_buffer_;
		auto decompressed_buffer = std::vector<uint8_t>{};

		const NaiveHuffmanNode* current_node = root;

		auto buffer_size = compressed_buffer.size();
		for (size_t i = 0; i < buffer_size; i++) {
			auto& compressed_byte = compressed_buffer[i];
			auto byte_shift = 7;

			auto byte_shift_terminator = -1;
			if (i == buffer_size - 1) {
				byte_shift_terminator = compress_result.leftover_bits_ - 1;
			}

			while (byte_shift > byte_shift_terminator) {
				auto current_bit = (compressed_byte >> byte_shift) & 1;
				current_node = current_bit ? current_node->right_.get() : current_node->left_.get();
				if (current_node->value_.has_value()) {
					decompressed_buffer.emplace_back(current_node->value_.value());
					current_node = root;
				}
				byte_shift--;
			}
		}

		return decompressed_buffer;
	}

} // namespace maxCompression
