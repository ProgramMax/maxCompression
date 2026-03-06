// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "NaiveHuffman.hpp"
#include "NaiveLZ77.hpp"

namespace {

	void PrintHuffmanTree(maxCompression::NaiveHuffmanNode* root, uint8_t indentation = 0) noexcept {
		for (auto i = uint8_t{0}; i < indentation; i++) {
			std::cout << ' ';
		}

		if (root->value_.has_value()) {
			std::cout << root->value_.value();
		} else {
			std::cout << "<nil>";
		}
		std::cout << ',' << ' ' << root->count_ << std::endl;

		if (root->left_) {
			PrintHuffmanTree(root->left_.get(), indentation + 1);
		}
		if (root->right_) {
			PrintHuffmanTree(root->right_.get(), indentation + 1);
		}
	}

	void PrintCanonicalHuffmanCodes(const maxCompression::CanonicalHuffmanCode& canonical_huffman_codes) noexcept {
		std::cout << '(';

		auto code_lengths_size = canonical_huffman_codes.code_lengths_.size();
		for (size_t i = 0; i < code_lengths_size; i++) {
			std::cout << int{canonical_huffman_codes.code_lengths_[i]};

			if (i != code_lengths_size - 1) {
				std::cout << ',';
			}
		}

		std::cout << "), (";

		auto symbols_size = canonical_huffman_codes.symbols_.size();
		for (size_t i = 0; i < symbols_size; i++) {
			std::cout << canonical_huffman_codes.symbols_[i];

			if (i != symbols_size - 1) {
				std::cout << ',';
			}
		}

		std::cout << ')';
	}

	void PrintSymbolEncodings(const std::array<maxCompression::SymbolEncoding, 256>& symbol_encodings) noexcept {
		for (auto i = size_t{0}; i < symbol_encodings.size(); i++) {
			auto& symbol_encoding = symbol_encodings[i];

			if (symbol_encoding.code_length_ == 0) {
				continue;
			}

			std::cout << static_cast<char>(i) << ": ";
			for (auto j = size_t{0}; j < symbol_encoding.code_length_; j++) {
				std::cout << (symbol_encoding.traversal_path_ >> (8 - symbol_encoding.code_length_ + j) & 1);
			}
			std::cout << '\n';
		}
	}

	void PrintCompressedBuffer(const maxCompression::CompressResult& compress_result) noexcept {
		auto buffer_size = compress_result.compressed_buffer_.size();
		for (size_t i = 0; i < buffer_size; i++) {
			auto& byte = compress_result.compressed_buffer_[i];

			std::cout << ((byte >> 7) & 1);
			std::cout << ((byte >> 6) & 1);
			std::cout << ((byte >> 5) & 1);
			std::cout << ((byte >> 4) & 1);
			std::cout << ' ';
			std::cout << ((byte >> 3) & 1);
			std::cout << ((byte >> 2) & 1);
			std::cout << ((byte >> 1) & 1);
			std::cout << ((byte >> 0) & 1);

			if (i != buffer_size - 1) {
				std::cout << ' ';
			}
		}

		std::cout << ", " << static_cast<int>(compress_result.leftover_bits_);
	}

	void PrintDecompressedBuffer(std::span<uint8_t> buffer) noexcept {
		for (auto& byte : buffer) {
			std::cout << byte;
		}
	}

	void TestHuffman() noexcept {
		auto uncompressed_string = std::string_view{"Hello world!"};
		auto uncompressed_span = std::span<uint8_t>{(uint8_t*)(uncompressed_string.data()), uncompressed_string.length()};


		auto occurrences = maxCompression::CountOccurrences(std::move(uncompressed_span));
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurrences);

		PrintHuffmanTree(huffman_tree.get());
		std::cout << std::endl;

		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		PrintCanonicalHuffmanCodes(canonical_huffman_codes);
		std::cout << std::endl;

		auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);
		PrintSymbolEncodings(symbol_encodings);
		std::cout << std::endl;

		auto compressed_result = maxCompression::CompressHuffman(uncompressed_span, symbol_encodings);
		PrintCompressedBuffer(compressed_result);
		std::cout << std::endl;

		auto reconstituted_huffman_tree = maxCompression::ReconstituteHuffmanTree(symbol_encodings);
		PrintHuffmanTree(reconstituted_huffman_tree.get());
		std::cout << std::endl;

		auto decompressed_buffer = maxCompression::DecompressHuffman(compressed_result, reconstituted_huffman_tree.get());
		PrintDecompressedBuffer(decompressed_buffer);
		std::cout << std::endl;
	}

	template<class... Ts>
	struct overloads : Ts... { using Ts::operator()...; };

	void PrintSegments(const std::vector<maxCompression::Segment>& segments) noexcept {
		const auto visitor = overloads
		{
			[](std::span<uint8_t> span) noexcept {
				for (auto element : span) {
					std::cout << element;
				}
			},
			[](maxCompression::DistanceAndLength distance_and_length) noexcept {
				std::cout << '(' << distance_and_length.distance_ << ", " << distance_and_length.length_ << ')';
			},
		};

		for (auto& segment : segments) {
			std::visit(visitor, segment);
		}
	}

	void TestLZ77() noexcept {
		//auto uncompressed_string = std::string_view{"Blah blah blah blah blah!"};
		auto uncompressed_string = std::string_view{
R"(I am Sam

Sam I am

That Sam-I-am!
That Sam-I-am!
I do not like
that Sam-I-am!

Do you like green eggs and ham?

I do not like them, Sam-I-am.
I do not like green eggs and ham.)"};
		auto uncompressed_span = std::span<uint8_t>{(uint8_t*)(uncompressed_string.data()), uncompressed_string.length()};

		auto segments = maxCompression::LZ77Compress(uncompressed_span, 2048, 3);
		PrintSegments(segments);
		std::cout << std::endl;
	}

} // anonymous namespace

int main() noexcept {
	TestHuffman();
	TestLZ77();

	return 0;
}