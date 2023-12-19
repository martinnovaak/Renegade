#pragma once
#include "Move.h"
#include "Magics.h"
#include "Utils.h"
#include <algorithm>

/*
* This is the board representation of Renegade.
* It also includes logic for move generation and handles repetition checks as well.
* A generic bitboard approach is used, pseudolegal moves are filtered via attack map generation.
*/

// Magic lookup tables
extern uint64_t GetBishopAttacks(const int square, const uint64_t occupancy);
extern uint64_t GetRookAttacks(const int square, const uint64_t occupancy);
extern uint64_t GetQueenAttacks(const int square, const uint64_t occupancy);

class Board
{

public:
	Board(const std::string& fen);
	Board() : Board(FEN::StartPos) {};
	Board(const Board& b);

	void Push(const Move& move);
	bool PushUci(const std::string& ucistr);

	uint64_t GetOccupancy() const;
	uint64_t GetOccupancy(const uint8_t pieceColor) const;
	uint8_t GetPieceAt(const uint8_t place) const;
	uint64_t Hash() const;
	uint64_t HashInternal();

	void GenerateMoves(std::vector<Move>& moves, const MoveGen moveGen, const Legality legality);
	uint64_t CalculateAttackedSquares(const uint8_t colorOfPieces) const;
	bool IsLegalMove(const Move& m);
	bool IsMoveQuiet(const Move& move) const;
	template <bool attackingSide> bool IsSquareAttacked(const uint8_t square) const;

	bool AreThereLegalMoves();
	bool IsDraw(const bool threefold) const;
	GameState GetGameState();
	int GetPlys() const;
	const std::string GetFEN() const;
	bool IsInCheck() const;

	template <bool side> uint8_t GetKingSquare() const;
	uint64_t GetAttackersOfSquare(const uint8_t square, const uint64_t occupied) const;

	template <bool side>
	inline uint64_t GetPawnAttacks() const {
		// For whatever reason there's some trouble with using templates
		if constexpr (side == Turn::White) return ((WhitePawnBits & ~Bitboards::FileA) << 7) | ((WhitePawnBits & ~Bitboards::FileH) << 9);
		else if (side == Turn::Black) return ((BlackPawnBits & ~Bitboards::FileA) >> 9) | ((BlackPawnBits & ~Bitboards::FileH) >> 7);
	}

	inline uint64_t GetWhitePawnAttacks() const {
		return GetPawnAttacks<Turn::White>();
	}

	inline uint64_t GetBlackPawnAttacks() const {
		return GetPawnAttacks<Turn::Black>();
	}

	inline bool ShouldNullMovePrune() const {
		const int friendlyPieces = (Turn == Turn::White) ? Popcount(GetOccupancy(PieceColor::White)) : Popcount(GetOccupancy(PieceColor::Black));
		const int friendlyPawns = (Turn == Turn::White) ? Popcount(WhitePawnBits) : Popcount(BlackPawnBits);
		return (friendlyPieces - friendlyPawns) > 1;
	}


	// Board variables:
	std::vector<uint64_t> PreviousHashes;
	uint8_t OccupancyInts[64];
	uint64_t WhitePawnBits;
	uint64_t WhiteKnightBits;
	uint64_t WhiteBishopBits;
	uint64_t WhiteRookBits;
	uint64_t WhiteQueenBits;
	uint64_t WhiteKingBits;
	uint64_t BlackPawnBits;
	uint64_t BlackKnightBits;
	uint64_t BlackBishopBits;
	uint64_t BlackRookBits;
	uint64_t BlackQueenBits;
	uint64_t BlackKingBits;
	uint64_t HashValue;
	uint16_t FullmoveClock;
	int8_t EnPassantSquare;
	uint8_t HalfmoveClock;
	bool WhiteRightToShortCastle;
	bool WhiteRightToLongCastle;
	bool BlackRightToShortCastle;
	bool BlackRightToLongCastle;
	bool Turn;

private:
	void GenerateOccupancy();
	void TryMove(const Move& move);

	template <bool side, MoveGen moveGen> void GeneratePseudolegalMoves(std::vector<Move>& moves) const;
	template <bool side, MoveGen moveGen> void GenerateKnightMoves(std::vector<Move>& moves, const int home) const;
	template <bool side, MoveGen moveGen> void GenerateKingMoves(std::vector<Move>& moves, const int home) const;
	template <bool side, MoveGen moveGen> void GeneratePawnMoves(std::vector<Move>& moves, const int home) const;
	template <bool side> void GenerateCastlingMoves(std::vector<Move>& moves) const;
	template <bool side, int pieceType, MoveGen moveGen> void GenerateSlidingMoves(std::vector<Move>& moves, const int home, const uint64_t whiteOccupancy, const uint64_t blackOccupancy) const;

	uint64_t GenerateKnightAttacks(const int from) const;
	uint64_t GenerateKingAttacks(const int from) const;
	
};

