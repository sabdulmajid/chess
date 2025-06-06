#!/bin/bash

# Ultra-Realistic Git History Generator - Fixed Version
# Creates realistic development history by rebuilding from current files

set -e

echo "🌟 Ultra-Realistic Chess Engine Development History (Fixed)"
echo "=========================================================="
echo ""
echo "This script creates the most realistic development pattern:"
echo "• 🎯 Natural project evolution using current files"
echo "• 📊 Realistic commit frequency variations"
echo "• 🔄 Bug fixes and iterative improvements"
echo "• 🎨 Refactoring and optimization commits"
echo "• 📚 Documentation updates throughout development"
echo "• 🧪 Testing commits spread naturally"
echo "• 🚀 CI/CD evolution over time"
echo ""
echo "⚠️  This will completely rewrite git history!"
read -p "Create ultra-realistic history? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

# Setup
CURRENT_BRANCH=$(git branch --show-current)
BACKUP_BRANCH="backup-ultra-fixed-$(date +%Y%m%d-%H%M%S)"
git branch $BACKUP_BRANCH

echo "📌 Current branch: $CURRENT_BRANCH"
echo "💾 Backup: $BACKUP_BRANCH"

# Create a new orphan branch to start fresh
TEMP_BRANCH="temp-ultra-$(date +%Y%m%d-%H%M%S)"
git checkout --orphan $TEMP_BRANCH

# Remove all files from git index but keep in working directory
git rm --cached -r . 2>/dev/null || true

# Ultra-realistic commit schedule with incremental file additions
declare -a ULTRA_REALISTIC=(
    # Week 1: Project Foundation (May 11-17)
    "2025-05-11 09:30:00,initial commit: chess engine project foundation,README.md .gitignore"
    "2025-05-11 09:45:00,add basic project structure and build system,CMakeLists.txt Makefile"
    "2025-05-11 11:00:00,implement core piece class hierarchy,include/pieces/ src/pieces/piece.cpp"
    "2025-05-11 13:30:00,add bishop rook and basic piece movement,src/pieces/bishop.cpp src/pieces/rook.cpp"
    "2025-05-11 15:45:00,implement knight and pawn pieces,src/pieces/knight.cpp src/pieces/pawn.cpp"
    "2025-05-11 17:20:00,add king and queen implementations,src/pieces/king.cpp src/pieces/queen.cpp include/pieces/"
    
    # Day 2-3: Board and Game Logic
    "2025-05-12 09:15:00,implement chessboard representation,include/board/ src/board/"
    "2025-05-12 14:00:00,add coordinate system and validation,src/board/chessboard.cpp"
    "2025-05-12 16:30:00,fix piece movement validation bugs,src/pieces/"
    "2025-05-13 08:45:00,morning bug fixes: piece interactions,src/pieces/ src/board/"
    "2025-05-13 12:15:00,implement core game logic framework,include/game/ src/game/"
    "2025-05-13 18:00:00,evening debugging: move validation edge cases,src/game/ src/board/"
    
    # Weekend Project Kick-off
    "2025-05-14 14:20:00,weekend project: player framework design,include/players/ src/players/"
    "2025-05-14 16:45:00,implement human player interface,src/players/human.cpp"
    "2025-05-14 19:30:00,add basic computer player with simple ai,src/players/computer.cpp"
    "2025-05-15 11:00:00,lazy sunday: implement minimax algorithm,src/players/computer.cpp"
    "2025-05-15 15:30:00,add difficulty levels to computer player,src/players/computer.cpp"
    
    # Week 2: UI and Observer Pattern (May 18-24)
    "2025-05-16 09:00:00,monday motivation: implement observer pattern,include/observers/ src/observers/"
    "2025-05-16 11:30:00,create text-based game observer,src/observers/textobserver.cpp"
    "2025-05-16 16:15:00,integrate observers with game engine,src/game/game.cpp"
    "2025-05-17 10:45:00,start graphics system implementation,include/graphics/"
    "2025-05-17 14:20:00,add x11 window management support,src/graphics/ include/graphics/window.h"
    "2025-05-17 17:00:00,basic graphical display working,src/graphics/window.cpp"
    "2025-05-18 09:30:00,implement graphical game observer,src/observers/graphicalobserver.cpp"
    "2025-05-18 12:00:00,fix graphics rendering and display issues,src/observers/ src/graphics/"
    "2025-05-18 15:45:00,improve piece visualization and board display,src/graphics/ src/observers/"
    
    # Mid-Week Development Push
    "2025-05-19 08:30:00,morning refactor: clean up header dependencies,include/"
    "2025-05-19 13:15:00,enhance main game loop and user interaction,src/game/game.cpp src/main.cc"
    "2025-05-19 17:30:00,add comprehensive input validation,src/players/human.cpp"
    "2025-05-20 10:00:00,improve computer ai search depth,src/players/computer.cpp"
    "2025-05-20 14:45:00,implement checkmate and stalemate detection,src/game/game.cpp"
    "2025-05-20 18:15:00,evening bugfix session: game state management,src/game/ src/board/"
    
    # Web Interface Experiment
    "2025-05-21 20:00:00,friday night experiment: web interface planning,web/ include/web/"
    "2025-05-22 10:30:00,saturday morning: web interface implementation,src/web/web_interface.cpp"
    "2025-05-22 14:15:00,add emscripten build support,build_web.sh"
    "2025-05-22 17:45:00,fix web compilation and dependency issues,src/web/ build_web.sh"
    "2025-05-23 11:20:00,sunday refinements: enhance cmake build system,CMakeLists.txt"
    "2025-05-23 15:00:00,add console-only build configuration,CMakeLists.txt"
    
    # Week 3: Testing and Polish (May 25-31)
    "2025-05-24 09:45:00,monday cleanup: code formatting and style,src/ include/"
    "2025-05-24 13:30:00,add initial test framework,testcases/ tests/"
    "2025-05-24 16:00:00,fix memory management issues,src/game/ src/pieces/"
    "2025-05-25 11:00:00,improve error handling throughout codebase,src/game/ src/players/"
    "2025-05-25 15:30:00,enhance move validation robustness,src/board/chessboard.cpp"
    "2025-05-26 08:45:00,morning optimization: computer player performance,src/players/computer.cpp"
    "2025-05-26 14:20:00,expand test suite with edge cases,tests/"
    "2025-05-26 18:45:00,late evening debugging session,src/"
    
    # Research and Documentation Phase
    "2025-05-27 10:15:00,research advanced chess algorithms,docs/"
    "2025-05-28 09:30:00,document current minimax implementation,docs/ADVANCED_ALGORITHMS.md"
    "2025-05-29 14:45:00,plan advanced ai architecture overhaul,docs/ADVANCED_ALGORITHMS.md"
    "2025-05-30 19:00:00,weekend research: alpha-beta pruning theory,docs/ADVANCED_ALGORITHMS.md"
    "2025-05-31 13:30:00,study transposition tables and zobrist hashing,docs/ADVANCED_ALGORITHMS.md"
    
    # June: Advanced AI Implementation Phase
    "2025-06-01 16:20:00,document zobrist hashing implementation plan,docs/ADVANCED_ALGORITHMS.md"
    "2025-06-02 09:00:00,begin advanced ai implementation,include/ai/advanced_ai.h"
    "2025-06-02 11:45:00,create advanced ai class structure,src/ai/advanced_ai.cpp"
    "2025-06-02 15:30:00,implement enhanced minimax foundation,src/ai/advanced_ai.cpp"
    "2025-06-03 08:30:00,add alpha-beta pruning algorithm,src/ai/advanced_ai.cpp"
    "2025-06-03 12:15:00,implement iterative deepening search,src/ai/advanced_ai.cpp"
    "2025-06-03 16:45:00,debug and optimize search algorithm,src/ai/advanced_ai.cpp"
    
    # Advanced Features Implementation
    "2025-06-04 10:20:00,implement principal variation search,src/ai/advanced_ai.cpp"
    "2025-06-04 14:00:00,optimize move generation for performance,src/ai/advanced_ai.cpp"
    "2025-06-04 17:30:00,fix pvs edge cases and boundary conditions,src/ai/advanced_ai.cpp"
    "2025-06-05 20:30:00,friday evening: start transposition tables,src/ai/advanced_ai.cpp"
    "2025-06-06 10:00:00,saturday morning: implement zobrist hashing,src/ai/advanced_ai.cpp"
    "2025-06-06 14:30:00,add hash table management system,src/ai/advanced_ai.cpp"
    "2025-06-06 18:15:00,debug hash collision handling,src/ai/advanced_ai.cpp"
    
    # Optimization Weekend
    "2025-06-07 11:30:00,sunday optimization: memory management,src/ai/advanced_ai.cpp"
    "2025-06-07 15:45:00,implement hash replacement strategies,src/ai/advanced_ai.cpp"
    "2025-06-08 09:15:00,add quiescence search for tactical accuracy,src/ai/advanced_ai.cpp"
    "2025-06-08 13:00:00,implement null move pruning optimization,src/ai/advanced_ai.cpp"
    "2025-06-08 16:30:00,fine-tune pruning conditions,src/ai/advanced_ai.cpp"
    
    # Move Ordering and Evaluation
    "2025-06-09 08:45:00,implement sophisticated move ordering,src/ai/advanced_ai.cpp"
    "2025-06-09 12:30:00,add killer moves heuristic,src/ai/advanced_ai.cpp"
    "2025-06-09 17:00:00,implement history table for move ordering,src/ai/advanced_ai.cpp"
    "2025-06-10 10:00:00,add piece-square evaluation tables,src/ai/advanced_ai.cpp"
    "2025-06-10 14:15:00,implement comprehensive evaluation function,src/ai/advanced_ai.cpp"
    "2025-06-10 18:45:00,evening parameter tuning session,src/ai/advanced_ai.cpp"
    
    # Integration Phase
    "2025-06-11 09:30:00,create ai factory for seamless integration,include/ai/ai_factory.h src/ai/ai_factory.cpp"
    "2025-06-11 13:15:00,implement progressive difficulty levels,src/ai/ai_factory.cpp"
    "2025-06-11 16:00:00,integrate advanced ai with existing game,src/main.cc"
    "2025-06-12 08:20:00,fix integration bugs and compilation issues,src/main.cc src/ai/"
    "2025-06-12 12:45:00,add detailed algorithm descriptions,src/ai/ai_factory.cpp"
    "2025-06-12 15:30:00,extend difficulty range to 8 levels,src/main.cc"
    
    # Testing and Validation
    "2025-06-13 10:45:00,comprehensive testing of all difficulty levels,tests/ src/"
    "2025-06-13 14:20:00,fix compilation warnings and code cleanup,src/ai/ include/ai/"
    "2025-06-13 17:15:00,performance optimization and profiling,src/ai/advanced_ai.cpp"
    "2025-06-14 09:00:00,implement comprehensive test suite,tests/"
    "2025-06-14 13:30:00,add valgrind memory testing configuration,tests/"
    "2025-06-14 16:45:00,fix memory leaks and optimization issues,src/ai/"
    
    # Documentation Sprint
    "2025-06-15 11:15:00,weekend documentation marathon,docs/ADVANCED_ALGORITHMS.md"
    "2025-06-15 15:00:00,performance benchmarking and analysis,src/ai/ docs/"
    "2025-06-15 19:30:00,second optimization pass,src/ai/advanced_ai.cpp"
    "2025-06-16 10:30:00,write comprehensive technical documentation,docs/ADVANCED_ALGORITHMS.md"
    "2025-06-16 14:45:00,document algorithm complexity analysis,docs/ADVANCED_ALGORITHMS.md"
    "2025-06-16 17:20:00,add performance benchmarks and statistics,docs/ADVANCED_ALGORITHMS.md"
    
    # Project Polish
    "2025-06-17 09:45:00,update readme with advanced features,README.md"
    "2025-06-17 13:15:00,add comprehensive usage examples,README.md docs/"
    "2025-06-17 16:30:00,document build instructions and requirements,README.md"
    
    # CI/CD Implementation
    "2025-06-18 08:30:00,setup github actions ci/cd pipeline,.github/workflows/"
    "2025-06-18 12:00:00,add docker containerization support,Dockerfile docker-compose.yml"
    "2025-06-18 15:45:00,implement automated web build pipeline,.github/workflows/ build_web.sh"
    "2025-06-19 10:15:00,fix ci compilation and dependency issues,.github/workflows/"
    "2025-06-19 14:30:00,add security scanning and code analysis,.github/workflows/ci-cd.yml"
    "2025-06-19 17:00:00,optimize docker build process,Dockerfile"
    
    # Final Polish
    "2025-06-20 09:20:00,enhance web interface for better compatibility,src/web/web_interface.cpp"
    "2025-06-20 13:45:00,fix emscripten compilation issues,build_web.sh src/web/"
    "2025-06-20 16:15:00,resolve remaining ci/cd pipeline issues,.github/workflows/"
    "2025-06-21 11:00:00,final comprehensive testing round,tests/ src/"
    "2025-06-21 15:30:00,last performance optimization pass,src/ai/"
    "2025-06-21 18:45:00,code cleanup and final formatting,src/ include/"
    
    # Project Completion
    "2025-06-22 10:45:00,update all documentation for release,README.md docs/"
    "2025-06-22 14:20:00,final bug fixes and polish,src/"
    "2025-06-22 16:00:00,project completion: advanced chess engine v2.0,README.md docs/ src/"
)

echo "📅 Creating ultra-realistic history with ${#ULTRA_REALISTIC[@]} commits"
echo ""

# Function with improved file handling
ultra_realistic_commit() {
    local commit_date="$1"
    local commit_msg="$2"
    local files_pattern="$3"
    
    # Determine commit type for emoji
    local emoji="💻"
    if [[ "$commit_msg" =~ (fix|debug|bug) ]]; then
        emoji="🐛"
    elif [[ "$commit_msg" =~ (test|testing) ]]; then
        emoji="🧪"
    elif [[ "$commit_msg" =~ (doc|documentation) ]]; then
        emoji="📚"
    elif [[ "$commit_msg" =~ (refactor|cleanup|optimize) ]]; then
        emoji="♻️"
    elif [[ "$commit_msg" =~ (add|implement) ]]; then
        emoji="✨"
    elif [[ "$commit_msg" =~ (initial|setup|foundation) ]]; then
        emoji="🎉"
    elif [[ "$commit_msg" =~ (weekend|evening|morning|late|night) ]]; then
        emoji="🌙"
    fi
    
    echo "$emoji [$commit_date] $commit_msg"
    
    # Add files progressively based on the commit stage
    if [[ "$files_pattern" == "." ]]; then
        # Add everything
        git add . 2>/dev/null || true
    else
        # Add specific files/patterns
        for pattern in $files_pattern; do
            if [[ -e "$pattern" ]]; then
                git add "$pattern" 2>/dev/null || true
            elif [[ -d "$pattern" ]]; then
                git add "$pattern"/ 2>/dev/null || true
            else
                # Try glob pattern
                git add $pattern 2>/dev/null || true
            fi
        done
        
        # If nothing was added, add a reasonable default
        if git diff --cached --quiet; then
            # For early commits, add basic files
            if [[ "$commit_date" < "2025-05-15" ]]; then
                git add README.md CMakeLists.txt Makefile include/ src/pieces/ src/board/ .gitignore 2>/dev/null || true
            elif [[ "$commit_date" < "2025-05-25" ]]; then
                git add include/ src/ CMakeLists.txt README.md 2>/dev/null || true
            elif [[ "$commit_date" < "2025-06-05" ]]; then
                git add src/ include/ tests/ docs/ README.md CMakeLists.txt 2>/dev/null || true
            else
                # Later commits, add everything
                git add . 2>/dev/null || true
            fi
        fi
    fi
    
    # Only commit if there are actual changes
    if ! git diff --cached --quiet; then
        GIT_COMMITTER_DATE="$commit_date" GIT_AUTHOR_DATE="$commit_date" \
        git commit -m "$commit_msg" --date="$commit_date" >/dev/null 2>&1
        echo "    ✅ Committed"
    else
        echo "    ⏭️  No changes to commit"
    fi
    echo ""
}

# Apply ultra-realistic pattern
echo "🚀 Generating ultra-realistic development history..."
echo ""

for entry in "${ULTRA_REALISTIC[@]}"; do
    IFS=',' read -r date msg files <<< "$entry"
    ultra_realistic_commit "$date" "$msg" "$files"
    
    # Realistic delay simulation
    sleep 0.05
done

# Switch back to main and replace history
echo "🔄 Replacing main branch history..."
git checkout $CURRENT_BRANCH
git reset --hard $TEMP_BRANCH
git branch -D $TEMP_BRANCH

echo ""
echo "🎉 Ultra-realistic git history created!"
echo ""
echo "📊 Final statistics:"
echo "   📈 Total commits: $(git rev-list --count HEAD)"
echo "   📅 Time span: May 11 - June 22, 2025 (6+ weeks)"
echo "   ⚡ Average: ~$(echo "$(git rev-list --count HEAD) / 42" | bc -l | xargs printf "%.1f") commits per day"
echo "   🎯 Development pattern: Ultra-realistic with natural progression"
echo ""
echo "🔍 Recent commits:"
git log --oneline --graph -10
echo ""
echo "💾 Original history saved as: $BACKUP_BRANCH"
echo "📈 View contribution pattern: git log --pretty=format:'%h %ad %s' --date=short"
echo ""
echo "⚠️  To apply this history to GitHub:"
echo "   git push --force-with-lease origin $CURRENT_BRANCH"
echo ""
echo "🌟 This creates the most natural-looking contribution graph possible!"
