#pragma once
#include "Board.h"
#include "Move.h"

extern uint64_t GetBishopAttacks(const int square, const uint64_t occupancy);
extern uint64_t GetRookAttacks(const int square, const uint64_t occupancy);
extern uint64_t GetQueenAttacks(const int square, const uint64_t occupancy);

struct EvaluationFeatures {

	// Weight size and its array
	static constexpr int WeightSize = 572;
	TaperedScore Weights[WeightSize];

	// King safety constants
	static constexpr int DangerMultipliers[8] = { 0, 50, 70, 80, 90, 95, 98, 100 };
	static constexpr int PieceDangers[7] = { 0, 1, 2, 2, 3, 5, 4 };

	// Indices (these are here to be easily used for tuning)
	constexpr int IndexPieceMaterial(const uint8_t pieceType) const { return pieceType - 1; }
	constexpr int IndexPSQT(const uint8_t pieceType, const uint8_t sq) const { return 6 + (pieceType-1) * 64 + sq; }
	constexpr int IndexKnightMobility(const uint8_t mobility) const { return 390 + mobility; }
	constexpr int IndexBishopMobility(const uint8_t mobility) const { return 399 + mobility; }
	constexpr int IndexRookMobility(const uint8_t mobility) const { return 413 + mobility; }
	constexpr int IndexQueenMobility(const uint8_t mobility) const { return 428 + mobility; }
	constexpr int IndexKingDanger(const uint8_t danger) const { return 455 + danger; }
	const int IndexPassedPawn(const uint8_t rank) const { return 481 + rank; }
	const int IndexBlockedPasser(const uint8_t rank) const { return 489 + rank; }
	const int IndexIsolatedPawn(const uint8_t file) const { return 497 + file; }
	const int IndexDoubledPawns = 505;
	const int IndexTripledPawns = 506;
	const int IndexBishopPair = 507;
	const int IndexRookOnOpenFile = 508;
	const int IndexRookOnSemiOpenFile = 509;
	const int IndexKnightOutpost = 510;
	const int IndexTempoBonus = 513;
	constexpr int IndexPawnThreats(const uint8_t attackedPieceType) const { return 513 + attackedPieceType; };
	constexpr int IndexKnightThreats(const uint8_t attackedPieceType) const { return 519 + attackedPieceType; };
	constexpr int IndexBishopThreats(const uint8_t attackedPieceType) const { return 525 + attackedPieceType; };
	constexpr int IndexRookThreats(const uint8_t attackedPieceType) const { return 531 + attackedPieceType; };
	constexpr int IndexQueenThreats(const uint8_t attackedPieceType) const { return 537 + attackedPieceType; };
	constexpr int IndexKingThreats(const uint8_t attackedPieceType) const { return 543 + attackedPieceType; };
	constexpr int IndexPawnSupportingPawn(const uint8_t rank) const { return 550 + rank; };
	constexpr int IndexPawnPhalanx(const uint8_t rank) const { return 558 + rank; };
	constexpr int IndexPawnSupportingPiece(const uint8_t pieceType) const { return 565 + pieceType; };

	// Shorthand for retrieving the evaluation
	inline const TaperedScore& GetMaterial(const uint8_t pieceType) const { return Weights[IndexPieceMaterial(pieceType)]; }
	inline const TaperedScore& GetPSQT(const uint8_t pieceType, const uint8_t sq) const { return Weights[IndexPSQT(pieceType, sq)]; }
	inline const TaperedScore& GetKnightMobility(const uint8_t mobility) const { return Weights[IndexKnightMobility(mobility)]; }
	inline const TaperedScore& GetBishopMobility(const uint8_t mobility) const { return Weights[IndexBishopMobility(mobility)]; }
	inline const TaperedScore& GetRookMobility(const uint8_t mobility) const { return Weights[IndexRookMobility(mobility)]; }
	inline const TaperedScore& GetQueenMobility(const uint8_t mobility) const { return Weights[IndexQueenMobility(mobility)]; }
	inline const TaperedScore& GetKingDanger(const uint8_t danger) const { return Weights[IndexKingDanger(danger)]; }
	inline const TaperedScore& GetPassedPawnEval(const uint8_t rank) const { return Weights[IndexPassedPawn(rank)]; }
	inline const TaperedScore& GetBlockedPasserEval(const uint8_t rank) const { return Weights[IndexBlockedPasser(rank)]; }
	inline const TaperedScore& GetIsolatedPawnEval(const uint8_t file) const { return Weights[IndexIsolatedPawn(file)]; }
	inline const TaperedScore& GetDoubledPawnEval() const { return Weights[IndexDoubledPawns]; }
	inline const TaperedScore& GetTripledPawnEval() const { return Weights[IndexTripledPawns]; }
	inline const TaperedScore& GetBishopPairEval() const { return Weights[IndexBishopPair]; }
	inline const TaperedScore& GetRookOnOpenFileEval() const { return Weights[IndexRookOnOpenFile]; }
	inline const TaperedScore& GetRookOnSemiOpenFileEval() const { return Weights[IndexRookOnSemiOpenFile]; }
	inline const TaperedScore& GetKnightOutpostEval() const { return Weights[IndexKnightOutpost]; }
	inline const TaperedScore& GetTempoBonus() const { return Weights[IndexTempoBonus]; }
	inline const TaperedScore& GetPawnThreat(const uint8_t attackedPieceType) const { return Weights[IndexPawnThreats(attackedPieceType)]; }
	inline const TaperedScore& GetKnightThreat(const uint8_t attackedPieceType) const { return Weights[IndexKnightThreats(attackedPieceType)]; }
	inline const TaperedScore& GetBishopThreat(const uint8_t attackedPieceType) const { return Weights[IndexBishopThreats(attackedPieceType)]; }
	inline const TaperedScore& GetRookThreat(const uint8_t attackedPieceType) const { return Weights[IndexRookThreats(attackedPieceType)]; }
	inline const TaperedScore& GetQueenThreat(const uint8_t attackedPieceType) const { return Weights[IndexQueenThreats(attackedPieceType)]; }
	inline const TaperedScore& GetKingThreat(const uint8_t attackedPieceType) const { return Weights[IndexKingThreats(attackedPieceType)]; }
	inline const TaperedScore& GetPawnSupportingPawn(const uint8_t rank) const { return Weights[IndexPawnSupportingPawn(rank)]; }
	inline const TaperedScore& GetPawnPhalanx(const uint8_t rank) const { return Weights[IndexPawnPhalanx(rank)]; }
	inline const TaperedScore& GetPawnSupportingPiece(const uint8_t pieceType) const { return Weights[IndexPawnSupportingPiece(pieceType)]; }

};


typedef TaperedScore S; // using S as tapered score seems somewhat standard

static EvaluationFeatures Weights = {

	// 1. Material values (pawn, knight, bishop, rook, queen, king)
	S(85, 80), S(325, 289), S(330, 318), S(444, 560), S(998, 1016), S(0, 0),

	// 2. Piece-square tables
	// Be careful, counter-intuitively the 1st element corresponds to white's bottom-left corner

	// 2.1 Pawn PSQT
	S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(-19, 24), S(-22, 32), S(-14, 25), S(-18, 30), S(-3, 34), S(18, 21), S(26, 14), S(-6, 9),
	S(-22, 18), S(-22, 25), S(-13, 15), S(-12, 21), S(-1, 20), S(-3, 19), S(14, 14), S(-3, 6),
	S(-21, 23), S(-23, 33), S(-12, 14), S(3, 9), S(1, 11), S(4, 10), S(-5, 22), S(-5, 6),
	S(-17, 48), S(-14, 47), S(-5, 31), S(-2, 17), S(18, 12), S(22, 13), S(13, 33), S(7, 25),
	S(6, 112), S(-1, 129), S(26, 98), S(37, 75), S(39, 65), S(71, 59), S(44, 102), S(20, 91),
	S(96, 184), S(93, 175), S(69, 191), S(110, 144), S(62, 155), S(80, 140), S(-9, 195), S(-9, 196),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),

	// 2.2 Knight PSQT
	S(-80, -29), S(-40, -14), S(-41, -3), S(-23, 1), S(-22, 0), S(-15, -9), S(-39, -2), S(-56, -13),
	S(-49, -14), S(-39, 0), S(-22, 4), S(-6, 6), S(-6, 6), S(-9, 2), S(-21, -12), S(-22, -3),
	S(-40, -10), S(-21, 5), S(-4, 13), S(5, 20), S(17, 21), S(4, 8), S(0, 3), S(-16, -7),
	S(-21, 7), S(-8, 12), S(11, 25), S(16, 24), S(23, 31), S(19, 13), S(20, 9), S(-9, 0),
	S(-9, -1), S(3, 18), S(24, 27), S(50, 24), S(32, 21), S(54, 19), S(16, 11), S(23, -7),
	S(-23, -4), S(21, 1), S(51, 9), S(62, 6), S(67, 3), S(78, -3), S(42, -13), S(1, -14),
	S(-47, -7), S(-7, -10), S(21, -11), S(23, -2), S(17, -15), S(52, -6), S(23, -24), S(12, -27),
	S(-118, -80), S(-98, -30), S(-47, -9), S(-32, -15), S(-22, -4), S(-72, -18), S(-88, -15), S(-102, -81),

	// 2.3 Bishop PSQT
	S(-8, -15), S(16, -4), S(-5, -3), S(-10, -1), S(-7, -3), S(-12, 10), S(10, -12), S(8, -30),
	S(4, -9), S(7, -15), S(18, -16), S(0, 0), S(7, 0), S(16, -7), S(25, -5), S(9, -18),
	S(-10, -5), S(12, 1), S(11, 3), S(11, 6), S(14, 14), S(12, 4), S(9, 0), S(10, -14),
	S(-14, -4), S(-11, 3), S(4, 8), S(30, 5), S(25, 3), S(4, 4), S(-4, 3), S(2, -24),
	S(-13, -4), S(0, 2), S(12, 3), S(36, 8), S(27, 0), S(22, 3), S(-4, 4), S(-8, -9),
	S(-2, 1), S(15, -6), S(18, -3), S(21, -12), S(19, -10), S(53, -4), S(41, -12), S(25, -6),
	S(-25, -13), S(8, -15), S(-6, -12), S(-11, -5), S(8, -22), S(1, -16), S(12, -13), S(-13, -17),
	S(-12, -2), S(-62, 6), S(-17, -6), S(-66, -1), S(-62, -11), S(-62, -8), S(-42, -7), S(-47, -10),

	// 2.4 Rook PSQT
	S(-18, 11), S(-13, 9), S(-6, 10), S(7, 0), S(10, -3), S(7, 3), S(12, -6), S(-17, 1),
	S(-33, 3), S(-22, 7), S(-15, 7), S(-11, 3), S(-3, -4), S(4, -12), S(11, -14), S(-20, -8),
	S(-29, 9), S(-29, 8), S(-21, 7), S(-15, 4), S(-8, -2), S(-7, -4), S(17, -17), S(5, -19),
	S(-24, 14), S(-22, 16), S(-19, 15), S(-15, 14), S(-8, 10), S(-20, 9), S(-2, 3), S(-13, 2),
	S(-13, 24), S(1, 16), S(-6, 25), S(2, 16), S(9, 3), S(16, 2), S(16, 6), S(10, 2),
	S(-10, 18), S(18, 15), S(15, 15), S(6, 14), S(39, 2), S(41, 0), S(72, -2), S(42, -3),
	S(-13, 22), S(-7, 29), S(0, 30), S(19, 20), S(7, 19), S(32, 11), S(32, 12), S(24, 12),
	S(-16, 29), S(4, 19), S(5, 29), S(-5, 28), S(-7, 26), S(14, 24), S(40, 16), S(32, 16),

	// 2.5 Queen PSQT
	S(-10, -19), S(-9, -16), S(0, -16), S(2, 6), S(4, -5), S(-7, -19), S(3, -14), S(-14, -20),
	S(-11, -11), S(-4, -16), S(2, -12), S(7, -2), S(6, 3), S(14, -21), S(10, -27), S(30, -64),
	S(-12, -5), S(-5, 4), S(-5, 15), S(-10, 18), S(-2, 25), S(4, 13), S(8, 16), S(1, 16),
	S(-12, 3), S(-12, 5), S(-9, 13), S(-4, 34), S(-4, 35), S(-5, 34), S(8, 15), S(4, 39),
	S(-16, 18), S(-10, 19), S(-7, 18), S(-12, 39), S(1, 35), S(1, 50), S(12, 41), S(17, 23),
	S(-1, 8), S(-6, 13), S(-8, 36), S(-7, 44), S(10, 42), S(52, 45), S(68, 4), S(43, 44),
	S(-17, 8), S(-26, 12), S(-11, 26), S(-16, 46), S(1, 56), S(13, 34), S(3, 27), S(47, 31),
	S(-38, 19), S(-11, 2), S(0, 25), S(-14, 53), S(11, 29), S(27, 25), S(53, -5), S(0, 20),

	// 2.6 King PSQT
	S(41, -86), S(67, -62), S(40, -43), S(-48, -25), S(2, -40), S(-27, -24), S(44, -55), S(43, -86),
	S(53, -51), S(17, -19), S(4, -7), S(-27, 2), S(-35, 7), S(-20, 2), S(28, -20), S(30, -40),
	S(-22, -34), S(11, -11), S(-29, 13), S(-51, 28), S(-39, 27), S(-45, 20), S(-16, -2), S(-51, -13),
	S(-31, -34), S(-34, -2), S(-49, 29), S(-62, 43), S(-75, 46), S(-30, 30), S(-36, 13), S(-64, -11),
	S(-21, -21), S(-22, 7), S(-42, 33), S(-61, 46), S(-53, 45), S(-33, 39), S(-20, 18), S(-28, -8),
	S(-44, -18), S(8, 10), S(-38, 31), S(-38, 40), S(-23, 42), S(-8, 45), S(0, 28), S(-3, -2),
	S(-45, -29), S(-13, 4), S(-42, 14), S(-5, 14), S(-18, 26), S(3, 31), S(8, 24), S(18, -11),
	S(-43, -92), S(-29, -48), S(-21, -36), S(-51, -4), S(-26, -15), S(4, -19), S(8, -8), S(-21, -90),

	// 3. Piece mobility
	// Score tables depending on the pseudolegal moves available
	
	// 3.1 Knight mobility (0-8)
	S(-19, -24), S(-2, 4), S(8, 18), S(11, 28), S(15, 33), S(20, 38), S(26, 36), S(31, 31), S(31, 24),
	
	// 3.2 Bishop mobility (0-13)
	S(-21, -42), S(-11, -24), S(-1, -8), S(6, 6), S(12, 16), S(16, 28), S(20, 32),
	S(21, 35), S(22, 41), S(26, 36), S(32, 33), S(40, 31), S(37, 40), S(38, 24),

	// 3.3 Rook mobility (0-14)
	S(-17, -24), S(-8, -8), S(-4, -3), S(0, 0), S(0, 12), S(7, 16), S(9, 19), S(15, 19),
	S(18, 23), S(22, 28), S(25, 29), S(24, 34), S(34, 30), S(39, 27), S(41, 29),

	// 3.4 Queen mobility (0-27)
	S(-6, -89), S(-3, -115), S(-9, -92), S(-6, -69), S(-6, -55), S(-3, -38), S(-1, -22), S(1, -16), S(2, 0),
	S(6, -1), S(7, 5), S(10, 14), S(11, 21), S(12, 27), S(11, 38), S(14, 42), S(11, 58), S(17, 61),
	S(22, 65), S(36, 58), S(31, 77), S(74, 54), S(61, 66), S(72, 63), S(120, 70), S(98, 57), S(98, 79), S(84, 60),

	// 4. King safety (1-25 danger points)
	// Danger points are given for attacks near the king, and then scaled according to the attacker count

	S(-5, -5), S(-5, -5), S(-20, -20), S(-25, -25), S(-36, -36), 
	S(-56, -56), S(-72, -72), S(-90, -90), S(-133, -133), S(-190, -190),
	S(-222, -222), S(-252, -252), S(-255, -255), S(-178, -178), S(-322, -322),
	S(-332, -332), S(-350, -350), S(-370, -370), S(-400, -400), S(-422, -422),
	S(-425, -425), S(-430, -430), S(-435, -435), S(-440, -440), S(-445, -445),
	
	// 5. Pawn structure
	// Collection of features to evaluate the pawn structure
	
	// 5.1 Passed pawns by rank
	S(0, 0), S(-13, 14), S(-15, 19), S(-9, 46), S(16, 65), S(26, 78), S(41, 62), S(0, 0),
	
	// 5.2 Blocked passed pawn penalties by rank
	S(0, 0), S(-28, 4), S(-21, 4), S(-20, -14), S(-10, -35), S(-17, -80), S(-46, -100), S(0, 0),
	
	// 5.3 Isolated pawns by file
	S(-13, 1), S(-12, -9), S(-13, -14), S(-17, -20), S(-18, -18), S(-20, -8), S(-6, -16), S(-5, -3),

	// 5.4 Doubled and tripled pawns
	S(-7, -20), S(-12, -37),
	
	// 6. Misc & piece-specific evaluation

	// 6.1 Bishop pairs
	S(34, 55),

	// 6.2 Rook on open and semi-open file
	S(27, 12), S(9, 9),

	// 6.3 Knight outposts
	S(1, 13),

	// 6.4 Pawn attacking minor and major pieces -- unused
	S(51, 28), S(56, 32),

	// 6.5 Tempo bonus
	S(18, 0),

	// 7. To be sorted

	// 7.1 Threat matrix
	// It somewhat helps to see beyond the horizon, also it ruins nps
	// Threatened king values are 0 due to the tuning dataset lacking such positions

	// Pawn attacking
	S(0, 0), S(47, -1), S(51, 28), S(64, -11), S(46, -13), S(0, 0),
	// Knight attacking
	S(-9, 3), S(0, 0), S(34, 14), S(52, -9), S(38, -24), S(0, 0),
	// Bishop attacking
	S(-6, 6), S(12, 20), S(0, 0), S(40, 9), S(58, 58), S(0, 0),
	// Rook attacking
	S(-6, 4), S(3, 6), S(13, 1), S(0, 0), S(61, 15), S(0, 0),
	// Queen attacking
	S(-6, 12), S(1, 3), S(0, 14), S(1, 7), S(0, 0), S(0, 0),
	// King attacking
	S(15, 27), S(-24, 9), S(-24, 18), S(-24, 3), S(0, 0), S(0, 0),

	// 7.2 Supported pawn bonus (by rank)
	S(0, 0), S(0, 0), S(9, 12), S(5, 4), S(-1, 6), S(7, 2), S(78, 33), S(0, 0),

	// 7.3 Pawn phalanx (by rank)
	S(2, 0), S(7, 0), S(1, -3), S(14, 17), S(36, 36), S(42, 42), S(42, 42), S(0, 0),

	// 7.4 Pawns supporting other pieces (pawn, knight, bishop, rook, queen, king) - todo
	S(3, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};

// Interpolation functions ------------------------------------------------------------------------

static inline int LinearTaper(const int earlyValue, const int lateValue, const float phase) {
	return static_cast<int>((1.f - phase) * earlyValue + phase * lateValue);
}

// Game phase calculation -------------------------------------------------------------------------

static const float CalculateGamePhase(const Board& board) {
	const int remainingPawns = Popcount(board.WhitePawnBits | board.BlackPawnBits);
	const int remainingKnights = Popcount(board.WhiteKnightBits | board.BlackKnightBits);
	const int remainingBishops = Popcount(board.WhiteBishopBits | board.BlackBishopBits);
	const int remainingRooks = Popcount(board.WhiteRookBits | board.BlackRookBits);
	const int remainingQueens = Popcount(board.WhiteQueenBits | board.BlackQueenBits);
	const int remainingScore = remainingPawns + remainingKnights * 10 + remainingBishops * 10 + remainingRooks * 20 + remainingQueens * 40;
	const float phase = (256 - remainingScore) / (256.f);
	return std::clamp(phase, 0.f, 1.f);
}

// Drawish endgame detection ----------------------------------------------------------------------

inline static const bool IsDrawishEndgame(const Board& board, const uint64_t whitePieces, const uint64_t blackPieces) {
	// Drawish endgame detection
	// To avoid simplifying down to a non-winning endgame with a nominal material advantage
	// This list is not complete, and probably should be expanded even more (e.g. by including pawns)
	// Source: Chess Programming Wiki + https://www.madchess.net/2021/04/08/madchess-3-0-beta-4d22dec-endgame-eval-scaling/
	bool endgame = (Popcount(whitePieces) <= 3) && (Popcount(blackPieces) <= 3);
	if (!endgame) return false;

	// Variables for easy access
	bool pawnless = (board.WhitePawnBits | board.BlackPawnBits) == 0ULL;
	bool queenless = (board.WhiteQueenBits | board.BlackQueenBits) == 0ULL;
	bool queenful = (Popcount(board.WhiteQueenBits | board.BlackQueenBits) > 0)
		&& (Popcount(board.WhiteQueenBits) <= 1) && (Popcount(board.BlackQueenBits) <= 1);
	bool potentiallyDrawishQueenless = queenless && pawnless && endgame;
	bool potentiallyDrawishQueenful = queenful && pawnless && endgame;
	int whiteExtras = Popcount(whitePieces) - 1;
	int blackExtras = Popcount(blackPieces) - 1;
	int whiteMinors = Popcount(board.WhiteKnightBits | board.WhiteBishopBits);
	int blackMinors = Popcount(board.BlackKnightBits | board.BlackBishopBits);
	int whiteKnights = Popcount(board.WhiteKnightBits);
	int blackKnights = Popcount(board.BlackKnightBits);
	int whiteBishops = Popcount(board.WhiteBishopBits);
	int blackBishops = Popcount(board.BlackBishopBits);
	int whiteRooks = Popcount(board.WhiteRookBits);
	int blackRooks = Popcount(board.BlackRookBits);

	// Endgames with no queens
	if (potentiallyDrawishQueenless) {
		bool drawish =
			// 2 minor pieces (no bishop pair) vs 1 minor piece
			((whiteExtras == 2) && (blackExtras == 1) && (whiteMinors == 2) && (whiteBishops != 2) && (blackMinors == 1)) ||
			((whiteExtras == 1) && (blackExtras == 2) && (blackMinors == 2) && (whiteMinors == 1) && (blackBishops != 2)) ||
			// 2 knights vs king
			((whiteExtras == 0) && (blackExtras == 2) && (blackKnights == 2)) ||
			((whiteExtras == 2) && (blackExtras == 0) && (whiteKnights == 2)) ||
			// minor piece vs minor piece
			((whiteExtras == 1) && (blackExtras == 1) && (whiteMinors == 1) && (blackMinors == 1)) ||
			// minor piece vs 2 knights
			((whiteExtras == 1) && (blackExtras == 2) && (whiteMinors == 1) && (blackKnights == 2)) ||
			((whiteExtras == 2) && (blackExtras == 1) && (whiteKnights == 2) && (blackMinors == 1)) ||
			// 2 bishop vs 1 bishop
			((whiteExtras == 2) && (blackExtras == 1) && (whiteBishops == 2) && (blackBishops == 1)) ||
			((whiteExtras == 1) && (blackExtras == 2) && (whiteBishops == 1) && (blackBishops == 2)) ||
			// rook vs rook
			((whiteExtras == 1) && (blackExtras == 1) && (whiteRooks == 1) && (blackRooks == 1)) ||
			// 2 rooks vs 2 rooks
			((whiteExtras == 1) && (blackExtras == 1) && (whiteRooks == 2) && (blackRooks == 2)) ||
			// rook vs rook + minor piece
			((whiteExtras == 2) && (blackExtras == 1) && (whiteRooks == 1) && (blackRooks == 1) && (whiteMinors == 1)) ||
			((whiteExtras == 1) && (blackExtras == 2) && (whiteRooks == 1) && (blackRooks == 1) && (blackMinors == 1)) ||
			// 2 rooks vs rook + minor
			((whiteExtras == 2) && (blackExtras == 2) && (whiteRooks == 2) && (blackRooks == 1) && (blackMinors == 1)) ||
			((whiteExtras == 2) && (blackExtras == 2) && (whiteRooks == 1) && (blackRooks == 2) && (whiteMinors == 1));
		if (drawish) return true;
	}

	if (potentiallyDrawishQueenful) {
		int whiteQueens = Popcount(board.WhiteQueenBits);
		int blackQueens = Popcount(board.BlackQueenBits);
		bool drawish =
			// queen vs queen
			((whiteExtras == 1) && (blackExtras == 1) && (whiteQueens == 1) && (blackQueens == 1)) ||
			// queen vs 2 bishops
			((whiteExtras == 1) && (blackExtras == 2) && (whiteQueens == 1) && (blackBishops == 2)) ||
			((whiteExtras == 2) && (blackExtras == 1) && (blackQueens == 1) && (whiteBishops == 2)) ||
			// queen vs 2 knights
			((whiteExtras == 1) && (blackExtras == 2) && (whiteQueens == 1) && (blackKnights == 2)) ||
			((whiteExtras == 2) && (blackExtras == 1) && (blackQueens == 1) && (whiteKnights == 2)) ||
			// queen vs 2 rooks
			((whiteExtras == 1) && (blackExtras == 2) && (whiteQueens == 1) && (blackRooks == 2)) ||
			((whiteExtras == 2) && (blackExtras == 1) && (blackQueens == 1) && (whiteRooks == 2)) ||
			// queen vs rook + minor
			((whiteExtras == 1) && (blackExtras == 2) && (whiteQueens == 1) && (blackRooks == 1) && (blackMinors == 1)) ||
			((whiteExtras == 2) && (blackExtras == 1) && (blackQueens == 1) && (whiteRooks == 1) && (whiteMinors == 1));
		if (drawish) return true;
	}
	return false;
}

// Board evaluation -------------------------------------------------------------------------------

inline static const int EvaluateBoard(const Board& board, const int level, const EvaluationFeatures& weights) {

	// Renegade's definitions in evaluation:
	// - attacks: squares that a piece can perform a capture on (neglecting checks and friendly pieces)
	// - mobility: sqaures that a piece can move to (pseudolegal, for now at least)

	//int score = 0, earlyScore = 0, lateScore = 0;

	TaperedScore taperedScore(0, 0);

	const uint64_t occupancy = board.GetOccupancy();
	const uint64_t whitePieces = board.GetOccupancy(PieceColor::White);
	const uint64_t blackPieces = board.GetOccupancy(PieceColor::Black);
	const float phase = CalculateGamePhase(board);

	const uint64_t whitePawnAttacks = ((board.WhitePawnBits & ~Bitboards::FileA) << 7) | ((board.WhitePawnBits & ~Bitboards::FileH) << 9);
	const uint64_t blackPawnAttacks = ((board.BlackPawnBits & ~Bitboards::FileA) >> 9) | ((board.BlackPawnBits & ~Bitboards::FileH) >> 7);

	uint64_t allOccupancy = occupancy;
	uint64_t whiteAttacks = 0, blackAttacks = 0;

	int whiteDangerScore = 0;
	int blackDangerScore = 0;
	int whiteDangerPieces = 0;
	int blackDangerPieces = 0;

	const int whiteKingSquare = 63 - Lzcount(board.WhiteKingBits);
	const int whiteKingFile = GetSquareFile(whiteKingSquare);
	const int whiteKingRank = GetSquareRank(whiteKingSquare);
	const uint64_t whiteKingZone = KingArea[whiteKingSquare];
	const int blackKingSquare = 63 - Lzcount(board.BlackKingBits);
	const int blackKingFile = GetSquareFile(blackKingSquare);
	const int blackKingRank = GetSquareRank(blackKingSquare);
	const uint64_t blackKingZone = KingArea[blackKingSquare];

	uint64_t piecesOnBoard = occupancy;
	while (piecesOnBoard != 0) {
		const int sq = Popsquare(piecesOnBoard);
		const int piece = board.GetPieceAt(sq);
		const int pieceType = TypeOfPiece(piece);
		const int pieceColor = ColorOfPiece(piece);
		const int file = GetSquareFile(sq);
		const int rank = GetSquareRank(sq);
		uint64_t mobility = 0, attacks = 0;

		// Material and piece-square tables
		if (pieceColor == PieceColor::White) {
			taperedScore += weights.GetMaterial(pieceType);
			taperedScore += weights.GetPSQT(pieceType, sq);
		}
		else {
			taperedScore -= weights.GetMaterial(pieceType);
			taperedScore -= weights.GetPSQT(pieceType, Mirror[sq]);
		}

		// Piece-specific evaluation
		switch (piece) {

		case Piece::WhitePawn:
			// Get attacks & update king danger scores
			attacks = WhitePawnAttacks[sq] & ~whitePieces;
			whiteAttacks |= attacks;
			if ((blackKingZone & attacks) != 0) {
				blackDangerScore += weights.PieceDangers[PieceType::Pawn];
				blackDangerPieces += 1;
			}
			// Isolated pawn evaluation
			if ((board.WhitePawnBits & IsolatedPawnMask[file]) == 0) {
				taperedScore += weights.GetIsolatedPawnEval(file);
			}
			// Passed pawn evaluation
			if (((WhitePassedPawnMask[sq] & board.BlackPawnBits) == 0) && ((WhitePassedPawnFilter[sq] & board.WhitePawnBits) == 0)) {
				taperedScore += weights.GetPassedPawnEval(rank);
				if (SquareBits[sq + 8] & blackPieces) taperedScore += weights.GetBlockedPasserEval(rank);
			}
			// Threats
			if (attacks & board.BlackKnightBits) taperedScore += weights.GetPawnThreat(PieceType::Knight) * Popcount(attacks & board.BlackKnightBits);
			if (attacks & board.BlackBishopBits) taperedScore += weights.GetPawnThreat(PieceType::Bishop) * Popcount(attacks & board.BlackBishopBits);
			if (attacks & board.BlackRookBits) taperedScore += weights.GetPawnThreat(PieceType::Rook) * Popcount(attacks & board.BlackRookBits);
			if (attacks & board.BlackQueenBits) taperedScore += weights.GetPawnThreat(PieceType::Queen) * Popcount(attacks & board.BlackQueenBits);
			// Pawn is supported?
			if (whitePawnAttacks & SquareBits[sq]) taperedScore += weights.GetPawnSupportingPawn(rank);
			// Pawn phalanx
			if ((file != 7) && (board.GetPieceAt(sq + 1) == Piece::WhitePawn)) taperedScore += weights.GetPawnPhalanx(rank);
			break;

		case Piece::BlackPawn:
			// Get attacks & update king danger scores
			attacks = BlackPawnAttacks[sq] & ~blackPieces;
			blackAttacks |= attacks;
			if ((whiteKingZone & attacks) != 0) {
				whiteDangerScore += weights.PieceDangers[PieceType::Pawn];
				whiteDangerPieces += 1;
			}
			// Isolated pawn evaluation
			if ((board.BlackPawnBits & IsolatedPawnMask[file]) == 0) {
				taperedScore -= weights.GetIsolatedPawnEval(7 - file);
			}
			// Passed pawn evaluation
			if (((BlackPassedPawnMask[sq] & board.WhitePawnBits) == 0) && ((BlackPassedPawnFilter[sq] & board.BlackPawnBits) == 0)) {
				taperedScore -= weights.GetPassedPawnEval(7 - rank);
				if (SquareBits[sq - 8] & whitePieces) taperedScore -= weights.GetBlockedPasserEval(7 - rank);
			}
			// Threats
			if (attacks & board.WhiteKnightBits) taperedScore -= weights.GetPawnThreat(PieceType::Knight) * Popcount(attacks & board.WhiteKnightBits);
			if (attacks & board.WhiteBishopBits) taperedScore -= weights.GetPawnThreat(PieceType::Bishop) * Popcount(attacks & board.WhiteBishopBits);
			if (attacks & board.WhiteRookBits) taperedScore -= weights.GetPawnThreat(PieceType::Rook) * Popcount(attacks & board.WhiteRookBits);
			if (attacks & board.WhiteQueenBits) taperedScore -= weights.GetPawnThreat(PieceType::Queen) * Popcount(attacks & board.WhiteQueenBits);
			// Pawn is supported?
			if (blackPawnAttacks & SquareBits[sq]) taperedScore -= weights.GetPawnSupportingPawn(7-rank);
			// Pawn phalanx
			if ((file != 7) && (board.GetPieceAt(sq + 1) == Piece::BlackPawn)) taperedScore -= weights.GetPawnPhalanx(7-rank);
			break;

		case Piece::WhiteKnight:
			// Get attacks, mobility & update king danger scores
			mobility = KnightMoveBits[sq] & ~whitePieces;
			whiteAttacks |= mobility;
			taperedScore += weights.GetKnightMobility(Popcount(mobility & ~blackPawnAttacks));
			if ((blackKingZone & mobility) != 0) {
				blackDangerScore += weights.PieceDangers[PieceType::Knight];
				blackDangerPieces += 1;
			}
			// Knight outposts
			if (OutpostFilter[sq]) {
				if ((board.GetPieceAt(sq - 7) == Piece::WhitePawn) || (board.GetPieceAt(sq - 9) == Piece::WhitePawn)) {
					taperedScore += weights.GetKnightOutpostEval();
				}
			}
			// Threats
			taperedScore += weights.GetKnightThreat(PieceType::Pawn) * Popcount(mobility & board.BlackPawnBits);
			//taperedScore += weights.GetKnightThreat(PieceType::Knight) * Popcount(mobility & board.BlackKnightBits);
			taperedScore += weights.GetKnightThreat(PieceType::Bishop) * Popcount(mobility & board.BlackBishopBits);
			taperedScore += weights.GetKnightThreat(PieceType::Rook) * Popcount(mobility & board.BlackRookBits);
			taperedScore += weights.GetKnightThreat(PieceType::Queen) * Popcount(mobility & board.BlackQueenBits);
			//taperedScore += weights.GetKnightThreat(PieceType::King) * Popcount(mobility & board.BlackKingBits);
			break;

		case Piece::BlackKnight:
			// Get attacks, mobility & update king danger scores
			mobility = KnightMoveBits[sq] & ~blackPieces;
			blackAttacks |= mobility;
			taperedScore -= weights.GetKnightMobility(Popcount(mobility & ~whitePawnAttacks));
			if ((whiteKingZone & mobility) != 0) {
				whiteDangerScore += weights.PieceDangers[PieceType::Knight];
				whiteDangerPieces += 1;
			}
			// Knight outposts
			if (OutpostFilter[sq]) {
				if ((board.GetPieceAt(sq + 7) == Piece::BlackPawn) || (board.GetPieceAt(sq + 9) == Piece::BlackPawn)) {
					taperedScore -= weights.GetKnightOutpostEval();
				}
			}
			// Threats
			taperedScore -= weights.GetKnightThreat(PieceType::Pawn) * Popcount(mobility & board.WhitePawnBits);
			//taperedScore -= weights.GetKnightThreat(PieceType::Knight) * Popcount(mobility & board.WhiteKnightBits);
			taperedScore -= weights.GetKnightThreat(PieceType::Bishop) * Popcount(mobility & board.WhiteBishopBits);
			taperedScore -= weights.GetKnightThreat(PieceType::Rook) * Popcount(mobility & board.WhiteRookBits);
			taperedScore -= weights.GetKnightThreat(PieceType::Queen) * Popcount(mobility & board.WhiteQueenBits);
			//taperedScore -= weights.GetKnightThreat(PieceType::King) * Popcount(mobility & board.WhiteKingBits);
			break;

		case Piece::WhiteBishop:
			// Get attacks, mobility & update king danger scores
			mobility = GetBishopAttacks(sq, occupancy) & ~whitePieces;
			whiteAttacks |= mobility;
			taperedScore += weights.GetBishopMobility(Popcount(mobility & ~blackPawnAttacks));
			if ((blackKingZone & mobility) != 0) {
				blackDangerScore += weights.PieceDangers[PieceType::Bishop];
				blackDangerPieces += 1;
			}
			// Threats
			taperedScore += weights.GetBishopThreat(PieceType::Pawn) * Popcount(mobility & board.BlackPawnBits);
			taperedScore += weights.GetBishopThreat(PieceType::Knight) * Popcount(mobility & board.BlackKnightBits);
			//taperedScore += weights.GetBishopThreat(PieceType::Bishop) * Popcount(mobility & board.BlackBishopBits);
			taperedScore += weights.GetBishopThreat(PieceType::Rook) * Popcount(mobility & board.BlackRookBits);
			taperedScore += weights.GetBishopThreat(PieceType::Queen) * Popcount(mobility & board.BlackQueenBits);
			//taperedScore += weights.GetBishopThreat(PieceType::King) * Popcount(mobility & board.BlackKingBits);
			break;

		case Piece::BlackBishop:
			// Get attacks, mobility & update king danger scores
			mobility = GetBishopAttacks(sq, occupancy) & ~blackPieces;
			blackAttacks |= mobility;
			taperedScore -= weights.GetBishopMobility(Popcount(mobility & ~whitePawnAttacks));
			if ((whiteKingZone & mobility) != 0) {
				whiteDangerScore += weights.PieceDangers[PieceType::Bishop];
				whiteDangerPieces += 1;
			}
			// Threats
			taperedScore -= weights.GetBishopThreat(PieceType::Pawn) * Popcount(mobility & board.WhitePawnBits);
			taperedScore -= weights.GetBishopThreat(PieceType::Knight) * Popcount(mobility & board.WhiteKnightBits);
			//taperedScore -= weights.GetBishopThreat(PieceType::Bishop) * Popcount(mobility & board.WhiteBishopBits);
			taperedScore -= weights.GetBishopThreat(PieceType::Rook) * Popcount(mobility & board.WhiteRookBits);
			taperedScore -= weights.GetBishopThreat(PieceType::Queen) * Popcount(mobility & board.WhiteQueenBits);
			//taperedScore -= weights.GetBishopThreat(PieceType::King) * Popcount(mobility & board.WhiteKingBits);
			break;

		case Piece::WhiteRook:
			// Get attacks, mobility & update king danger scores
			mobility = GetRookAttacks(sq, occupancy) & ~whitePieces;
			whiteAttacks |= mobility;
			taperedScore += weights.GetRookMobility(Popcount(mobility & ~blackPawnAttacks));
			if ((blackKingZone & mobility) != 0) {
				blackDangerScore += weights.PieceDangers[PieceType::Rook];
				blackDangerPieces += 1;
			}
			// Rook on open or semi-open file
			if ((board.WhitePawnBits & Files[file]) == 0) {
				if ((board.BlackPawnBits & Files[file]) == 0) { // open file
					taperedScore += weights.GetRookOnOpenFileEval();
				}
				else { // semi-open file
					taperedScore += weights.GetRookOnSemiOpenFileEval();
				}
			}
			// Threats
			taperedScore += weights.GetRookThreat(PieceType::Pawn) * Popcount(mobility & board.BlackPawnBits);
			taperedScore += weights.GetRookThreat(PieceType::Knight) * Popcount(mobility & board.BlackKnightBits);
			taperedScore += weights.GetRookThreat(PieceType::Bishop) * Popcount(mobility & board.BlackBishopBits);
			//taperedScore += weights.GetRookThreat(PieceType::Rook) * Popcount(mobility & board.BlackRookBits);
			taperedScore += weights.GetRookThreat(PieceType::Queen) * Popcount(mobility & board.BlackQueenBits);
			//taperedScore += weights.GetRookThreat(PieceType::King) * Popcount(mobility & board.BlackKingBits);
			break;

		case Piece::BlackRook:
			// Get attacks, mobility & update king danger scores
			mobility = GetRookAttacks(sq, occupancy) & ~blackPieces;
			blackAttacks |= mobility;
			taperedScore -= weights.GetRookMobility(Popcount(mobility & ~whitePawnAttacks));
			if ((whiteKingZone & mobility) != 0) {
				whiteDangerScore += weights.PieceDangers[PieceType::Rook];
				whiteDangerPieces += 1;
			}
			// Rook on open or semi-open file
			if ((board.BlackPawnBits & Files[file]) == 0) {
				if ((board.WhitePawnBits & Files[file]) == 0) { // open file
					taperedScore -= weights.GetRookOnOpenFileEval();
				}
				else { // semi-open file
					taperedScore -= weights.GetRookOnSemiOpenFileEval();
				}
			}
			// Threats
			taperedScore -= weights.GetRookThreat(PieceType::Pawn) * Popcount(mobility & board.WhitePawnBits);
			taperedScore -= weights.GetRookThreat(PieceType::Knight) * Popcount(mobility & board.WhiteKnightBits);
			taperedScore -= weights.GetRookThreat(PieceType::Bishop) * Popcount(mobility & board.WhiteBishopBits);
			//taperedScore -= weights.GetRookThreat(PieceType::Rook) * Popcount(mobility & board.WhiteRookBits);
			taperedScore -= weights.GetRookThreat(PieceType::Queen) * Popcount(mobility & board.WhiteQueenBits);
			//taperedScore -= weights.GetRookThreat(PieceType::King) * Popcount(mobility & board.WhiteKingBits);
			break;

		case Piece::WhiteQueen:
			// Get attacks, mobility & update king danger scores
			mobility = GetQueenAttacks(sq, occupancy) & ~whitePieces;
			whiteAttacks |= mobility;
			taperedScore += weights.GetQueenMobility(Popcount(mobility & ~blackPawnAttacks));
			if ((blackKingZone & mobility) != 0) {
				blackDangerScore += weights.PieceDangers[PieceType::Queen];
				blackDangerPieces += 1;
			}
			// Threats
			taperedScore += weights.GetQueenThreat(PieceType::Pawn) * Popcount(mobility & board.BlackPawnBits);
			taperedScore += weights.GetQueenThreat(PieceType::Knight) * Popcount(mobility & board.BlackKnightBits);
			taperedScore += weights.GetQueenThreat(PieceType::Bishop) * Popcount(mobility & board.BlackBishopBits);
			taperedScore += weights.GetQueenThreat(PieceType::Rook) * Popcount(mobility & board.BlackRookBits);
			//taperedScore += weights.GetQueenThreat(PieceType::Queen) * Popcount(mobility & board.BlackQueenBits);
			//taperedScore += weights.GetQueenThreat(PieceType::King) * Popcount(mobility & board.BlackKingBits);
			break;

		case Piece::BlackQueen:
			// Get attacks, mobility & update king danger scores
			mobility = GetQueenAttacks(sq, occupancy) & ~blackPieces;
			blackAttacks |= mobility;
			taperedScore -= weights.GetQueenMobility(Popcount(mobility & ~whitePawnAttacks));
			if ((whiteKingZone & mobility) != 0) {
				whiteDangerScore += weights.PieceDangers[PieceType::Queen];
				whiteDangerPieces += 1;
			}
			// Threats
			taperedScore -= weights.GetQueenThreat(PieceType::Pawn) * Popcount(mobility & board.WhitePawnBits);
			taperedScore -= weights.GetQueenThreat(PieceType::Knight) * Popcount(mobility & board.WhiteKnightBits);
			taperedScore -= weights.GetQueenThreat(PieceType::Bishop) * Popcount(mobility & board.WhiteBishopBits);
			taperedScore -= weights.GetQueenThreat(PieceType::Rook) * Popcount(mobility & board.WhiteRookBits);
			//taperedScore -= weights.GetQueenThreat(PieceType::Queen) * Popcount(mobility & board.WhiteQueenBits);
			//taperedScore -= weights.GetQueenThreat(PieceType::King) * Popcount(mobility & board.WhiteKingBits);
			break;

		case Piece::WhiteKing:
			mobility = KingMoveBits[sq] & ~whitePieces;
			// Threats
			taperedScore += weights.GetKingThreat(PieceType::Pawn) * Popcount(mobility & board.BlackPawnBits);
			taperedScore += weights.GetKingThreat(PieceType::Knight) * Popcount(mobility & board.BlackKnightBits);
			taperedScore += weights.GetKingThreat(PieceType::Bishop) * Popcount(mobility & board.BlackBishopBits);
			taperedScore += weights.GetKingThreat(PieceType::Rook) * Popcount(mobility & board.BlackRookBits);
			taperedScore += weights.GetKingThreat(PieceType::Queen) * Popcount(mobility & board.BlackQueenBits);
			//taperedScore += weights.GetKingThreat(PieceType::King) * Popcount(mobility & board.BlackKingBits);
			break;
		case Piece::BlackKing:
			mobility = KingMoveBits[sq] & ~blackPieces;
			taperedScore -= weights.GetKingThreat(PieceType::Pawn) * Popcount(mobility & board.WhitePawnBits);
			taperedScore -= weights.GetKingThreat(PieceType::Knight) * Popcount(mobility & board.WhiteKnightBits);
			taperedScore -= weights.GetKingThreat(PieceType::Bishop) * Popcount(mobility & board.WhiteBishopBits);
			taperedScore -= weights.GetKingThreat(PieceType::Rook) * Popcount(mobility & board.WhiteRookBits);
			taperedScore -= weights.GetKingThreat(PieceType::Queen) * Popcount(mobility & board.WhiteQueenBits);
			//taperedScore -= weights.GetKingThreat(PieceType::King) * Popcount(mobility & board.WhiteKingBits);
			break;
		}
	}

	// King safety
	const int whiteKingSafetyFinal = whiteDangerScore * weights.DangerMultipliers[std::min(whiteDangerPieces, 7)] / 100;
	const int blackKingSafetyFinal = blackDangerScore * weights.DangerMultipliers[std::min(blackDangerPieces, 7)] / 100;
	if (whiteKingSafetyFinal != 0) taperedScore += weights.GetKingDanger(std::min(whiteKingSafetyFinal, 25));
	if (blackKingSafetyFinal != 0) taperedScore -= weights.GetKingDanger(std::min(blackKingSafetyFinal, 25));

	// Bishop pair bonus
	if (Popcount(board.WhiteBishopBits) >= 2) taperedScore += weights.GetBishopPairEval();
	if (Popcount(board.BlackBishopBits) >= 2) taperedScore -= weights.GetBishopPairEval();

	// Doubled & tripled pawn penalties
	for (int i = 0; i < 8; i++) {
		const int whitePawnsOnFile = Popcount(board.WhitePawnBits & Files[i]);
		const int blackPawnsOnFile = Popcount(board.BlackPawnBits & Files[i]);
		if (whitePawnsOnFile == 2) taperedScore += weights.GetDoubledPawnEval();
		else if (whitePawnsOnFile > 2) taperedScore += weights.GetTripledPawnEval();
		if (blackPawnsOnFile == 2) taperedScore -= weights.GetDoubledPawnEval();
		else if (blackPawnsOnFile > 2) taperedScore -= weights.GetTripledPawnEval();
	}

	// Get untapered score
	int score = LinearTaper(taperedScore.early, taperedScore.late, phase);

	// Convert to the correct perspective
	if (!board.Turn) score *= -1;

	// Tempo bonus
	score += LinearTaper(weights.GetTempoBonus().early, weights.GetTempoBonus().late, phase);

	// Drawish endgame
	if (IsDrawishEndgame(board, whitePieces, blackPieces)) score /= 8;

	return score;
}

inline static const int EvaluateBoard(const Board& board, const int level) {
	return EvaluateBoard(board, level, Weights);
}
