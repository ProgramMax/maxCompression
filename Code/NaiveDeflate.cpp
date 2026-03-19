// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "NaiveDeflate.hpp"
#include "BitReader.hpp"

#include <array>

namespace {

	void DeflateWithFixedHuffmanCodes(maxCompression::BitReader& bit_reader, uint16_t /*window_size*/, std::vector<uint8_t>& decompressed_buffer)noexcept {
		while (true) {
			auto huffman_code = uint32_t{0};

			auto literal_value = uint16_t{0};

			for (auto code_length = uint8_t{1}; code_length < 10; code_length++) {
				auto bit = bit_reader.ReadBit();
				huffman_code = (huffman_code << 1) | bit;

				if (code_length == 7 && huffman_code <= 0b001'0111) {
					literal_value = 256 + huffman_code;
					break;
				} else if (code_length == 8) {
					if (huffman_code >= 0b0011'0000 && huffman_code <= 0b1011'1111) {
						literal_value = huffman_code - 0b0011'0000;
						break;
					} else if (huffman_code >= 0b1100'0000 && huffman_code <= 0b1100'0111) {
						literal_value = (huffman_code - 0b1100'0000) + 280;
						break;
					}
				} else if (code_length == 9) {
					literal_value = (huffman_code - 0b1'1001'0000) + 144;
					break;
				}
			}

			if (literal_value < 256) {
				decompressed_buffer.emplace_back(literal_value);
			} else if (literal_value == 256) {
				// end of block
				break;
			} else if (literal_value > 256) {
				// This is a length code for Lempel-Ziv
				// It could include up to 5 extra bits to be read.
				auto extra_bits = uint8_t{0};
				if (literal_value > 281) { // Read the potential 5th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 277) { // Read the potential 4th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 273) { // Read the potential 3rd bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 269) { // Read the potential 2nd bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 265 && literal_value < 285) { // Read the potential 1st bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}

				// Now use those extra bits to find the length
				constexpr auto base_lengths = std::array<uint16_t, 29>{3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 285};
				// TODO: Make sure the literal value is within range
				auto base_length = base_lengths[literal_value - 257];

				auto length = static_cast<uint16_t>(base_length + extra_bits);


				// In fixed Huffman codes, all distance codes are 5-bit length
				literal_value = 0;
				for (auto i = uint8_t{0}; i < 5; i++) {
					auto bit = bit_reader.ReadBit();
					literal_value = (literal_value << 1) | bit;
				}

				constexpr auto base_distances = std::array<uint16_t, 30>{1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16375, 24577};
				// TODO: Make sure the literal value is within range
				auto base_distance = base_distances[literal_value];

				// The distance also has extra bits
				extra_bits = 0;
				if (literal_value > 28) { // Read the potential 13th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 26) { // Read the potential 12th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 24) { // Read the potential 11th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 22) { // Read the potential 10th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 20) { // Read the potential 9th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 18) { // Read the potential 8th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 16) { // Read the potential 7th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 14) { // Read the potential 6th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 12) { // Read the potential 5th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 10) { // Read the potential 4th bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 8) { // Read the potential 3rd bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 6) { // Read the potential 2nd bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}
				if (literal_value > 4) { // Read the potential 1st bit
					auto bit = bit_reader.ReadBit();
					extra_bits = (extra_bits << 1) | bit;
				}


				auto distance = static_cast<uint16_t>(base_distance + extra_bits);

				for (auto i = size_t{0}; i < length; i++) {
					decompressed_buffer.emplace_back(decompressed_buffer[decompressed_buffer.size() - distance]);
				}
			}
		}
	}

} // anonymous namespace

namespace maxCompression {

	std::vector<uint8_t> NaiveDeflateCompress(const std::span<uint8_t>& /*uncompressed_buffer*/) noexcept {
		// Deflate uses 2 Huffman trees: One for literals & Lempel-Ziv lengths, the other for Lempel-Ziv distances.
		// In the literal & length tree, we store the symbols 0-255, a 256 EndOfBlock, and 257-285 lengths.
		// In the distance tree we store 0-29 distance codes.

		// When a length is stored, it might have extra bits appended.
		// RFC 1951 §3.2.5 has a table to describe this: https://www.rfc-editor.org/rfc/rfc1951#page-11

		// code  extra bits   length(s)
		//  257           0           3
		//  258           0           4
		//  259           0           5
		//  260           0           6
		//  261           0           7
		//  262           0           8
		//  263           0           9
		//  264           0          10
		//  265           1      11, 12
		//  266           1      13, 14
		//  267           1      15, 16
		//  268           1      17, 18
		//  269           2       19-22
		//  270           2       23-26
		//  271           2       27-30
		//  272           2       31-34
		//  273           3       35-42
		//  274           3       43-50
		//  275           3       51-58
		//  276           3       59-66
		//  277           4       67-82
		//  278           4       83-98
		//  279           4      99-114
		//  280           4     115-130
		//  281           5     131-162
		//  282           5     163-194
		//  283           5     195-226
		//  284           5     227-257
		//  285           0         258

		// The same extra bits are used in the distance tree as well:

		// code  extra bits  distance(s)
		//    0           0            1
		//    1           0            2
		//    2           0            3
		//    3           0            4
		//    4           1         5, 6
		//    5           1         7, 8
		//    6           2         9-12
		//    7           2        13-16
		//    8           3        17-24
		//    9           3        25-32
		//   10           4        33-48
		//   11           4        49-64
		//   12           5        65-96
		//   13           5       97-128
		//   14           6      129-192
		//   15           6      193-256
		//   16           7      257-384
		//   17           7      385-512
		//   18           8      513-768
		//   19           8     769-1024
		//   20           9    1025-1536
		//   21           9    1537-2048
		//   22          10    2049-3072
		//   23          10    3073-4096
		//   24          11    4097-6144
		//   25          11    6145-8192
		//   26          12   8193-12288
		//   27          12  12289-16384
		//   28          13  16375-24576
		//   29          13  24577-32768

		// Note: RFC 1951 says these extra bits are stored most-significant bit first.
		// But that is incorrect and was fixed in an errata: https://www.rfc-editor.org/errata/eid7764

		// Using the example "abracadabra" would produce {"abracad", (len 4, dist 7), EndOfBlock}
		// For the literal & length tree, length of 4 produces code 258.
		// The distance 7 produces code 5 with the extra bit set to 0.




		// Block header:
		// First bit indicates if this is the final block.
		// Next two bits indicate the type of block:
		// 00 - uncompressed
		// 01 - compressed with fixed Huffman codes
		// 10 - compressed with dynamic Huffman codes
		// 11 - reserved (error)

		// Uncompressed block header:
		// 16-bit length
		// 16-bit one's complement of the length
		// #length bytes of data

		// Fixed Huffman codes:
		// symbol  bits  codes
		// 0-143  8  0011'0000 - 1011'1111
		// 144-255 9 1100'1000'0 - 1111'1111'1
		// 256-279 7 0000'000  - 0010'111
		// 280-287 8 1100'0000 - 1100'0111
		// Note: 286-287 are not used but participate in the code construction

		// The distance table is 0-31, all fixed-length 5-bit codes.
		// Note: 30-31 will not be used.

		// When we get to decoding the dynamic distance table, if there is only 1 distance code of 0 bits
		// that means there are no distance codes used at all. Said another way, the data is all literals.

		// Dynamic Huffman codes header:
		// 5 bits: HLIT: number of literal & length codes - 257 (257-286)
		// 5 bits: number of distance codes - 1 (1-32)
		// 4 bits: HCLEN: number of codde length codes - 4 (4-19)
		// (#HCLEN + 4) * 3 bits: code lengths for the compressed literal & length tree
		// These are in the order: 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
		// These are interpreted as 3-bit integers, 0-7. 0 means the value is unused.
		// HLIT + 257 code lengths

		
		return std::vector<uint8_t>{};
	}

	void NaiveDeflateDecompress(BitReader& bit_reader, uint16_t window_size, std::vector<uint8_t>& decompressed_buffer) noexcept {
		auto is_last_block = false;
		do {
			is_last_block = static_cast<bool>(bit_reader.ReadBit());

			auto block_type_low_bit = bit_reader.ReadBit();
			auto block_type_high_bit = bit_reader.ReadBit();
			auto block_type = (block_type_high_bit << 1) | block_type_low_bit;
			switch (block_type) {
				case 0b00: // uncompressed
					break;
				case 0b01: // compressed with fixed Huffman codes
					return DeflateWithFixedHuffmanCodes(bit_reader, window_size, decompressed_buffer);
				case 0b10: // compressed with dynamic Huffman codes
					break;
				case 0b11: // reserved
					break;
			}
		} while (!is_last_block);
	}

} // namespace maxCompression