#ifndef ADVANCED_AI_H
#define ADVANCED_AI_H

#include <unordered_map>
#include <vector>
#include <chrono>
#include <climits>
#include "chessboard.h"
#include "player.h"

// Forward declarations
class Move;
class ChessBoard;

/**
 * @brief Advanced Chess AI Engine implementing state-of-the-art algorithms
 * 
 * This class implements multiple sophisticated chess algorithms including:
 * - Minimax with Alpha-Beta Pruning
 * - Iterative Deepening
 * - Transposition Tables with Zobrist Hashing
 * - Principal Variation Search (PVS)
 * - Null Move Pruning
 * - Quiescence Search
 * - Advanced Move Ordering
 * - Opening Book Integration
 */
class AdvancedAI : public Player {
public:
    // Move representation for internal algorithm use
    struct Move {
        int fromRow, fromCol, toRow, toCol;
        char promotion;
        int score;
        
        Move(int fr = -1, int fc = -1, int tr = -1, int tc = -1, char p = 'x', int s = 0) 
            : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), promotion(p), score(s) {}
            
        bool operator==(const Move& other) const {
            return fromRow == other.fromRow && fromCol == other.fromCol && 
                   toRow == other.toRow && toCol == other.toCol && 
                   promotion == other.promotion;
        }
    };

    // Transposition table entry types
    enum class NodeType {
        EXACT,      // Exact score
        LOWER_BOUND, // Alpha cutoff (fail-high)
        UPPER_BOUND  // Beta cutoff (fail-low)
    };

    // Transposition table entry
    struct TTEntry {
        uint64_t hash;
        int depth;
        int score;
        Move bestMove;
        NodeType type;
        
        TTEntry() : hash(0), depth(-1), score(0), type(NodeType::EXACT) {}
        TTEntry(uint64_t h, int d, int s, Move m, NodeType t) 
            : hash(h), depth(d), score(s), bestMove(m), type(t) {}
    };

private:
    // Algorithm parameters
    int maxDepth;
    int timeLimit; // milliseconds
    bool useIterativeDeepening;
    bool useTranspositionTable;
    bool useNullMovePruning;
    bool useQuiescenceSearch;
    bool usePrincipalVariationSearch;
    
    // Search statistics
    mutable int nodesSearched;
    mutable int transpositionHits;
    mutable int alphaBetaCutoffs;
    mutable int quiescenceNodes;
    
    // Transposition table
    mutable std::unordered_map<uint64_t, TTEntry> transpositionTable;
    const size_t MAX_TT_SIZE = 1000000; // Maximum entries
    
    // Zobrist hashing for position encoding
    static uint64_t zobristTable[8][8][12]; // [square][piece]
    static uint64_t zobristBlackToMove;
    static uint64_t zobristCastling[4];
    static uint64_t zobristEnPassant[8];
    static bool zobristInitialized;
    
    // Piece-square tables for evaluation
    static const int PAWN_TABLE[8][8];
    static const int KNIGHT_TABLE[8][8];
    static const int BISHOP_TABLE[8][8];
    static const int ROOK_TABLE[8][8];
    static const int QUEEN_TABLE[8][8];
    static const int KING_MIDDLEGAME_TABLE[8][8];
    static const int KING_ENDGAME_TABLE[8][8];
    
    // Opening book
    std::unordered_map<uint64_t, std::vector<Move>> openingBook;
    
    // Killer moves heuristic (best moves at each depth)
    mutable Move killerMoves[64][2]; // [depth][killer_slot]
    
    // History heuristic (move ordering)
    mutable int historyTable[8][8][8][8]; // [from][to] move scores

public:
    explicit AdvancedAI(bool isWhite, int difficulty = 4);
    virtual ~AdvancedAI() = default;
    
    // Player interface
    bool makeMove(ChessBoard& board) override;
    
    // Configuration methods
    void setMaxDepth(int depth) { maxDepth = depth; }
    void setTimeLimit(int ms) { timeLimit = ms; }
    void enableIterativeDeepening(bool enable) { useIterativeDeepening = enable; }
    void enableTranspositionTable(bool enable) { useTranspositionTable = enable; }
    void enableNullMovePruning(bool enable) { useNullMovePruning = enable; }
    void enableQuiescenceSearch(bool enable) { useQuiescenceSearch = enable; }
    void enablePrincipalVariationSearch(bool enable) { usePrincipalVariationSearch = enable; }
    
    // Statistics
    void printSearchStatistics() const;
    void clearStatistics() const;

private:
    // Core search algorithms
    Move findBestMove(ChessBoard& board);
    int minimax(ChessBoard& board, int depth, int alpha, int beta, bool maximizing, 
                std::chrono::steady_clock::time_point startTime) const;
    int principalVariationSearch(ChessBoard& board, int depth, int alpha, int beta, 
                                bool maximizing, std::chrono::steady_clock::time_point startTime) const;
    int quiescenceSearch(ChessBoard& board, int alpha, int beta, bool maximizing, 
                        std::chrono::steady_clock::time_point startTime) const;
    
    // Move generation and ordering
    std::vector<Move> generateMoves(ChessBoard& board, bool capturesOnly = false) const;
    void orderMoves(std::vector<Move>& moves, ChessBoard& board, int depth, Move ttMove) const;
    int scoreMoveForOrdering(const Move& move, ChessBoard& board, int depth, Move ttMove) const;
    
    // Evaluation function
    int evaluatePosition(ChessBoard& board) const;
    int evaluateMaterial(ChessBoard& board) const;
    int evaluatePositional(ChessBoard& board) const;
    int evaluatePawnStructure(ChessBoard& board) const;
    int evaluateKingSafety(ChessBoard& board) const;
    int evaluateMobility(ChessBoard& board) const;
    bool isEndgame(ChessBoard& board) const;
    
    // Transposition table
    uint64_t computeZobristHash(ChessBoard& board) const;
    void storeInTranspositionTable(uint64_t hash, int depth, int score, 
                                  Move bestMove, NodeType type) const;
    bool probeTranspositionTable(uint64_t hash, int depth, int alpha, int beta, 
                                int& score, Move& bestMove) const;
    
    // Utility functions
    bool isTimeUp(std::chrono::steady_clock::time_point startTime) const;
    void updateKillerMoves(const Move& move, int depth) const;
    void updateHistoryTable(const Move& move, int depth) const;
    Move convertToInternalMove(int fromRow, int fromCol, int toRow, int toCol, char promotion = 'x') const;
    bool isCapture(const Move& move, ChessBoard& board) const;
    bool isCheck(const Move& move, ChessBoard& board) const;
    
    // Opening book
    void initializeOpeningBook();
    Move getOpeningMove(ChessBoard& board) const;
    
    // Static initialization
    static void initializeZobristTable();
    static int getPieceIndex(char pieceType, bool isWhite);
};

#endif // ADVANCED_AI_H
