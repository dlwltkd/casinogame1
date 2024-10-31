#include "RouletteGame.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

RouletteGame::RouletteGame(sf::RenderWindow& win, sf::Font& fnt, BettingPlayer& plyr)
    : window(win), font(fnt), player(plyr), selectedChipValue(1), winningNumber(-1), gameState(PlacingBets) {
    initializeTable();
    initializeUI();
}

void RouletteGame::initializeTable() {
    // Create number cells
    int number = 1;
    for (int row = NUM_ROWS - 1; row >= 0; --row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            sf::RectangleShape cell(sf::Vector2f(static_cast<float>(CELL_WIDTH), static_cast<float>(CELL_HEIGHT)));
            cell.setPosition(100.0f + col * CELL_WIDTH, 100.0f + row * CELL_HEIGHT);
            cell.setFillColor(getNumberColor(number));
            cell.setOutlineColor(sf::Color::White);
            cell.setOutlineThickness(1.0f);

            numberCells[number] = cell;
            number++;
        }
    }

    // Create 0 cell
    sf::RectangleShape zeroCell(sf::Vector2f(static_cast<float>(CELL_WIDTH), static_cast<float>(CELL_HEIGHT * NUM_ROWS)));
    zeroCell.setPosition(40.0f, 100.0f);
    zeroCell.setFillColor(sf::Color::Green);
    zeroCell.setOutlineColor(sf::Color::White);
    zeroCell.setOutlineThickness(1.0f);

    numberCells[0] = zeroCell;
}

void RouletteGame::initializeUI() {
    // Chip selectors
    float chipX = 700.0f;
    float chipY = 100.0f;
    for (size_t i = 0; i < CHIP_VALUES.size(); ++i) {
        sf::CircleShape chip(20.0f);
        chip.setFillColor(sf::Color::White);
        chip.setOutlineColor(sf::Color::Black);
        chip.setOutlineThickness(2.0f);
        chip.setPosition(chipX, chipY + i * 60);
        chipSelectors.push_back(chip);

        sf::Text chipText("$" + std::to_string(CHIP_VALUES[i]), font, 16);
        chipText.setFillColor(sf::Color::Black);
        chipText.setPosition(chipX + 10.0f, chipY + i * 60 + 10.0f);
        chipValueTexts.push_back(chipText);
    }

    // Betting areas
    redBetArea.setSize(sf::Vector2f(100.0f, 40.0f));
    redBetArea.setPosition(100.0f, 600.0f);
    redBetArea.setFillColor(sf::Color::Red);
    redBetArea.setOutlineColor(sf::Color::White);
    redBetArea.setOutlineThickness(1.0f);

    blackBetArea = redBetArea;
    blackBetArea.setPosition(210.0f, 600.0f);
    blackBetArea.setFillColor(sf::Color::Black);

    evenBetArea = redBetArea;
    evenBetArea.setPosition(320.0f, 600.0f);
    evenBetArea.setFillColor(sf::Color(128, 128, 128));

    oddBetArea = evenBetArea;
    oddBetArea.setPosition(430.0f, 600.0f);

    highBetArea = evenBetArea;
    highBetArea.setPosition(540.0f, 600.0f);

    lowBetArea = evenBetArea;
    lowBetArea.setPosition(650.0f, 600.0f);

    // Betting area texts
    redBetText.setFont(font);
    redBetText.setString("Red");
    redBetText.setCharacterSize(18);
    redBetText.setFillColor(sf::Color::White);
    redBetText.setPosition(125.0f, 610.0f);

    blackBetText = redBetText;
    blackBetText.setString("Black");
    blackBetText.setPosition(230.0f, 610.0f);

    evenBetText = redBetText;
    evenBetText.setString("Even");
    evenBetText.setPosition(345.0f, 610.0f);

    oddBetText = redBetText;
    oddBetText.setString("Odd");
    oddBetText.setPosition(455.0f, 610.0f);

    highBetText = redBetText;
    highBetText.setString("High");
    highBetText.setPosition(565.0f, 610.0f);

    lowBetText = redBetText;
    lowBetText.setString("Low");
    lowBetText.setPosition(675.0f, 610.0f);
}

sf::Color RouletteGame::getNumberColor(int number) {
    const std::vector<int> redNumbers = {
        1,3,5,7,9,12,14,16,18,19,21,23,25,27,30,32,34,36
    };
    if (std::find(redNumbers.begin(), redNumbers.end(), number) != redNumbers.end()) {
        return sf::Color::Red;
    }
    else {
        return sf::Color::Black;
    }
}

void RouletteGame::processEvents(sf::Event& event) {
    switch (event.type) {
    case sf::Event::Closed:
        window.close();
        break;
    case sf::Event::MouseButtonPressed:
        handleMouseClick(event.mouseButton);
        break;
    case sf::Event::KeyPressed:
        if (gameState == PlacingBets && event.key.code == sf::Keyboard::S) {
            gameState = Spinning;
        }
        break;
    default:
        break;
    }
}

void RouletteGame::handleMouseClick(sf::Event::MouseButtonEvent mouseEvent) {
    sf::Vector2f mousePos(static_cast<float>(mouseEvent.x), static_cast<float>(mouseEvent.y));
    if (gameState == PlacingBets) {
        if (mouseEvent.button == sf::Mouse::Left) {
            // Check if a chip selector was clicked
            for (size_t i = 0; i < chipSelectors.size(); ++i) {
                if (chipSelectors[i].getGlobalBounds().contains(mousePos)) {
                    selectedChipValue = CHIP_VALUES[i];
                    return;
                }
            }

            // Check if a number cell was clicked
            for (auto& pair : numberCells) {
                if (pair.second.getGlobalBounds().contains(mousePos)) {
                    placeBetOnNumber(pair.first);
                    return;
                }
            }

            // Check if betting areas were clicked
            if (redBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnColor(sf::Color::Red);
                return;
            }
            else if (blackBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnColor(sf::Color::Black);
                return;
            }
            else if (evenBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnEvenOdd(true);
                return;
            }
            else if (oddBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnEvenOdd(false);
                return;
            }
            else if (highBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnHighLow(true);
                return;
            }
            else if (lowBetArea.getGlobalBounds().contains(mousePos)) {
                placeBetOnHighLow(false);
                return;
            }
        }
    }
    else if (gameState == ShowingResult) {
        // Exit Roulette game
        gameState = Exiting;
    }
}

void RouletteGame::placeBetOnNumber(int number) {
    if (selectedChipValue > player.getBalance()) {
        // Not enough balance
        return;
    }

    player.setBalance(player.getBalance() - selectedChipValue);

    // Create a bet
    Bet bet;
    bet.betType = 0; // Number bet
    bet.numbers.push_back(number);
    bet.amount = selectedChipValue;
    bets.push_back(bet);

    // Place a chip on the table
    sf::CircleShape chip(15.0f);
    chip.setFillColor(sf::Color::Yellow);
    chip.setPosition(numberCells[number].getPosition().x + CELL_WIDTH / 2 - 15.0f,
        numberCells[number].getPosition().y + CELL_HEIGHT / 2 - 15.0f);
    placedChips.push_back(chip);
}

void RouletteGame::placeBetOnColor(sf::Color color) {
    if (selectedChipValue > player.getBalance()) {
        // Not enough balance
        return;
    }

    player.setBalance(player.getBalance() - selectedChipValue);

    // Create a bet
    Bet bet;
    bet.betType = 1; // Color bet
    bet.amount = selectedChipValue;
    bet.numbers.push_back((color == sf::Color::Red) ? 1 : 2); // 1 for Red, 2 for Black
    bets.push_back(bet);

    // Place a chip on the betting area
    sf::CircleShape chip(15.0f);
    chip.setFillColor(sf::Color::Yellow);
    sf::Vector2f position = (color == sf::Color::Red) ? redBetArea.getPosition() : blackBetArea.getPosition();
    chip.setPosition(position.x + redBetArea.getSize().x / 2 - 15.0f,
        position.y + redBetArea.getSize().y / 2 - 15.0f);
    placedChips.push_back(chip);
}

void RouletteGame::placeBetOnEvenOdd(bool isEven) {
    if (selectedChipValue > player.getBalance()) {
        return;
    }

    player.setBalance(player.getBalance() - selectedChipValue);

    Bet bet;
    bet.betType = 2; // Even/Odd bet
    bet.amount = selectedChipValue;
    bet.numbers.push_back(isEven ? 1 : 2); // 1 for Even, 2 for Odd
    bets.push_back(bet);

    sf::CircleShape chip(15.0f);
    chip.setFillColor(sf::Color::Yellow);
    sf::Vector2f position = isEven ? evenBetArea.getPosition() : oddBetArea.getPosition();
    chip.setPosition(position.x + evenBetArea.getSize().x / 2 - 15.0f,
        position.y + evenBetArea.getSize().y / 2 - 15.0f);
    placedChips.push_back(chip);
}

void RouletteGame::placeBetOnHighLow(bool isHigh) {
    if (selectedChipValue > player.getBalance()) {
        return;
    }

    player.setBalance(player.getBalance() - selectedChipValue);

    Bet bet;
    bet.betType = 3; // High/Low bet
    bet.amount = selectedChipValue;
    bet.numbers.push_back(isHigh ? 1 : 2); // 1 for High, 2 for Low
    bets.push_back(bet);

    sf::CircleShape chip(15.0f);
    chip.setFillColor(sf::Color::Yellow);
    sf::Vector2f position = isHigh ? highBetArea.getPosition() : lowBetArea.getPosition();
    chip.setPosition(position.x + highBetArea.getSize().x / 2 - 15.0f,
        position.y + highBetArea.getSize().y / 2 - 15.0f);
    placedChips.push_back(chip);
}

void RouletteGame::update() {
    if (gameState == Spinning) {
        // Simulate spinning
        winningNumber = rand() % 37; // 0 to 36
        winningColor = (winningNumber == 0) ? sf::Color::Green : getNumberColor(winningNumber);
        calculateWinnings();
        gameState = ShowingResult;
    }
}

void RouletteGame::calculateWinnings() {
    int totalWinnings = 0;
    for (const auto& bet : bets) {
        switch (bet.betType) {
        case 0: // Number bet
            if (std::find(bet.numbers.begin(), bet.numbers.end(), winningNumber) != bet.numbers.end()) {
                int winnings = bet.amount * 35;
                totalWinnings += winnings;
            }
            break;
        case 1: // Color bet
            if ((bet.numbers[0] == 1 && winningColor == sf::Color::Red) ||
                (bet.numbers[0] == 2 && winningColor == sf::Color::Black)) {
                int winnings = bet.amount * 2;
                totalWinnings += winnings;
            }
            break;
        case 2: // Even/Odd bet
            if (winningNumber != 0) {
                bool isEven = (winningNumber % 2 == 0);
                if ((bet.numbers[0] == 1 && isEven) || (bet.numbers[0] == 2 && !isEven)) {
                    int winnings = bet.amount * 2;
                    totalWinnings += winnings;
                }
            }
            break;
        case 3: // High/Low bet
            if (winningNumber != 0) {
                bool isHigh = (winningNumber >= 19);
                if ((bet.numbers[0] == 1 && isHigh) || (bet.numbers[0] == 2 && !isHigh)) {
                    int winnings = bet.amount * 2;
                    totalWinnings += winnings;
                }
            }
            break;
        default:
            break;
        }
    }
    player.setBalance(player.getBalance() + totalWinnings);
    bets.clear();
    placedChips.clear();
}

void RouletteGame::render() {
    window.clear(sf::Color(0, 100, 0)); // Dark green background

    // Draw table
    drawTable();

    // Draw chips
    for (const auto& chip : placedChips) {
        window.draw(chip);
    }

    // Draw balance
    sf::Text balanceText("Balance: $" + std::to_string(player.getBalance()), font, 24);
    balanceText.setPosition(700.0f, 20.0f);
    window.draw(balanceText);

    // Draw selected chip value
    sf::Text chipValueText("Selected Chip: $" + std::to_string(selectedChipValue), font, 24);
    chipValueText.setPosition(700.0f, 60.0f);
    window.draw(chipValueText);

    // Draw chip selectors
    for (size_t i = 0; i < chipSelectors.size(); ++i) {
        if (selectedChipValue == CHIP_VALUES[i]) {
            chipSelectors[i].setOutlineColor(sf::Color::Red);
        }
        else {
            chipSelectors[i].setOutlineColor(sf::Color::Black);
        }
        window.draw(chipSelectors[i]);
        window.draw(chipValueTexts[i]);
    }

    // Draw betting areas
    window.draw(redBetArea);
    window.draw(blackBetArea);
    window.draw(evenBetArea);
    window.draw(oddBetArea);
    window.draw(highBetArea);
    window.draw(lowBetArea);

    // Draw betting area texts
    window.draw(redBetText);
    window.draw(blackBetText);
    window.draw(evenBetText);
    window.draw(oddBetText);
    window.draw(highBetText);
    window.draw(lowBetText);

    // Draw instructions
    if (gameState == PlacingBets) {
        sf::Text instructionText("Place your bets!\nLeft-click to place bets.\nPress 'S' to spin the wheel.", font, 20);
        instructionText.setPosition(700.0f, 300.0f);
        instructionText.setFillColor(sf::Color::White);
        window.draw(instructionText);
    }
    else if (gameState == ShowingResult) {
        // Display winning number and color
        std::string colorStr = (winningColor == sf::Color::Red) ? "Red" :
            (winningColor == sf::Color::Black) ? "Black" : "Green";
        sf::Text resultText("Winning Number: " + std::to_string(winningNumber) + " (" + colorStr + ")\nLeft-click to return.", font, 24);
        resultText.setPosition(700.0f, 300.0f);
        resultText.setFillColor(sf::Color::White);
        window.draw(resultText);
    }

    window.display();
}

void RouletteGame::drawTable() {
    // Draw zero cell
    window.draw(numberCells[0]);

    // Draw zero text
    sf::Text zeroText("0", font, 20);
    zeroText.setFillColor(sf::Color::White);
    zeroText.setPosition(numberCells[0].getPosition().x + CELL_WIDTH / 2 - 5.0f,
        numberCells[0].getPosition().y + (CELL_HEIGHT * NUM_ROWS) / 2 - 10.0f);
    window.draw(zeroText);

    // Draw number cells
    for (int i = 1; i <= 36; ++i) {
        window.draw(numberCells[i]);

        // Draw number text
        sf::Text numberText(std::to_string(i), font, 20);
        numberText.setFillColor(sf::Color::White);
        numberText.setPosition(numberCells[i].getPosition().x + CELL_WIDTH / 2 - 5.0f,
            numberCells[i].getPosition().y + CELL_HEIGHT / 2 - 10.0f);
        window.draw(numberText);
    }
}
