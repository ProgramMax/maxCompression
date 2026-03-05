// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
#include <span>
#include <string_view>

#include <benchmark/registration.h>
#include <benchmark/state.h>

#include "NaiveHuffman.hpp"

namespace {

	auto uncompressed_string = std::string_view{"The only verdict is vengeance; a vendetta, held as a votive, not in vain, for the value and veracity of such shall one day vindicate the vigilant and the virtuous. V: Verily, this vichyssoise of verbiage veers most verbose, so let me simply add that it's my very good honor to meet you and you may call me V."};
	auto uncompressed_span = std::span<uint8_t>{(uint8_t*)(uncompressed_string.data()), uncompressed_string.length()};


	void CountOccurrences(benchmark::State& state) {
		for (auto _ : state) {
			auto occurrences = maxCompression::CountOccurrences(uncompressed_span);
		}
	}
	BENCHMARK(CountOccurrences);

	void CreateHuffmanTree(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);

		for (auto _ : state) {
			auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		}
	}
	BENCHMARK(CreateHuffmanTree);

	void GetCanonicalHuffmanCodes(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);

		for (auto _ : state) {
			auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		}
	}
	BENCHMARK(GetCanonicalHuffmanCodes);

	void ReconstituteSymbolEncodings(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());

		for (auto _ : state) {
			auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);
		}
	}
	BENCHMARK(ReconstituteSymbolEncodings);

	void CompressHuffman(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);

		for (auto _ : state) {
			auto compressed_result = maxCompression::CompressHuffman(uncompressed_span, symbol_encodings);
		}
	}
	BENCHMARK(CompressHuffman);

	void ReconstituteHuffmanTree(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);
		auto compressed_result = maxCompression::CompressHuffman(uncompressed_span, symbol_encodings);

		for (auto _ : state) {
			auto reconstituted_huffman_tree = maxCompression::ReconstituteHuffmanTree(symbol_encodings);
		}
	}
	BENCHMARK(ReconstituteHuffmanTree);

	void DecompressHuffman(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);
		auto compressed_result = maxCompression::CompressHuffman(uncompressed_span, symbol_encodings);
		auto reconstituted_huffman_tree = maxCompression::ReconstituteHuffmanTree(symbol_encodings);

		for (auto _ : state) {
			auto decompressed_buffer = maxCompression::DecompressHuffman(compressed_result, reconstituted_huffman_tree.get());
		}
	}
	BENCHMARK(DecompressHuffman);

} // anonymous namespace

BENCHMARK_MAIN();
