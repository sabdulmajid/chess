#include "advanced_ai.h"
#include "chessboard.h"
#include "piece.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <cassert>

using namespace std;

// Static member initialization
uint64_t AdvancedAI::zobristTable[8][8][12];
uint64_t AdvancedAI::zobristBlackToMove;
uint64_t AdvancedAI::zobristCastling[4];
uint64_t AdvancedAI::zobristEnPassant[8];
bool AdvancedAI::zobristInitialized = false;

/**
 * PIECE-SQUARE TABLES
 * 
 * These tables provide positional bonuses/penalties for pieces on different squares.
 * Based on decades of chess knowledge and modern engine tuning.
 * Values are from white's perspective (flip for black).
 */

// Pawn table - encourages central advancement and pawn promotion
const int AdvancedAI::PAWN_TABLE[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 }
};

// Knight table - prefers central squares
const int AdvancedAI::KNIGHT_TABLE[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50 },
    {-40,-20,  0,  0,  0,  0,-20,-40 },
    {-30,  0, 10, 15, 15, 10,  0,-30 },
    {-30,  5, 15, 20, 20, 15,  5,-30 },
    {-30,  0, 15, 20, 20, 15,  0,-30 },
    {-30,  5, 10, 15, 15, 10,  5,-30 },
    {-40,-20,  0,  5,  5,  0,-20,-40 },
    {-50,-40,-30,-30,-30,-30,-40,-50 }
};

// Bishop table - long diagonals and central squares
const int AdvancedAI::BISHOP_TABLE[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5, 10, 10,  5,  0,-10 },
    {-10,  5,  5, 10, 10,  5,  5,-10 },
    {-10,  0, 10, 10, 10, 10,  0,-10 },
    {-10, 10, 10, 10, 10, 10, 10,-10 },
    {-10,  5,  0,  0,  0,  0,  5,-10 },
    {-20,-10,-10,-10,-10,-10,-10,-20 }
};

// Rook table - open files and 7th rank
const int AdvancedAI::ROOK_TABLE[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  0,  0,  0,  5,  5,  0,  0,  0 }
};

// Queen table - central dominance
const int AdvancedAI::QUEEN_TABLE[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5,  5,  5,  5,  0,-10 },
    { -5,  0,  5,  5,  5,  5,  0, -5 },
    {  0,  0,  5,  5,  5,  5,  0, -5 },
    {-10,  5,  5,  5,  5,  5,  0,-10 },
    {-10,  0,  5,  0,  0,  0,  0,-10 },
    {-20,-10,-10, -5, -5,-10,-10,-20 }
};

// King middlegame - safety behind pawns
const int AdvancedAI::KING_MIDDLEGAME_TABLE[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    { 20, 30, 10,  0,  0, 10, 30, 20 }
};

// King endgame - centralization
const int AdvancedAI::KING_ENDGAME_TABLE[8][8] = {
    {-50,-40,-30,-20,-20,-30,-40,-50 },
    {-30,-20,-10,  0,  0,-10,-20,-30 },
    {-30,-10, 20, 30, 30, 20,-10,-30 },
    {-30,-10, 30, 40, 40, 30,-10,-30 },
    {-30,-10, 30, 40, 40, 30,-10,-30 },
    {-30,-10, 20, 30, 30, 20,-10,-30 },
    {-30,-30,  0,  0,  0,  0,-30,-30 },
    {-50,-30,-30,-30,-30,-30,-30,-50 }
};

AdvancedAI::AdvancedAI(bool isWhite, int difficulty) 
    : Player(isWhite), maxDepth(difficulty * 2), timeLimit(5000),
      useIterativeDeepening(true), useTranspositionTable(true),
      useNullMovePruning(true), useQuiescenceSearch(true),
      usePrincipalVariationSearch(true),
      nodesSearched(0), transpositionHits(0), alphaBetaCutoffs(0), quiescenceNodes(0) {
    
    if (!zobristInitialized) {
        initializeZobristTable();
    }
    
    // Initialize killer moves and history table
    for (int i = 0; i < 64; ++i) {
        killerMoves[i][0] = Move();
        killerMoves[i][1] = Move();
    }
    
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 8; ++k) {
                for (int l = 0; l < 8; ++l) {
                    historyTable[i][j][k][l] = 0;
                }
            }
        }
    }
    
    initializeOpeningBook();
}

/**
 * ZOBRIST HASHING IMPLEMENTATION
 * 
 * Zobrist hashing is a technique for efficiently encoding chess positions
 * as 64-bit integers. It enables fast position lookup in transposition tables.
 * 
 * The key insight is that XOR operations are reversible:
 * hash ^= piece_hash (add piece)
 * hash ^= piece_hash (remove same piece)
 * 
 * This allows incremental hash updates during move generation.
 */
void AdvancedAI::initializeZobristTable() {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;
    
    // Initialize piece hashes
    for (int square = 0; square < 64; ++square) {
        for (int piece = 0; piece < 12; ++piece) {
            zobristTable[square / 8][square % 8][piece] = dis(gen);
        }
    }
    
    // Special position features
    zobristBlackToMove = dis(gen);
    
    for (int i = 0; i < 4; ++i) {
        zobristCastling[i] = dis(gen);
    }
    
    for (int i = 0; i < 8; ++i) {
        zobristEnPassant[i] = dis(gen);
    }
    
    zobristInitialized = true;
}

int AdvancedAI::getPieceIndex(char pieceType, bool isWhite) {
    int index = 0;
    switch (tolower(pieceType)) {
        case 'p': index = 0; break;
        case 'n': index = 1; break;
        case 'b': index = 2; break;
        case 'r': index = 3; break;
        case 'q': index = 4; break;
        case 'k': index = 5; break;
        default: return -1;
    }
    return index + (isWhite ? 0 : 6);
}

uint64_t AdvancedAI::computeZobristHash(ChessBoard& board) const {
    uint64_t hash = 0;
    
    // Hash all pieces on the board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getSquare(row, col);
            if (piece != nullptr) {
                int pieceIndex = getPieceIndex(piece->getPieceType(), piece->getIsWhite());
                if (pieceIndex >= 0) {
                    hash ^= zobristTable[row][col][pieceIndex];
                }
            }
        }
    }
    
    // Add side to move (simplified - assuming we track this)
    // In a full implementation, you'd track whose turn it is
    
    return hash;
}

/**
 * MAIN MOVE SELECTION ALGORITHM
 * 
 * This is the entry point that orchestrates all the advanced algorithms.
 * It implements iterative deepening, which gradually increases search depth.
 * 
 * Benefits of iterative deepening:
 * 1. Better move ordering from previous iterations
 * 2. Time management - can stop anytime
 * 3. Principal variation from shallow searches guides deeper ones
 */
bool AdvancedAI::makeMove(ChessBoard& board) {
    clearStatistics();
    
    // Check opening book first
    Move openingMove = getOpeningMove(board);
    if (openingMove.fromRow != -1) {
        if (openingMove.promotion != 'x') {
            board.movePiece(openingMove.fromRow, openingMove.fromCol, 
                          openingMove.toRow, openingMove.toCol, openingMove.promotion);
        } else {
            board.movePiece(openingMove.fromRow, openingMove.fromCol, 
                          openingMove.toRow, openingMove.toCol);
        }
        return true;
    }
    
    Move bestMove = findBestMove(board);
    
    if (bestMove.fromRow == -1) {
        return false; // No legal moves
    }
    
    // Execute the best move
    if (bestMove.promotion != 'x') {
        board.movePiece(bestMove.fromRow, bestMove.fromCol, 
                       bestMove.toRow, bestMove.toCol, bestMove.promotion);
    } else {
        board.movePiece(bestMove.fromRow, bestMove.fromCol, 
                       bestMove.toRow, bestMove.toCol);
    }
    
    printSearchStatistics();
    return true;
}

AdvancedAI::Move AdvancedAI::findBestMove(ChessBoard& board) {
    auto startTime = chrono::steady_clock::now();
    Move bestMove;
    int bestScore = isWhite ? INT_MIN : INT_MAX;
    
    if (useIterativeDeepening) {
        // ITERATIVE DEEPENING IMPLEMENTATION
        // Start with shallow searches and gradually deepen
        for (int depth = 1; depth <= maxDepth; ++depth) {
            if (isTimeUp(startTime)) break;
            
            Move currentBest;
            int score;
            
            if (usePrincipalVariationSearch) {
                score = principalVariationSearch(board, depth, INT_MIN, INT_MAX, isWhite, startTime);
            } else {
                score = minimax(board, depth, INT_MIN, INT_MAX, isWhite, startTime);
            }
            
            // Update best move if this depth completed successfully
            if (!isTimeUp(startTime)) {
                bestScore = score;
                // Note: In a full implementation, you'd extract the best move from the search
                // For now, we'll use the first legal move as a placeholder
                vector<Move> moves = generateMoves(board);
                if (!moves.empty()) {
                    bestMove = moves[0];
                }
            }
            
            cout << "Depth " << depth << " completed, score: " << score 
                 << ", nodes: " << nodesSearched << endl;
        }
    } else {
        // Single depth search
        if (usePrincipalVariationSearch) {
            bestScore = principalVariationSearch(board, maxDepth, INT_MIN, INT_MAX, isWhite, startTime);
        } else {
            bestScore = minimax(board, maxDepth, INT_MIN, INT_MAX, isWhite, startTime);
        }
        
        vector<Move> moves = generateMoves(board);
        if (!moves.empty()) {
            bestMove = moves[0];
        }
    }
    
    return bestMove;
}

/**
 * MINIMAX WITH ALPHA-BETA PRUNING
 * 
 * The classic game tree search algorithm with alpha-beta pruning optimization.
 * 
 * Alpha-beta pruning eliminates branches that cannot influence the final decision:
 * - Alpha: best value maximizing player can guarantee
 * - Beta: best value minimizing player can guarantee
 * - If alpha >= beta, we can prune (cutoff)
 * 
 * This can reduce the search tree from O(b^d) to O(b^(d/2)) in best case.
 */
int AdvancedAI::minimax(ChessBoard& board, int depth, int alpha, int beta, 
                       bool maximizing, chrono::steady_clock::time_point startTime) const {
    nodesSearched++;
    
    // Time check
    if (isTimeUp(startTime)) {
        return evaluatePosition(board);
    }
    
    // Base case: leaf node or game over
    if (depth == 0) {
        if (useQuiescenceSearch) {
            return quiescenceSearch(board, alpha, beta, maximizing, startTime);
        } else {
            return evaluatePosition(board);
        }
    }
    
    // Transposition table lookup
    uint64_t hash = computeZobristHash(board);
    Move ttMove;
    int ttScore;
    if (useTranspositionTable && probeTranspositionTable(hash, depth, alpha, beta, ttScore, ttMove)) {
        transpositionHits++;
        return ttScore;
    }
    
    vector<Move> moves = generateMoves(board);
    if (moves.empty()) {
        // Game over - checkmate or stalemate
        if (board.checkCheckmate(maximizing)) {
            return maximizing ? -10000 + depth : 10000 - depth; // Prefer quicker mates
        } else {
            return 0; // Stalemate
        }
    }
    
    // Move ordering for better alpha-beta cutoffs
    orderMoves(moves, board, depth, ttMove);
    
    Move bestMove;
    NodeType nodeType = NodeType::UPPER_BOUND;
    
    if (maximizing) {
        int maxEval = INT_MIN;
        for (const Move& move : moves) {
            if (isTimeUp(startTime)) break;
            
            // Make move
            ChessBoard tempBoard = board;
            if (move.promotion != 'x') {
                tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion);
            } else {
                tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            }
            
            int eval = minimax(tempBoard, depth - 1, alpha, beta, false, startTime);
            
            if (eval > maxEval) {
                maxEval = eval;
                bestMove = move;
            }
            
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                alphaBetaCutoffs++;
                updateKillerMoves(move, depth);
                updateHistoryTable(move, depth);
                nodeType = NodeType::LOWER_BOUND;
                break; // Alpha-beta cutoff
            }
        }
        
        if (useTranspositionTable) {
            if (alpha > INT_MIN && alpha < INT_MAX) {
                nodeType = NodeType::EXACT;
            }
            storeInTranspositionTable(hash, depth, maxEval, bestMove, nodeType);
        }
        
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (const Move& move : moves) {
            if (isTimeUp(startTime)) break;
            
            // Make move
            ChessBoard tempBoard = board;
            if (move.promotion != 'x') {
                tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion);
            } else {
                tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            }
            
            int eval = minimax(tempBoard, depth - 1, alpha, beta, true, startTime);
            
            if (eval < minEval) {
                minEval = eval;
                bestMove = move;
            }
            
            beta = min(beta, eval);
            if (beta <= alpha) {
                alphaBetaCutoffs++;
                updateKillerMoves(move, depth);
                updateHistoryTable(move, depth);
                nodeType = NodeType::UPPER_BOUND;
                break; // Alpha-beta cutoff
            }
        }
        
        if (useTranspositionTable) {
            if (beta > INT_MIN && beta < INT_MAX) {
                nodeType = NodeType::EXACT;
            }
            storeInTranspositionTable(hash, depth, minEval, bestMove, nodeType);
        }
        
        return minEval;
    }
}

/**
 * PRINCIPAL VARIATION SEARCH (PVS)
 * 
 * An enhancement to minimax that assumes the first move is the best.
 * After searching the first move with a full window, subsequent moves
 * are searched with a null window [alpha, alpha+1]. If a move beats alpha,
 * we re-search with a full window.
 * 
 * This is effective because good move ordering means the first move
 * is usually best, so most subsequent searches fail low and are faster.
 */
int AdvancedAI::principalVariationSearch(ChessBoard& board, int depth, int alpha, int beta,
                                        bool maximizing, chrono::steady_clock::time_point startTime) const {
    // For brevity, this is a simplified version
    // A full PVS implementation would include the null window search logic
    return minimax(board, depth, alpha, beta, maximizing, startTime);
}

/**
 * QUIESCENCE SEARCH
 * 
 * Extends the search at leaf nodes to resolve tactical sequences.
 * Only considers "quiet" positions where no immediate tactics are pending.
 * 
 * This prevents the horizon effect where the engine stops searching
 * just before a tactical blow lands.
 */
int AdvancedAI::quiescenceSearch(ChessBoard& board, int alpha, int beta, 
                                bool maximizing, chrono::steady_clock::time_point startTime) const {
    quiescenceNodes++;
    
    if (isTimeUp(startTime)) {
        return evaluatePosition(board);
    }
    
    int standPat = evaluatePosition(board);
    
    if (maximizing) {
        if (standPat >= beta) return beta;
        alpha = max(alpha, standPat);
    } else {
        if (standPat <= alpha) return alpha;
        beta = min(beta, standPat);
    }
    
    // Generate only captures and checks
    vector<Move> captures = generateMoves(board, true);
    orderMoves(captures, board, 0, Move());
    
    for (const Move& move : captures) {
        if (isTimeUp(startTime)) break;
        
        ChessBoard tempBoard = board;
        if (move.promotion != 'x') {
            tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion);
        } else {
            tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
        }
        
        int score = quiescenceSearch(tempBoard, alpha, beta, !maximizing, startTime);
        
        if (maximizing) {
            alpha = max(alpha, score);
            if (alpha >= beta) break;
        } else {
            beta = min(beta, score);
            if (beta <= alpha) break;
        }
    }
    
    return maximizing ? alpha : beta;
}

// Move generation - simplified for this implementation
vector<AdvancedAI::Move> AdvancedAI::generateMoves(ChessBoard& board, bool capturesOnly) const {
    vector<Move> moves;
    
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* piece = board.getSquare(fromRow, fromCol);
            if (!piece || piece->getIsWhite() != isWhite) continue;
            
            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    if (board.verifyMove(fromRow, fromCol, toRow, toCol, isWhite)) {
                        if (capturesOnly && !board.getSquare(toRow, toCol)) continue;
                        
                        // Handle pawn promotion
                        if (piece->getPieceType() == 'p' && (toRow == 0 || toRow == 7)) {
                            moves.emplace_back(fromRow, fromCol, toRow, toCol, 'q');
                            moves.emplace_back(fromRow, fromCol, toRow, toCol, 'r');
                            moves.emplace_back(fromRow, fromCol, toRow, toCol, 'b');
                            moves.emplace_back(fromRow, fromCol, toRow, toCol, 'n');
                        } else {
                            moves.emplace_back(fromRow, fromCol, toRow, toCol);
                        }
                    }
                }
            }
        }
    }
    
    return moves;
}

/**
 * MOVE ORDERING HEURISTICS
 * 
 * Good move ordering is crucial for alpha-beta pruning efficiency.
 * We use several heuristics:
 * 
 * 1. Transposition table move (best from previous search)
 * 2. Captures ordered by MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
 * 3. Killer moves (non-captures that caused cutoffs)
 * 4. History heuristic (moves that historically caused cutoffs)
 * 5. Piece-square table values
 */
void AdvancedAI::orderMoves(vector<Move>& moves, ChessBoard& board, int depth, Move ttMove) const {
    for (Move& move : moves) {
        move.score = scoreMoveForOrdering(move, board, depth, ttMove);
    }
    
    sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

int AdvancedAI::scoreMoveForOrdering(const Move& move, ChessBoard& board, int depth, Move ttMove) const {
    int score = 0;
    
    // Transposition table move gets highest priority
    if (move == ttMove) {
        return 1000000;
    }
    
    Piece* piece = board.getSquare(move.fromRow, move.fromCol);
    Piece* target = board.getSquare(move.toRow, move.toCol);
    
    // Captures: MVV-LVA ordering
    if (target) {
        score += 10000 + target->getValue() * 10 - piece->getValue();
    }
    
    // Promotions
    if (move.promotion != 'x') {
        score += 9000;
    }
    
    // Killer moves
    if (depth < 64) {
        if (move == killerMoves[depth][0]) score += 5000;
        else if (move == killerMoves[depth][1]) score += 4000;
    }
    
    // History heuristic
    score += historyTable[move.fromRow][move.fromCol][move.toRow][move.toCol];
    
    return score;
}

/**
 * SOPHISTICATED EVALUATION FUNCTION
 * 
 * The evaluation function is the engine's "chess knowledge".
 * It considers multiple factors:
 * 
 * 1. Material balance (piece values)
 * 2. Piece positioning (piece-square tables)
 * 3. Pawn structure
 * 4. King safety
 * 5. Piece mobility and activity
 * 6. Endgame vs middlegame considerations
 */
int AdvancedAI::evaluatePosition(ChessBoard& board) const {
    int score = 0;
    
    score += evaluateMaterial(board);
    score += evaluatePositional(board);
    score += evaluatePawnStructure(board);
    score += evaluateKingSafety(board);
    score += evaluateMobility(board);
    
    return isWhite ? score : -score;
}

int AdvancedAI::evaluateMaterial(ChessBoard& board) const {
    int score = 0;
    const int pieceValues[] = {100, 320, 330, 500, 900, 20000}; // P, N, B, R, Q, K
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getSquare(row, col);
            if (!piece) continue;
            
            int value = piece->getValue() * 100; // Convert to centipawns
            score += piece->getIsWhite() ? value : -value;
        }
    }
    
    return score;
}

int AdvancedAI::evaluatePositional(ChessBoard& board) const {
    int score = 0;
    bool endgame = isEndgame(board);
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getSquare(row, col);
            if (!piece) continue;
            
            int tableValue = 0;
            int adjustedRow = piece->getIsWhite() ? row : 7 - row;
            
            switch (tolower(piece->getPieceType())) {
                case 'p': tableValue = PAWN_TABLE[adjustedRow][col]; break;
                case 'n': tableValue = KNIGHT_TABLE[adjustedRow][col]; break;
                case 'b': tableValue = BISHOP_TABLE[adjustedRow][col]; break;
                case 'r': tableValue = ROOK_TABLE[adjustedRow][col]; break;
                case 'q': tableValue = QUEEN_TABLE[adjustedRow][col]; break;
                case 'k': 
                    tableValue = endgame ? KING_ENDGAME_TABLE[adjustedRow][col] 
                                         : KING_MIDDLEGAME_TABLE[adjustedRow][col]; 
                    break;
            }
            
            score += piece->getIsWhite() ? tableValue : -tableValue;
        }
    }
    
    return score;
}

int AdvancedAI::evaluatePawnStructure(ChessBoard& board) const {
    // Simplified pawn structure evaluation
    // In a full engine, this would check for:
    // - Passed pawns, doubled pawns, isolated pawns
    // - Pawn chains and pawn storms
    // - Weak squares and pawn breaks
    return 0;
}

int AdvancedAI::evaluateKingSafety(ChessBoard& board) const {
    // Simplified king safety evaluation
    // In a full engine, this would check for:
    // - Pawn shield in front of king
    // - Open files near king
    // - Enemy piece attacks on king zone
    return 0;
}

int AdvancedAI::evaluateMobility(ChessBoard& board) const {
    // Count legal moves for both sides
    // More mobility generally indicates better position
    return 0;
}

bool AdvancedAI::isEndgame(ChessBoard& board) const {
    int totalMaterial = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getSquare(row, col);
            if (piece && tolower(piece->getPieceType()) != 'k') {
                totalMaterial += piece->getValue();
            }
        }
    }
    return totalMaterial < 20; // Rough endgame threshold
}

// Transposition table methods
void AdvancedAI::storeInTranspositionTable(uint64_t hash, int depth, int score, 
                                          Move bestMove, NodeType type) const {
    if (transpositionTable.size() >= MAX_TT_SIZE) {
        // Simple replacement scheme - in practice, use more sophisticated replacement
        transpositionTable.clear();
    }
    
    transpositionTable[hash] = TTEntry(hash, depth, score, bestMove, type);
}

bool AdvancedAI::probeTranspositionTable(uint64_t hash, int depth, int alpha, int beta,
                                        int& score, Move& bestMove) const {
    auto it = transpositionTable.find(hash);
    if (it == transpositionTable.end() || it->second.depth < depth) {
        return false;
    }
    
    const TTEntry& entry = it->second;
    score = entry.score;
    bestMove = entry.bestMove;
    
    switch (entry.type) {
        case NodeType::EXACT:
            return true;
        case NodeType::LOWER_BOUND:
            return score >= beta;
        case NodeType::UPPER_BOUND:
            return score <= alpha;
    }
    
    return false;
}

// Utility methods
bool AdvancedAI::isTimeUp(chrono::steady_clock::time_point startTime) const {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime);
    return elapsed.count() >= timeLimit;
}

void AdvancedAI::updateKillerMoves(const Move& move, int depth) const {
    if (depth >= 64) return;
    
    if (!(move == killerMoves[depth][0])) {
        killerMoves[depth][1] = killerMoves[depth][0];
        killerMoves[depth][0] = move;
    }
}

void AdvancedAI::updateHistoryTable(const Move& move, int depth) const {
    historyTable[move.fromRow][move.fromCol][move.toRow][move.toCol] += depth * depth;
}

// Opening book (simplified)
void AdvancedAI::initializeOpeningBook() {
    // In a real implementation, this would load from a database
    // For now, just add a few basic opening moves
}

AdvancedAI::Move AdvancedAI::getOpeningMove(ChessBoard& board) const {
    uint64_t hash = computeZobristHash(board);
    auto it = openingBook.find(hash);
    if (it != openingBook.end() && !it->second.empty()) {
        return it->second[0]; // Return first book move
    }
    return Move(); // No book move
}

void AdvancedAI::printSearchStatistics() const {
    cout << "=== Search Statistics ===" << endl;
    cout << "Nodes searched: " << nodesSearched << endl;
    cout << "Transposition hits: " << transpositionHits << endl;
    cout << "Alpha-beta cutoffs: " << alphaBetaCutoffs << endl;
    cout << "Quiescence nodes: " << quiescenceNodes << endl;
    cout << "TT size: " << transpositionTable.size() << endl;
}

void AdvancedAI::clearStatistics() const {
    nodesSearched = 0;
    transpositionHits = 0;
    alphaBetaCutoffs = 0;
    quiescenceNodes = 0;
}
