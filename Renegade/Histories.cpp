#include "Histories.h"

Histories::Histories() {
	ContinuationHistory = std::make_unique<Continuations>();
	ClearAll();
}

void Histories::ClearAll() {
	ClearKillerAndCounterMoves();
	std::memset(&HistoryTables, 0, sizeof(HistoryTables));
    std::memset(&CorrectionHistory, 0, sizeof(CorrectionHistory));
	std::memset(ContinuationHistory.get(), 0, sizeof(Continuations));
}

void Histories::ClearKillerAndCounterMoves() {
	std::memset(&KillerMoves, 0, sizeof(KillerMoves));
	std::memset(&CounterMoves, 0, sizeof(CounterMoves));
}

// Killer and countermoves ------------------------------------------------------------------------

void Histories::AddKillerMove(const Move& move, const int level) {
	if (level >= MaxDepth) return;
	if (IsFirstKillerMove(move, level)) return;
	if (IsSecondKillerMove(move, level)) {
		KillerMoves[level][1] = KillerMoves[level][0];
		KillerMoves[level][0] = move;
		return;
	}
	KillerMoves[level][1] = KillerMoves[level][0];
	KillerMoves[level][0] = move;
}

bool Histories::IsKillerMove(const Move& move, const int level) const {
	if (KillerMoves[level][0] == move) return true;
	if (KillerMoves[level][1] == move) return true;
	return false;
}

bool Histories::IsFirstKillerMove(const Move& move, const int level) const {
	return KillerMoves[level][0] == move;
}

bool Histories::IsSecondKillerMove(const Move& move, const int level) const {
	return KillerMoves[level][1] == move;
}

void Histories::ResetKillersForPly(const int level) {
	KillerMoves[level][0] = EmptyMove;
	KillerMoves[level][1] = EmptyMove;
}

void Histories::AddCountermove(const Move& previousMove, const Move& thisMove) {
	if (!previousMove.IsNull()) CounterMoves[previousMove.from][previousMove.to] = thisMove;
}

bool Histories::IsCountermove(const Move& previousMove, const Move& thisMove) const {
	return CounterMoves[previousMove.from][previousMove.to] == thisMove;
}

// History heuristic ------------------------------------------------------------------------------

void Histories::UpdateHistory(const Position& position, const Move& m, const uint8_t piece, const int16_t delta, const int level) {

	// Main quiet history
	const bool side = ColorOfPiece(piece) == PieceColor::White;
	const bool fromSquareAttacked = position.IsSquareThreatened(m.from);
	const bool toSquareAttacked = position.IsSquareThreatened(m.to);
	UpdateHistoryValue(HistoryTables[piece][m.to][fromSquareAttacked][toSquareAttacked], delta);

	// Continuation history
	for (const int ply : { 1, 2, 4 }) {
		if (level < ply) break;
		const auto& [prevMove, prevPiece] = position.GetPreviousMove(ply);
		if (prevPiece != Piece::None) {
			int16_t& value = (*ContinuationHistory)[prevPiece][prevMove.to][piece][m.to];
			UpdateHistoryValue(value, delta);
		}
	}
}

int Histories::GetHistoryScore(const Position& position, const Move& m, const uint8_t movedPiece, const int level) const {
	const bool fromSquareThreatened = position.IsSquareThreatened(m.from);
	const bool toSquareThreatened = position.IsSquareThreatened(m.to);
	int historyScore = HistoryTables[movedPiece][m.to][fromSquareThreatened][toSquareThreatened];

	for (const int ply : { 1, 2, 4 }) {
		if (level < ply) break;
		historyScore += (*ContinuationHistory)[position.GetPreviousMove(ply).piece][position.GetPreviousMove(ply).move.to][movedPiece][m.to];
	}
	return historyScore;
}

// Static evaluation correction history -----------------------------------------------------------

void Histories::UpdateCorrection(const Position& position, const int16_t rawEval, const int16_t score, const int depth, const bool exactNode) {
    // Constants based on a slightly older build of Alexandria
    const int key = position.PawnHash() & (CorrectionEntryCount - 1);
    const int target = std::clamp(score - rawEval, -256, 256) * CorrectionGranularity;
    constexpr int correctionInertia = 4096;

    const int targetWeight = exactNode ? std::min(4 * depth * depth + 8 * depth + 4, 512)
                                       : std::min(3 * depth * depth + 6 * depth + 3, 384);

    // Linearly interpolate between the current and the "move towards" value
    // unlike gravity, there's no explicit force to center the values around zero
    constexpr int correctionHistoryCap = CorrectionCap * CorrectionGranularity;
    int32_t& entry = CorrectionHistory[position.Turn()][key];
    entry = ((correctionInertia - targetWeight) * entry + targetWeight * target) / correctionInertia;
    entry = std::clamp(entry, -correctionHistoryCap, correctionHistoryCap);
}

int Histories::AdjustStaticEvaluation(const Position& position, const int16_t rawEval) const {
    constexpr int correctionThreshold = 8192;

    if (std::abs(rawEval) > correctionThreshold) return rawEval;

    const int key = position.PawnHash() & (CorrectionEntryCount - 1);
    const int corrected = rawEval + CorrectionHistory[position.Turn()][key] / CorrectionGranularity;
    return std::clamp(corrected, -MateThreshold + 1, MateThreshold - 1);
}
