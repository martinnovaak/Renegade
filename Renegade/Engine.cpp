#include "Engine.h"

typedef std::chrono::high_resolution_clock Clock;

Engine::Engine() {
	EvaluatedNodes = 0;
	HashSize = 125000; // 1 MB
	InitOpeningBook();
}

void Engine::perft(string fen, int depth, bool verbose) {
	Board board = Board(fen);
	perft(board, depth, verbose);
}

void Engine::perft(Board board, int depth, bool verbose) {
	Board b = board.Copy();
	b.DrawCheck = false;
	auto t0 = Clock::now();
	int r = perft1(b, depth, verbose);
	auto t1 = Clock::now();
	float seconds = (t1 - t0).count() / 1e9;
	float speed = r / seconds / 1000;
	if (verbose) cout << "Perft(" << depth << ") = " << r << "  | " << std::setprecision(3) << std::fixed << seconds << " s | " << std::setprecision(1) << speed << "kN/s" << endl;
	else cout << r << endl;
}

int Engine::perft1(Board board, int depth, bool verbose) {
	std::vector<unsigned __int64> moves = board.GenerateLegalMoves(board.Turn);
	if (verbose) cout << "Legal moves (" << moves.size() << "): " << endl;
	int count = 0;
	for (unsigned __int64 m : moves) {
		Board b = board.Copy();
		b.Push(m);
		int r;
		if (depth == 1) {
			r = 1;
			count += r;
		} else {
			r = perftRecursive(b, depth - 1);
			count += r;
		}
		if (verbose) cout << " - " << MoveToString(m) << " : " << r << endl;
	}
	return count;
}


int Engine::perftRecursive(Board b, int depth) { 
	std::vector<unsigned __int64> moves = b.GenerateLegalMoves(b.Turn);
	if (depth == 1) return moves.size();
	int count = 0;
	for (const unsigned __int64& m : moves) {
		Board child = b.Copy();
		child.Push(m);
		count += perftRecursive(child, depth - 1);
	}
	return count;
}

Evaluation Engine::Search(Board board, SearchParams params) {
	
	int myTime = board.Turn ? params.wtime: params.btime;
	if (myTime == -1) myTime = 2000;

	auto startTime = Clock::now();
	Hashes.reserve(131072); // 1 MB
	int elapsedMs = 0;
	int depth = 0;
	EvaluatedNodes = 0;
	bool finished = false;

	// Calculating the time allocated for searching
	int searchTime;
	if (myTime != -1) {
		if (myTime > 120000) searchTime = 5000;
		else if (myTime > 60000) searchTime = 2000;
		else if (myTime > 30000) searchTime = 1000;
		else searchTime = 200;
	}
	else {
		searchTime = 1000;
	}

	// Check for book moves
	/*std::string bookMove = GetBookMove(board.Hash(false));
	if (bookMove != "") {
		Evaluation e;
		cout << "bestmove " << bookMove << endl;
		return e;
	}*/

	// Iterative deepening
	Evaluation e = Evaluation();
	while (!finished) {
		depth += 1;
		Hashes.clear();
		eval result = SearchRecursive(board, depth, 0, NegativeInfinity, PositiveInfinity, NoEval);

		// Check limits
		auto currentTime = Clock::now();
		elapsedMs = (currentTime - startTime).count() / 1e6;
		if (elapsedMs >= searchTime) finished = true;

		// Send info
		e.score = get<0>(result);
		e.move = get<1>(result);
		e.depth = depth;
		e.nodes = EvaluatedNodes;
		e.time = elapsedMs;
		e.nps = EvaluatedNodes * 1e9 / (currentTime - startTime).count();
		e.hashfull = Hashes.size() * 1000 / HashSize;
		PrintInfo(e);
	}
	cout << "bestmove " << MoveToString(e.move) << endl;
	Hashes.clear();
	return e;
}

eval Engine::SearchRecursive(Board board, int depth, int level, int alpha, int beta, int nodeEval) {

	if (nodeEval == NoEval) nodeEval = StaticEvaluation(board, level);

	// Calculate hash
	unsigned __int64 hash = board.Hash(true);

	// Return result for terminal nodes
	if (depth == 0) {
		eval e = eval{ nodeEval, 0ULL};
		//Hashes[hash] = e;
		return e;
	}

	// Check hash
	auto it = Hashes.find(hash);
	if (it != Hashes.end()) {
		eval e = it->second;
		return e;
	}

	// Initalize variables
	int bestScore = NoEval;
	unsigned __int64 bestMove = 0;

	// Generate moves - if there are no legal moves, we return the eval
	std::vector<unsigned __int64> legalMoves = board.GenerateLegalMoves(board.Turn);
	if (legalMoves.size() == 0) {
		eval e = eval{ nodeEval, 0ULL };
		if (Hashes.size() < HashSize) Hashes[hash] = e;
		return e;
	}

	// Move ordering
	std::vector<std::tuple<int, unsigned __int64>> order = vector<std::tuple<int, unsigned __int64>>();
	for (const unsigned __int64& m : legalMoves) {
		Board b = board.Copy();
		b.Push(m);
		int interiorScore = StaticEvaluation(b, level);
		order.push_back({interiorScore, m});
	}
	std::sort(order.begin(), order.end(), [](auto const& t1, auto const& t2) {
		return get<0>(t1) < get<0>(t2);
	});
	
	// Iterate through legal moves
	int i = 0;
	for (const std::tuple<int, unsigned __int64>&o : order) {
		Board b = board.Copy();
		b.Push(get<1>(o));

		eval childEval = SearchRecursive(b, depth - 1, level + 1, -beta, -alpha, get<0>(o));
		int childScore = -get<0>(childEval);
		unsigned __int64 childMove = get<1>(childEval);

		if (childScore > bestScore) {
			bestScore = childScore;
			alpha = bestScore;
			bestMove = get<1>(o);
			if (alpha >= beta) break;
		}

		i++;
	}

	eval e = eval{ bestScore, bestMove };
	if (Hashes.size() < HashSize) Hashes[hash] = e;
	return e;

}

int Engine::StaticEvaluation(Board board, int level) {
	EvaluatedNodes += 1;
	// 1. is over?
	if (board.State == GameState::Draw) return 0;
	if (board.State == GameState::WhiteVictory) {
		if (board.Turn == Turn::White) return MateEval - (level + 1) / 2;
		if (board.Turn == Turn::Black) return -MateEval + (level + 1) / 2;
	} else if (board.State == GameState::BlackVictory) {
		if (board.Turn == Turn::White) return -MateEval + (level + 1) / 2;
		if (board.Turn == Turn::Black) return MateEval - (level + 1) / 2;
	}

	// 2. Materials
	int score = 0;
	score += NonZeros(board.WhitePawnBits) * PawnValue;
	score += NonZeros(board.WhiteKnightBits) * KnightValue;
	score += NonZeros(board.WhiteBishopBits) * BishopValue;
	score += NonZeros(board.WhiteRookBits) * RookValue;
	score += NonZeros(board.WhiteQueenBits) * QueenValue;
	score -= NonZeros(board.BlackPawnBits) * PawnValue;
	score -= NonZeros(board.BlackKnightBits) * KnightValue;
	score -= NonZeros(board.BlackBishopBits) * BishopValue;
	score -= NonZeros(board.BlackRookBits) * RookValue;
	score -= NonZeros(board.BlackQueenBits) * QueenValue;

	if (NonZeros(board.WhiteBishopBits) >= 2) score += 50;
	if (NonZeros(board.BlackBishopBits) >= 2) score -= 50;

	for (int i = 0; i < 64; i++) {
		int piece = board.GetPieceAt(i);
		if (piece == Piece::WhitePawn) score += PawnPSQT[i];
		if (piece == Piece::WhiteKnight) score += KnightPSQT[i];
		if (piece == Piece::WhiteBishop) score += BishopPSQT[i];
		if (piece == Piece::WhiteRook) score += RookPSQT[i];
		if (piece == Piece::WhiteQueen) score += QueenPSQT[i];

		if (piece == Piece::BlackPawn) score -= PawnPSQT[63 - i];
		if (piece == Piece::BlackKnight) score -= KnightPSQT[63 - i];
		if (piece == Piece::BlackBishop) score -= BishopPSQT[63 - i];
		if (piece == Piece::BlackRook) score -= RookPSQT[63 - i];
		if (piece == Piece::BlackQueen) score -= QueenPSQT[63 - i];
	}

	if (!board.Turn) score *= -1;
	return score;
}

// Start UCI protocol
void Engine::Start() {
	cout << "Renegade chess engine " << Version << " [" << __DATE__ << " " << __TIME__ << "]" << endl;
	std::string cmd = "";
	Board board = Board(starting_fen);
	while (getline(cin, cmd)) {
		
		cmd = trimstr(cmd);
		if (cmd.size() == 0) continue;
		std::stringstream ss(cmd);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> parts(begin, end);

		if (cmd == "quit") break;

		if (cmd == "uci") {
			cout << "id name Renegade" << endl;
			cout << "id author Krisztian Peocz" << endl;
			cout << "uciok" << endl;
			continue;
		}

		if (cmd == "isready") {
			cout << "readyok" << endl;
			continue;
		}

		if (cmd == "ucinewgame") {
			continue;
		}

		if (cmd == "play") {
			Play();
			continue;
		}

		// Debug commands
		if (parts[0] == "debug") {
			if (parts[1] == "attackmap") board.Draw(board.AttackedSquares);
			if (parts[1] == "whitepawn") board.Draw(board.WhitePawnBits);
			if (parts[1] == "whiteking") board.Draw(board.WhiteKingBits);
			if (parts[1] == "blackpawn") board.Draw(board.BlackPawnBits);
			if (parts[1] == "blackking") board.Draw(board.BlackKingBits);
			if (parts[1] == "enpassant") cout << "En passant target: " << board.EnPassantSquare << endl;
			if (parts[1] == "pseudolegal") {
				std::vector<unsigned __int64> v = board.GenerateMoves(board.Turn);
				for (unsigned __int64 m : v) cout << MoveToString(m) << " ";
				cout << endl;
			}
			if (parts[1] == "hash") cout << "Hash (polyglot): " << std::hex << board.Hash(false) << std::dec << endl;
			if (parts[1] == "book") {
				if (parts[2] == "count") {
					cout << "Book entries: " << BookEntries.size() << endl;
				}
				else if (parts[2] == "entry") {
					int n = stoi(parts[3]);
					cout << "Entry " << n << ": " << std::hex << BookEntries[n].hash << std::dec << PolyglotMoveToString(BookEntries[n].from, BookEntries[n].to, BookEntries[n].promotion) << endl;
				}
				else {
					std::string s = GetBookMove(board.Hash(false));
					if (s != "") cout << "Book move: " << s << endl;
					else cout << "No book move found" << endl;
				}
				
			}
			continue;
		}

		// Position command
		if (parts[0] == "position") {

			if (parts[1] == "startpos") {
				board = Board(starting_fen);
				if (parts[2] == "moves") {
					for (int i = 3; i < parts.size(); i++) {
						bool r = board.PushUci(parts[i]);
						if (!r) cout << "!!! Error: invalid pushuci move !!!" << endl;
					}
				}
			}

			if (parts[1] == "fen") {
				std::string fen = parts[2] + " " + parts[3] + " " + parts[4] + " " + parts[5] + " " + parts[6] + " " + parts[7];
				board = Board(fen);
				if (parts[8] == "moves") {
					for (int i = 9; i < parts.size(); i++) {
						bool r = board.PushUci(parts[i]);
						if (!r) cout << "!!! Error: invalid pushuci move !!!" << endl;
					}
				}
			}

			continue;
		}

		// Go command
		if (parts[0] == "go") {

			if (parts[1] == "perft") {
				int depth = stoi(parts[2]);
				perft(board, depth, true);
				continue;
			}
			if (parts[1] == "perfd") {
				int depth = stoi(parts[2]);
				perft(board, depth, false);
				continue;
			}

			SearchParams params;
			for (__int64 i = 1; i < parts.size(); i++) {
				if (parts[i] == "wtime") {
					params.wtime = stoi(parts[i + 1]);
					i += 1;
				}
				if (parts[i] == "btime") {
					params.btime = stoi(parts[i + 1]);
					i += 1;
				}
				if (parts[i] == "movestogo") {
					params.movestogo = stoi(parts[i + 1]);
					i += 1;
				}
			}
			Search(board, params);
			continue;
		}

		cout << "Unknown command: '" << parts[0] << "'" << endl;

	}
	cout << "Renegade UCI interface ended" << endl;
}

void Engine::PrintInfo(Evaluation e) {
	cout << "info depth " << e.depth << " score cp " << e.score << " nodes " << e.nodes << " nps " << e.nps << " time " << e.time << " hashfull " << e.hashfull << " pv " << MoveToString(e.move) << endl;
}

void Engine::Play() {
	Board board = Board(starting_fen);

	cout << "c - Computer, h - Human" << endl;
	cout << "White player? ";
	char white;
	cin >> white;
	cout << "Black player? ";
	char black;
	cin >> black;

	if ((white != 'c') && (white != 'h')) return;
	if ((black != 'c') && (black != 'h')) return;

	while (board.State == GameState::Playing) {
		cout << "\033[2J\033[1;1H" << endl;
		board.Draw(0);

		char player = board.Turn ? white : black;

		if (player == 'h') {
			// Player
			std::string str;
			bool success = false;
			while (!success) {
				cout << "Move to play? ";
				cin >> str;
				success = board.PushUci(str);
			}
		} else {
			Evaluation e = Search(board, SearchParams());
			board.Push(e.move);
			cout << "Renegade plays " << MoveToString(e.move) << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}

	}

}

void Engine::InitOpeningBook() {
	std::ifstream ifs("book.bin", std::ios::in | std::ios::binary);

	if (!ifs) return;

	unsigned __int64 buffer[2];
	int c = 0;

	while (ifs.read(reinterpret_cast<char*>(&buffer), 16)) {
		BookEntry entry;
		int b = _byteswap_ushort(0xFFFF & buffer[1]);
		entry.hash = _byteswap_uint64(buffer[0]);
		entry.to =        (0b000000000111111 & b) >> 0;
		entry.from =      (0b000111111000000 & b) >> 6;
		entry.promotion = (0b111000000000000 & b) >> 12;
		entry.weight = 0;
		entry.learn = 0;
		BookEntries.push_back(entry);
	}
	ifs.close();


}

std::string Engine::GetBookMove(unsigned __int64 hash) {
	// should take about 2-3 ms for Human.bin (~900k entries) 

	std::vector<string> matches;
	for (const BookEntry &e : BookEntries) {
		if (e.hash != hash) continue;

		unsigned __int64 m = EncodeMove(e.from, e.to, 0);
		switch (e.promotion) {
			case 1: { m = SetMoveFlag(m, MoveFlag::PromotionToKnight); break; }
			case 2: { m = SetMoveFlag(m, MoveFlag::PromotionToBishop); break; }
			case 3: { m = SetMoveFlag(m, MoveFlag::PromotionToRook); break; }
			case 4: { m = SetMoveFlag(m, MoveFlag::PromotionToQueen); break; }
		}
		matches.push_back(MoveToString(m));
	}

	//cout << matches.size() << endl;
	if (matches.size() == 0) return "";
	std::srand(std::time(0));
	int random_pos = std::rand() % matches.size();

	return matches[random_pos];
}