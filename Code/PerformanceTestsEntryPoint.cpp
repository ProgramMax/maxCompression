// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <array>
#include <cstdint>
#include <span>
#include <string_view>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <zlib.h>

#include "NaiveHuffman.hpp"
#include "NaiveLempelZiv.hpp"

namespace {

	auto uncompressed_string = std::string_view{"The only verdict is vengeance; a vendetta, held as a votive, not in vain, for the value and veracity of such shall one day vindicate the vigilant and the virtuous. V: Verily, this vichyssoise of verbiage veers most verbose, so let me simply add that it's my very good honor to meet you and you may call me V."};
	auto uncompressed_span = std::span<uint8_t>{(uint8_t*)(uncompressed_string.data()), uncompressed_string.length()};


	void CountOccurrences(benchmark::State& state) {
		for (auto _ : state) {
			maxCompression::CountOccurrences(uncompressed_span);
		}
	}
	BENCHMARK(CountOccurrences);

	void CreateHuffmanTree(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);

		for (auto _ : state) {
			maxCompression::CreateHuffmanTree(occurances);
		}
	}
	BENCHMARK(CreateHuffmanTree);

	void GetCanonicalHuffmanCodes(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);

		for (auto _ : state) {
			maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		}
	}
	BENCHMARK(GetCanonicalHuffmanCodes);

	void ReconstituteSymbolEncodings(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());

		for (auto _ : state) {
			maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);
		}
	}
	BENCHMARK(ReconstituteSymbolEncodings);

	void CompressHuffman(benchmark::State& state) {
		auto occurances = maxCompression::CountOccurrences(uncompressed_span);
		auto huffman_tree = maxCompression::CreateHuffmanTree(occurances);
		auto canonical_huffman_codes = maxCompression::GetCanonicalHuffmanCodes(huffman_tree.get());
		auto symbol_encodings = maxCompression::ReconstituteSymbolEncodings(canonical_huffman_codes);

		for (auto _ : state) {
			maxCompression::CompressHuffman(uncompressed_span, symbol_encodings);
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
			maxCompression::ReconstituteHuffmanTree(symbol_encodings);
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
			maxCompression::DecompressHuffman(compressed_result, reconstituted_huffman_tree.get());
		}
	}
	BENCHMARK(DecompressHuffman);

	void CompressLempelZiv(benchmark::State& state) {
		for (auto _ : state) {
			maxCompression::CompressLempelZiv(uncompressed_span, 2048, 3);
		}
	}
	BENCHMARK(CompressLempelZiv);

	void DecompressLempelZiv(benchmark::State& state) {
		auto compressed_result = maxCompression::CompressLempelZiv(uncompressed_span, 2048, 3);

		for (auto _ : state) {
			maxCompression::DecompressLempelZiv(compressed_result);
		}
	}
	BENCHMARK(DecompressLempelZiv);

	void CompressZlib(benchmark::State& state) {
		auto compress_buffer = std::array<uint8_t, 4 * 1024>{};

		for (auto _ : state) {
			auto compression_state = z_stream{};
			deflateInit(&compression_state, /*level=*/1);
			compression_state.avail_in = uncompressed_span.size();
			compression_state.avail_out = compress_buffer.size();
			compression_state.next_in = uncompressed_span.data();
			compression_state.next_out = compress_buffer.data();

			deflate(&compression_state, Z_FINISH);

			deflateEnd(&compression_state);
		}
	}
	BENCHMARK(CompressZlib);

	void DecompressZlib(benchmark::State& state) {
		auto compress_buffer = std::array<uint8_t, 4 * 1024>{};

		auto compression_state = z_stream{};
		deflateInit(&compression_state, /*level=*/1);
		compression_state.avail_in = uncompressed_span.size();
		compression_state.avail_out = compress_buffer.size();
		compression_state.next_in = uncompressed_span.data();
		compression_state.next_out = compress_buffer.data();

		deflate(&compression_state, Z_FINISH);

		deflateEnd(&compression_state);

		auto decompress_buffer = std::array<uint8_t, 4 * 1024>{};

		for (auto _ : state) {
			auto decompression_state = z_stream{};
			inflateInit(&decompression_state);
			decompression_state.avail_in = compression_state.total_out;
			decompression_state.avail_out = decompress_buffer.size();
			decompression_state.next_in = compress_buffer.data();
			decompression_state.next_out = decompress_buffer.data();

			inflate(&decompression_state, Z_SYNC_FLUSH);

			inflateEnd(&decompression_state);
		}
	}
	BENCHMARK(DecompressZlib);

} // anonymous namespace

BENCHMARK_MAIN();
