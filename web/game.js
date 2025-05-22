// Chess Game Web Interface
class ChessGame {
    constructor() {
        this.webInterface = null;
        this.selectedSquare = null;
        this.gameActive = false;
        this.currentTurn = 'white';
        this.moveHistory = [];
        this.capturedPieces = { white: [], black: [] };
        this.gameStats = { played: 0, wins: 0, draws: 0 };
        
        this.pieceSymbols = {
            'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
            'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟'
        };
        
        this.init();
    }
    
    async init() {
        this.createBoard();
        this.attachEventListeners();
        this.loadStats();
        
        // Initialize WebAssembly module when available
        if (typeof Module !== 'undefined') {
            Module.onRuntimeInitialized = () => {
                this.webInterface = new Module.WebInterface();
                this.setupCallbacks();
            };
        }
    }
    
    createBoard() {
        const board = document.getElementById('chessboard');
        board.innerHTML = '';
        
        for (let row = 7; row >= 0; row--) {
            for (let col = 0; col < 8; col++) {
                const square = document.createElement('div');
                square.className = `square ${(row + col) % 2 === 0 ? 'dark' : 'light'}`;
                square.dataset.row = row;
                square.dataset.col = col;
                square.dataset.position = this.coordsToPosition(row, col);
                
                square.addEventListener('click', (e) => this.handleSquareClick(e));
                board.appendChild(square);
            }
        }
        
        this.setupInitialPosition();
    }
    
    setupInitialPosition() {
        const initialPosition = {
            '7,0': 'r', '7,1': 'n', '7,2': 'b', '7,3': 'q',
            '7,4': 'k', '7,5': 'b', '7,6': 'n', '7,7': 'r',
            '6,0': 'p', '6,1': 'p', '6,2': 'p', '6,3': 'p',
            '6,4': 'p', '6,5': 'p', '6,6': 'p', '6,7': 'p',
            '1,0': 'P', '1,1': 'P', '1,2': 'P', '1,3': 'P',
            '1,4': 'P', '1,5': 'P', '1,6': 'P', '1,7': 'P',
            '0,0': 'R', '0,1': 'N', '0,2': 'B', '0,3': 'Q',
            '0,4': 'K', '0,5': 'B', '0,6': 'N', '0,7': 'R'
        };
        
        Object.entries(initialPosition).forEach(([coords, piece]) => {
            const [row, col] = coords.split(',').map(Number);
            this.setPiece(row, col, piece);
        });
    }
    
    setPiece(row, col, piece) {
        const square = this.getSquare(row, col);
        if (square) {
            square.textContent = piece ? this.pieceSymbols[piece] : '';
            square.dataset.piece = piece || '';
        }
    }
    
    getSquare(row, col) {
        return document.querySelector(`[data-row="${row}"][data-col="${col}"]`);
    }
    
    coordsToPosition(row, col) {
        return String.fromCharCode(97 + col) + (row + 1);
    }
    
    positionToCoords(position) {
        const col = position.charCodeAt(0) - 97;
        const row = parseInt(position[1]) - 1;
        return [row, col];
    }
    
    handleSquareClick(event) {
        if (!this.gameActive) return;
        
        const square = event.target;
        const row = parseInt(square.dataset.row);
        const col = parseInt(square.dataset.col);
        const piece = square.dataset.piece;
        
        if (this.selectedSquare) {
            // Attempt to make a move
            if (this.selectedSquare.dataset.row === square.dataset.row && 
                this.selectedSquare.dataset.col === square.dataset.col) {
                // Deselect
                this.clearSelection();
            } else {
                // Try to move
                this.attemptMove(this.selectedSquare, square);
            }
        } else if (piece && this.isPieceOwnedByCurrentPlayer(piece)) {
            // Select piece
            this.selectSquare(square);
            this.showPossibleMoves(square.dataset.position);
        }
    }
    
    selectSquare(square) {
        this.clearSelection();
        this.selectedSquare = square;
        square.classList.add('selected');
    }
    
    clearSelection() {
        if (this.selectedSquare) {
            this.selectedSquare.classList.remove('selected');
            this.selectedSquare = null;
        }
        this.clearPossibleMoves();
    }
    
    showPossibleMoves(position) {
        if (this.webInterface) {
            const moves = JSON.parse(this.webInterface.getPossibleMoves(position));
            moves.forEach(move => {
                const [row, col] = this.positionToCoords(move);
                const square = this.getSquare(row, col);
                if (square) {
                    square.classList.add('possible-move');
                }
            });
        }
    }
    
    clearPossibleMoves() {
        document.querySelectorAll('.possible-move').forEach(square => {
            square.classList.remove('possible-move');
        });
    }
    
    isPieceOwnedByCurrentPlayer(piece) {
        if (this.currentTurn === 'white') {
            return piece === piece.toUpperCase();
        } else {
            return piece === piece.toLowerCase();
        }
    }
    
    attemptMove(fromSquare, toSquare) {
        const fromPos = fromSquare.dataset.position;
        const toPos = toSquare.dataset.position;
        
        if (this.webInterface && this.webInterface.isValidMove(fromPos, toPos)) {
            // Handle promotion if needed
            let promotion = '';
            const piece = fromSquare.dataset.piece.toLowerCase();
            const toRow = parseInt(toSquare.dataset.row);
            
            if (piece === 'p' && (toRow === 0 || toRow === 7)) {
                promotion = this.getPromotionChoice();
            }
            
            this.makeMove(fromPos, toPos, promotion);
        } else {
            this.showMessage('Invalid move!', 'error');
        }
        
        this.clearSelection();
    }
    
    getPromotionChoice() {
        const choices = ['q', 'r', 'b', 'n'];
        const choice = prompt('Promote to (q/r/b/n):');
        return choices.includes(choice?.toLowerCase()) ? choice.toLowerCase() : 'q';
    }
    
    makeMove(from, to, promotion = '') {
        if (!this.webInterface) return;
        
        // Capture piece if present
        const [toRow, toCol] = this.positionToCoords(to);
        const capturedPiece = this.getSquare(toRow, toCol).dataset.piece;
        if (capturedPiece) {
            this.addCapturedPiece(capturedPiece);
        }
        
        this.webInterface.makeMove(from, to, promotion);
        this.addMoveToHistory(from, to, promotion);
        
        // Switch turns
        this.currentTurn = this.currentTurn === 'white' ? 'black' : 'white';
        this.updateGameInfo();
    }
    
    addCapturedPiece(piece) {
        const color = piece === piece.toUpperCase() ? 'white' : 'black';
        this.capturedPieces[color].push(piece);
        
        const container = document.getElementById(`captured-${color}-pieces`);
        const pieceElement = document.createElement('span');
        pieceElement.textContent = this.pieceSymbols[piece];
        pieceElement.className = 'captured-piece';
        container.appendChild(pieceElement);
    }
    
    addMoveToHistory(from, to, promotion) {
        const moveNotation = `${from}-${to}${promotion ? '=' + promotion.toUpperCase() : ''}`;
        this.moveHistory.push(moveNotation);
        
        const moveList = document.getElementById('move-list');
        const moveElement = document.createElement('div');
        moveElement.className = 'move';
        moveElement.textContent = `${Math.ceil(this.moveHistory.length / 2)}. ${moveNotation}`;
        moveList.appendChild(moveElement);
        moveList.scrollTop = moveList.scrollHeight;
    }
    
    updateGameInfo() {
        document.getElementById('current-turn').textContent = 
            `${this.currentTurn.charAt(0).toUpperCase() + this.currentTurn.slice(1)} to move`;
    }
    
    setupCallbacks() {
        if (!this.webInterface) return;
        
        this.webInterface.setUpdateCallback((boardState) => {
            this.updateBoard(JSON.parse(boardState));
        });
        
        this.webInterface.setGameStateCallback((gameState) => {
            this.updateGameState(JSON.parse(gameState));
        });
    }
    
    updateBoard(boardData) {
        if (!boardData.board) return;
        
        boardData.board.forEach((row, rowIndex) => {
            row.forEach((square, colIndex) => {
                this.setPiece(7 - rowIndex, colIndex, square.piece);
            });
        });
    }
    
    updateGameState(gameState) {
        document.getElementById('game-status').textContent = this.getStatusText(gameState);
        
        if (gameState.status === 'checkmate' || gameState.status === 'stalemate') {
            this.endGame(gameState);
        }
    }
    
    getStatusText(gameState) {
        switch (gameState.status) {
            case 'check': return 'Check!';
            case 'checkmate': return 'Checkmate!';
            case 'stalemate': return 'Stalemate!';
            case 'playing': return 'Game in progress';
            default: return 'Ready to play';
        }
    }
    
    attachEventListeners() {
        document.getElementById('new-game').addEventListener('click', () => this.startNewGame());
        document.getElementById('resign').addEventListener('click', () => this.resign());
        document.getElementById('reset').addEventListener('click', () => this.resetGame());
    }
    
    startNewGame() {
        const difficulty = parseInt(document.getElementById('difficulty').value);
        
        if (this.webInterface) {
            this.webInterface.startNewGame(true, false, difficulty);
        }
        
        this.gameActive = true;
        this.currentTurn = 'white';
        this.moveHistory = [];
        this.capturedPieces = { white: [], black: [] };
        
        this.clearCapturedPieces();
        this.clearMoveHistory();
        this.clearSelection();
        this.setupInitialPosition();
        this.updateGameInfo();
        
        this.showMessage('New game started!', 'success');
    }
    
    resign() {
        if (!this.gameActive) return;
        
        if (confirm('Are you sure you want to resign?')) {
            if (this.webInterface) {
                this.webInterface.resign();
            }
            
            this.endGame({ status: 'resigned', winner: this.currentTurn === 'white' ? 'black' : 'white' });
        }
    }
    
    resetGame() {
        if (this.webInterface) {
            this.webInterface.resetGame();
        }
        
        this.gameActive = false;
        this.currentTurn = 'white';
        this.clearSelection();
        this.clearCapturedPieces();
        this.clearMoveHistory();
        this.setupInitialPosition();
        
        document.getElementById('game-status').textContent = 'Ready to play';
        document.getElementById('current-turn').textContent = 'White to move';
    }
    
    endGame(gameState) {
        this.gameActive = false;
        this.gameStats.played++;
        
        if (gameState.status === 'checkmate' && gameState.winner === 'white') {
            this.gameStats.wins++;
        } else if (gameState.status === 'stalemate') {
            this.gameStats.draws++;
        }
        
        this.saveStats();
        this.updateStatsDisplay();
    }
    
    clearCapturedPieces() {
        document.getElementById('captured-white-pieces').innerHTML = '';
        document.getElementById('captured-black-pieces').innerHTML = '';
    }
    
    clearMoveHistory() {
        document.getElementById('move-list').innerHTML = '';
    }
    
    showMessage(text, type = 'info') {
        const existingMessage = document.querySelector('.message');
        if (existingMessage) {
            existingMessage.remove();
        }
        
        const message = document.createElement('div');
        message.className = `message ${type}`;
        message.textContent = text;
        
        const gameContainer = document.querySelector('.game-container');
        gameContainer.insertBefore(message, gameContainer.firstChild);
        
        setTimeout(() => message.remove(), 3000);
    }
    
    loadStats() {
        const saved = localStorage.getItem('chessGameStats');
        if (saved) {
            this.gameStats = JSON.parse(saved);
            this.updateStatsDisplay();
        }
    }
    
    saveStats() {
        localStorage.setItem('chessGameStats', JSON.stringify(this.gameStats));
    }
    
    updateStatsDisplay() {
        document.getElementById('games-played').textContent = this.gameStats.played;
        document.getElementById('wins').textContent = this.gameStats.wins;
        document.getElementById('draws').textContent = this.gameStats.draws;
    }
}

// Initialize the game when the page loads
document.addEventListener('DOMContentLoaded', () => {
    window.chessGame = new ChessGame();
});
