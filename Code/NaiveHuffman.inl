// Copyright 2026, The maxCompression Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

namespace maxCompression {

	template<typename SymbolDecodedCallback>
	void DecompressHuffmanUpToSymbolCount(BitReader& bit_reader, const CanonicalHuffmanCodeTree& canonical_huffman_code_tree, size_t up_to_symbol_count, SymbolDecodedCallback symbol_decoded_callback) noexcept {
		const auto tree_layer_size = canonical_huffman_code_tree.layers_.size();
		for (auto symbols_left = up_to_symbol_count; symbols_left != 0; ) {

			auto tree_layer_index = size_t{0};
			auto code = uint16_t{0};

			for ( ; tree_layer_index < tree_layer_size; tree_layer_index++) {
				code <<= 1;
				code |= bit_reader.ReadBit();
				if (code < canonical_huffman_code_tree.layers_[tree_layer_index].first_code_word_ + canonical_huffman_code_tree.layers_[tree_layer_index].symbols_.size()) {
					// This is the layer of the tree
					break;
				}
			}

			auto symbol_index = code - canonical_huffman_code_tree.layers_[tree_layer_index].first_code_word_;
			symbols_left -= symbol_decoded_callback(canonical_huffman_code_tree.layers_[tree_layer_index].symbols_[symbol_index]);
		}
	}

	template<typename SymbolDecodedCallback>
	void DecompressHuffman(BitReader& bit_reader, const CanonicalHuffmanCodeTree& canonical_huffman_code_tree, SymbolDecodedCallback symbol_decoded_callback) noexcept {
		const auto tree_layer_size = canonical_huffman_code_tree.layers_.size();

		bool continue_looping = true;
		while (continue_looping) {
			auto tree_layer_index = size_t{0};
			auto code = uint16_t{0};

			for ( ; tree_layer_index < tree_layer_size; tree_layer_index++) {
				code <<= 1;
				code |= bit_reader.ReadBit();
				if (code < canonical_huffman_code_tree.layers_[tree_layer_index].first_code_word_ + canonical_huffman_code_tree.layers_[tree_layer_index].symbols_.size()) {
					// This is the layer of the tree
					break;
				}
			}

			auto symbol_index = code - canonical_huffman_code_tree.layers_[tree_layer_index].first_code_word_;
			continue_looping = symbol_decoded_callback(canonical_huffman_code_tree.layers_[tree_layer_index].symbols_[symbol_index]);
		};
	}

} // namespace maxCompression
