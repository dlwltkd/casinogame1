#ifndef ROULETTEGAME_H
#define ROULETTEGAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include "BettingPlayer.h"

class RouletteGame {
public:
    // Constructor
    RouletteGame(sf::RenderWindow& win, sf::Font& fnt, BettingPlayer& plyr);

    // Game state
    enum GameState {
        PlacingBets,
        Spinning,
        ShowingResult,
        Exiting
    };
    GameState gameState;

    // Public methods
    void processEvents(sf::Event& event);
    void update();
    void render();

private:
    // Reference to the main window
    sf::RenderWindow& window;
    sf::Font& font;

    // Reference to the BettingPlayer object
    BettingPlayer& player;

    // Current selected chip value
    int selectedChipValue;

    // Bets placed by the player
    struct Bet {
        int betType; // 0 for number bet, 1 for color bet, etc.
        std::vector<int> numbers;
        int amount;
    };
    std::vector<Bet> bets;

    // Map to hold the positions of numbers on the table
    std::map<int, sf::RectangleShape> numberCells;

    // Map to hold chip sprites placed on the table
    std::vector<sf::CircleShape> placedChips;

    // Winning number
    int winningNumber;

    // Winning color
    sf::Color winningColor;

    // Constants for the roulette table
    const int CELL_WIDTH = 60;
    const int CELL_HEIGHT = 40;
    const int NUM_ROWS = 12;
    const int NUM_COLS = 3;

    // Chip values
    const std::vector<int> CHIP_VALUES = { 1, 5, 25, 100 };

    // Chip selector UI
    std::vector<sf::CircleShape> chipSelectors;
    std::vector<sf::Text> chipValueTexts;

    // Betting areas for Red/Black, Even/Odd, High/Low
    sf::RectangleShape redBetArea;
    sf::RectangleShape blackBetArea;
    sf::RectangleShape evenBetArea;
    sf::RectangleShape oddBetArea;
    sf::RectangleShape highBetArea;
    sf::RectangleShape lowBetArea;

    sf::Text redBetText;
    sf::Text blackBetText;
    sf::Text evenBetText;
    sf::Text oddBetText;
    sf::Text highBetText;
    sf::Text lowBetText;

    // Private methods
    void initializeTable();
    void initializeUI();
    sf::Color getNumberColor(int number);
    void handleMouseClick(sf::Event::MouseButtonEvent mouseEvent);
    void placeBetOnNumber(int number);
    void placeBetOnColor(sf::Color color);
    void placeBetOnEvenOdd(bool isEven);
    void placeBetOnHighLow(bool isHigh);
    void drawTable();
    void calculateWinnings();
};

#endif // ROULETTEGAME_H
