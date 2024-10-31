#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "BettingPlayer.h"
#include "RouletteGame.h" // Include the RouletteGame header

enum GamePhase {
    SetupPhase,
    GameSelectionPhase,
    BetPhase,
    GamePlayPhase,
    ResultPhase,
    BankruptcyPhase,
    RoulettePhase // New phase for the Roulette game
};

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // Set up the SFML window
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Casino Game");
    window.setFramerateLimit(60);

    // Load the font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Could not load font.\n";
        return -1;
    }

    BettingPlayer player;
    GamePhase currentPhase = SetupPhase;

    // Variables for game logic
    int initialBalance = 0;
    int gameChoice = 0;
    int currentBet = 0;
    int playerInput = 0;
    int betType = 0;
    GameResult gameResult;

    std::string inputString = "";

    // Text elements
    sf::Text titleText("Welcome to the Casino Game!", font, 50);
    titleText.setPosition(150, 20);
    titleText.setFillColor(sf::Color::Yellow);

    sf::Text promptText("", font, 30);
    promptText.setPosition(100, 100);
    promptText.setFillColor(sf::Color::White);

    sf::Text inputText("", font, 30);
    inputText.setPosition(110, 155);
    inputText.setFillColor(sf::Color::Black);

    sf::Text balanceText("Balance: $0", font, 30);
    balanceText.setPosition(700, 20);
    balanceText.setFillColor(sf::Color::White);

    sf::Text resultText("", font, 28);
    resultText.setPosition(100, 550);
    resultText.setFillColor(sf::Color::White);

    // Button colors
    sf::Color buttonColor(218, 165, 32); // Golden color
    sf::Color hoverColor(255, 215, 0);   // Bright gold

    // Game choice buttons
    sf::RectangleShape button1(sf::Vector2f(250, 60));
    button1.setPosition(100, 200);
    button1.setFillColor(buttonColor);

    sf::Text button1Text("Exact Number", font, 28);
    button1Text.setPosition(120, 210);
    button1Text.setFillColor(sf::Color::Black);

    sf::RectangleShape button2(sf::Vector2f(250, 60));
    button2.setPosition(100, 280);
    button2.setFillColor(buttonColor);

    sf::Text button2Text("Odd/Even", font, 28);
    button2Text.setPosition(120, 290);
    button2Text.setFillColor(sf::Color::Black);

    sf::RectangleShape button3(sf::Vector2f(250, 60));
    button3.setPosition(100, 360);
    button3.setFillColor(buttonColor);

    sf::Text button3Text("Low/High", font, 28);
    button3Text.setPosition(120, 370);
    button3Text.setFillColor(sf::Color::Black);

    sf::RectangleShape button4(sf::Vector2f(250, 60));
    button4.setPosition(100, 440);
    button4.setFillColor(buttonColor);

    sf::Text button4Text("Roulette", font, 28);
    button4Text.setPosition(120, 450);
    button4Text.setFillColor(sf::Color::Black);

    // Input box
    sf::RectangleShape inputBox(sf::Vector2f(600, 50));
    inputBox.setPosition(100, 150);
    inputBox.setFillColor(sf::Color::White);

    // Create a background
    sf::RectangleShape background(sf::Vector2f(1000, 700));
    background.setFillColor(sf::Color(0, 100, 0)); // Dark green, like a casino table

    // Gameplay option buttons
    sf::RectangleShape optionButton1(sf::Vector2f(250, 60));
    optionButton1.setPosition(100, 250);
    optionButton1.setFillColor(buttonColor);

    sf::Text optionButton1Text("", font, 28);
    optionButton1Text.setPosition(120, 260);
    optionButton1Text.setFillColor(sf::Color::Black);

    sf::RectangleShape optionButton2(sf::Vector2f(250, 60));
    optionButton2.setPosition(100, 330);
    optionButton2.setFillColor(buttonColor);

    sf::Text optionButton2Text("", font, 28);
    optionButton2Text.setPosition(120, 340);
    optionButton2Text.setFillColor(sf::Color::Black);

    // Variables for animations
    sf::Clock clock;
    float deltaTime = 0.0f;

    // Variables for the Roulette game
    RouletteGame* rouletteGame = nullptr;

    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Handle events
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (currentPhase == RoulettePhase) {
                // Pass events to the RouletteGame
                rouletteGame->processEvents(event);

                if (rouletteGame->gameState == RouletteGame::Exiting) {
                    delete rouletteGame;
                    rouletteGame = nullptr;
                    balanceText.setString("Balance: $" + std::to_string(player.getBalance()));
                    currentPhase = GameSelectionPhase;
                }
            }
            else {
                // Handle events for other phases
                switch (currentPhase) {
                case SetupPhase:
                    promptText.setString("Enter your starting balance:");
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode >= 48 && event.text.unicode <= 57) { // Unicode for '0'-'9'
                            inputString += static_cast<char>(event.text.unicode);
                            inputText.setString("$" + inputString);
                        }
                        else if (event.text.unicode == 8 && !inputString.empty()) { // Unicode for backspace
                            inputString.pop_back();
                            inputText.setString("$" + inputString);
                        }
                    }
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                        if (!inputString.empty()) {
                            initialBalance = std::stoi(inputString);
                            player.setBalance(initialBalance);
                            balanceText.setString("Balance: $" + std::to_string(player.getBalance()));
                            inputString = "";
                            inputText.setString("");
                            currentPhase = GameSelectionPhase;
                        }
                    }
                    break;

                case GameSelectionPhase:
                    // Check for bankruptcy after setup phase
                    if (player.getBalance() <= 0) {
                        currentPhase = BankruptcyPhase;
                        resultText.setString("You are bankrupt. Press R to restart or close the window.");
                        break;
                    }
                    promptText.setString("Select a game:");

                    // Button hover effect
                    if (button1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        button1.setFillColor(hoverColor);
                    }
                    else {
                        button1.setFillColor(buttonColor);
                    }
                    if (button2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        button2.setFillColor(hoverColor);
                    }
                    else {
                        button2.setFillColor(buttonColor);
                    }
                    if (button3.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        button3.setFillColor(hoverColor);
                    }
                    else {
                        button3.setFillColor(buttonColor);
                    }
                    if (button4.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        button4.setFillColor(hoverColor);
                    }
                    else {
                        button4.setFillColor(buttonColor);
                    }

                    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                        if (button1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            gameChoice = 1;
                            currentPhase = BetPhase;
                        }
                        else if (button2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            gameChoice = 2;
                            currentPhase = BetPhase;
                        }
                        else if (button3.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            gameChoice = 3;
                            currentPhase = BetPhase;
                        }
                        else if (button4.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            currentPhase = RoulettePhase;
                            rouletteGame = new RouletteGame(window, font, player); // Corrected line
                        }
                    }
                    break;

                case BetPhase:
                    promptText.setString("Enter your bet amount:");
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode >= 48 && event.text.unicode <= 57) {
                            inputString += static_cast<char>(event.text.unicode);
                            inputText.setString("$" + inputString);
                        }
                        else if (event.text.unicode == 8 && !inputString.empty()) {
                            inputString.pop_back();
                            inputText.setString("$" + inputString);
                        }
                    }
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                        if (!inputString.empty()) {
                            currentBet = std::stoi(inputString);
                            if (currentBet > player.getBalance()) {
                                resultText.setString("Insufficient balance. Enter a valid bet.");
                                inputString = "";
                                inputText.setString("");
                            }
                            else {
                                inputString = "";
                                inputText.setString("");
                                currentPhase = GamePlayPhase;
                            }
                        }
                    }
                    break;

                case GamePlayPhase:
                    if (gameChoice == 1) {
                        promptText.setString("Guess a number between 1 and 10:");
                        if (event.type == sf::Event::TextEntered) {
                            if (event.text.unicode >= 48 && event.text.unicode <= 57) {
                                inputString += static_cast<char>(event.text.unicode);
                                inputText.setString(inputString);
                            }
                            else if (event.text.unicode == 8 && !inputString.empty()) {
                                inputString.pop_back();
                                inputText.setString(inputString);
                            }
                        }
                        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                            if (!inputString.empty()) {
                                playerInput = std::stoi(inputString);
                                inputString = "";
                                inputText.setString("");
                                gameResult = player.betExactNum(currentBet, playerInput);
                                balanceText.setString("Balance: $" + std::to_string(player.getBalance()));
                                resultText.setString(gameResult.message);
                                currentBet = 0; // Reset current bet
                                currentPhase = ResultPhase;
                            }
                        }
                    }
                    else {
                        // For games with choices, use buttons
                        if (gameChoice == 2) {
                            promptText.setString("Choose Odd or Even:");
                            optionButton1Text.setString("Odd");
                            optionButton2Text.setString("Even");
                        }
                        else if (gameChoice == 3) {
                            promptText.setString("Choose Low or High:");
                            optionButton1Text.setString("Low (1-5)");
                            optionButton2Text.setString("High (6-10)");
                        }

                        // Hover effects for option buttons
                        if (optionButton1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            optionButton1.setFillColor(hoverColor);
                        }
                        else {
                            optionButton1.setFillColor(buttonColor);
                        }
                        if (optionButton2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            optionButton2.setFillColor(hoverColor);
                        }
                        else {
                            optionButton2.setFillColor(buttonColor);
                        }

                        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                            if (optionButton1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                                playerInput = 1;
                                if (gameChoice == 2) {
                                    gameResult = player.betOddOrEven(currentBet, playerInput);
                                }
                                else if (gameChoice == 3) {
                                    gameResult = player.betHighLow(currentBet, playerInput);
                                }
                                balanceText.setString("Balance: $" + std::to_string(player.getBalance()));
                                resultText.setString(gameResult.message);
                                currentBet = 0; // Reset current bet
                                currentPhase = ResultPhase;
                            }
                            else if (optionButton2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                                playerInput = 2;
                                if (gameChoice == 2) {
                                    gameResult = player.betOddOrEven(currentBet, playerInput);
                                }
                                else if (gameChoice == 3) {
                                    gameResult = player.betHighLow(currentBet, playerInput);
                                }
                                balanceText.setString("Balance: $" + std::to_string(player.getBalance()));
                                resultText.setString(gameResult.message);
                                currentBet = 0; // Reset current bet
                                currentPhase = ResultPhase;
                            }
                        }
                    }
                    break;

                case ResultPhase:
                    promptText.setString("Press any key to continue.");
                    if (event.type == sf::Event::KeyPressed) {
                        resultText.setString("");
                        promptText.setString("");
                        currentPhase = GameSelectionPhase;
                    }
                    break;

                case BankruptcyPhase:
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                        // Reset game
                        player.setBalance(0);
                        balanceText.setString("Balance: $0");
                        resultText.setString("");
                        currentPhase = SetupPhase;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        // Update
        if (currentPhase == RoulettePhase) {
            rouletteGame->update();
        }

        // Render
        if (currentPhase == RoulettePhase) {
            rouletteGame->render();
        }
        else {
            // Clear and draw the window content
            window.clear();

            // Draw background
            window.draw(background);

            // Draw common elements
            window.draw(titleText);
            window.draw(balanceText);
            window.draw(resultText);

            // Draw phase-specific elements
            if (currentPhase == GameSelectionPhase) {
                window.draw(promptText);

                // Draw buttons
                window.draw(button1);
                window.draw(button1Text);
                window.draw(button2);
                window.draw(button2Text);
                window.draw(button3);
                window.draw(button3Text);
                window.draw(button4);
                window.draw(button4Text);
            }
            else if (currentPhase == SetupPhase || currentPhase == BetPhase || currentPhase == GamePlayPhase) {
                window.draw(promptText);
                if (currentPhase == GamePlayPhase && (gameChoice == 2 || gameChoice == 3)) {
                    // Draw option buttons
                    window.draw(optionButton1);
                    window.draw(optionButton1Text);
                    window.draw(optionButton2);
                    window.draw(optionButton2Text);
                }
                else {
                    window.draw(inputBox);
                    window.draw(inputText);
                }
            }
            else if (currentPhase == ResultPhase || currentPhase == BankruptcyPhase) {
                window.draw(promptText);
            }

            window.display();
        }
    }

    // Clean up
    if (rouletteGame != nullptr) {
        delete rouletteGame;
    }

    return 0;
}
