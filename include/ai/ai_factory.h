#ifndef AI_FACTORY_H
#define AI_FACTORY_H

#include <memory>
#include "player.h"

/**
 * @brief Factory class for creating different types of AI players
 * 
 * This factory creates appropriate AI players based on difficulty level:
 * - Levels 1-4: Original Computer class (backward compatibility)
 * - Levels 5-8: Advanced AI with sophisticated algorithms
 */
class AIFactory {
public:
    /**
     * @brief Creates an AI player based on difficulty level
     * 
     * @param isWhite Player color
     * @param difficulty Difficulty level (1-8)
     * @return std::unique_ptr<Player> AI player instance
     */
    static std::unique_ptr<Player> createAI(bool isWhite, int difficulty);
    
    /**
     * @brief Creates an advanced AI player with custom configuration
     * 
     * @param isWhite Player color
     * @param maxDepth Maximum search depth
     * @param timeLimit Time limit in milliseconds
     * @param useAdvancedFeatures Enable all advanced features
     * @return std::unique_ptr<Player> Advanced AI player instance
     */
    static std::unique_ptr<Player> createAdvancedAI(bool isWhite, 
                                                    int maxDepth = 6,
                                                    int timeLimit = 5000,
                                                    bool useAdvancedFeatures = true);
    
    /**
     * @brief Get description of AI features for each difficulty level
     * 
     * @param difficulty Difficulty level
     * @return std::string Description of AI capabilities
     */
    static std::string getAIDescription(int difficulty);
};

/**
 * @brief Print detailed information about AI algorithms
 */
void printAIAlgorithmInfo();

#endif // AI_FACTORY_H
