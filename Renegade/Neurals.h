#pragma once
#include "Board.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <immintrin.h>
#include <memory>

// An early attempt at supporting NNUE evaluation.
// Not meant to be as a permanent solution in its current form.

// Network constants
constexpr int FeatureSize = 768;
constexpr int HiddenSize = 512;

struct alignas(64) NetworkRepresentation {
	std::array<std::array<int16_t, HiddenSize>, FeatureSize> FeatureWeights;
	std::array<int16_t, HiddenSize> FeatureBias;
	std::array<int16_t, HiddenSize * 2> OutputWeights;
	int16_t OutputBias;
};

extern const NetworkRepresentation* Network;

struct alignas(64) AccumulatorRepresentation {
	std::array<int16_t, HiddenSize> White;
	std::array<int16_t, HiddenSize> Black;

	void Reset() {
		for (int i = 0; i < HiddenSize; i++) White[i] = Network->FeatureBias[i];
		for (int i = 0; i < HiddenSize; i++) Black[i] = Network->FeatureBias[i];
	}

	void AddFeature(const int whiteFeature, const int blackFeature) {
		for (int i = 0; i < HiddenSize; i++) White[i] += Network->FeatureWeights[whiteFeature][i];
		for (int i = 0; i < HiddenSize; i++) Black[i] += Network->FeatureWeights[blackFeature][i];
	}

	void RemoveFeature(const int whiteFeature, const int blackFeature) {
		for (int i = 0; i < HiddenSize; i++) White[i] -= Network->FeatureWeights[whiteFeature][i];
		for (int i = 0; i < HiddenSize; i++) Black[i] -= Network->FeatureWeights[blackFeature][i];
	}

	template<bool add> void UpdateFeature(const int whiteFeature, const int blackFeature) {
		if constexpr (add) AddFeature(whiteFeature, blackFeature);
		else RemoveFeature(whiteFeature, blackFeature);
	}

	template<bool add> void UpdateFeature(const std::pair<int, int>& features) {
		if constexpr (add) AddFeature(features.first, features.second);
		else RemoveFeature(features.first, features.second);
	}
};

void LoadDefaultNetwork();
void LoadExternalNetwork(const std::string& filename);
int NeuralEvaluate(const Board &board);
int NeuralEvaluate2(const AccumulatorRepresentation& acc, const bool turn);

inline int32_t ClippedReLU(const int16_t value) {
	return std::clamp<int32_t>(value, 0, 255);
}

inline __m256i SquareClippedReLU(__m256i value) {
	const auto min = _mm256_setzero_si256();
	const auto max = _mm256_set1_epi16(static_cast<int16_t>(181));
	value = _mm256_min_epi16(_mm256_max_epi16(value, min), max);
	return _mm256_mullo_epi16(value, value);
}

inline int16_t SquareClippedReLU(const int16_t value) {
	const int32_t x = std::clamp<int32_t>(value, 0, 181);
	return x * x;
}

inline int32_t ExtractInteger(const __m256i sum) {
	const auto upper_128 = _mm256_extracti128_si256(sum, 1);
	const auto lower_128 = _mm256_castsi256_si128(sum);
	const auto sum_128 = _mm_add_epi32(upper_128, lower_128);
	const auto upper_64 = _mm_unpackhi_epi64(sum_128, sum_128);
	const auto sum_64 = _mm_add_epi32(upper_64, sum_128);
	const auto upper_32 = _mm_shuffle_epi32(sum_64, 0b00'00'00'01);
	const auto sum_32 = _mm_add_epi32(upper_32, sum_64);
	return _mm_cvtsi128_si32(sum_32);
}

inline std::pair<int, int> FeatureIndexes(const uint8_t piece, const uint8_t sq) {
	const uint8_t pieceColor = ColorOfPiece(piece);
	const uint8_t pieceType = TypeOfPiece(piece);
	const int colorOffset = 64 * 6;

	const int whiteFeatureIndex = (pieceColor == PieceColor::White ? 0 : colorOffset) + (pieceType - 1) * 64 + sq;
	const int blackFeatureIndex = (pieceColor == PieceColor::Black ? 0 : colorOffset) + (pieceType - 1) * 64 + Mirror(sq);
	return { whiteFeatureIndex, blackFeatureIndex };
}