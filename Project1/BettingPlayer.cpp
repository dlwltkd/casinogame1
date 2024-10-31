#include "BettingPlayer.h"
#include <cstdlib>
#include <ctime>

BettingPlayer::BettingPlayer() : balance(0) {
    srand(static_cast<unsigned int>(time(0)));
}

void BettingPlayer::setBalance(int amount) {
    balance = amount;
}

int BettingPlayer::getBalance() const {
    return balance;
}

GameResult BettingPlayer::betExactNum(int betAmount, int guessedNumber) {
    GameResult result;
    result.win = false;
    result.winnings = 0;

    if (betAmount > balance) {
        result.message = "Insufficient balance.";
        return result;
    }

    balance -= betAmount;

    int winningNumber = rand() % 10 + 1;
    result.correctNumber = winningNumber;

    if (guessedNumber == winningNumber) {
        result.win = true;
        result.winnings = betAmount * 10;
        balance += result.winnings;
        result.message = "You guessed correctly! You won $" + std::to_string(result.winnings) + ".";
    }
    else {
        result.message = "Wrong guess. The correct number was " + std::to_string(winningNumber) + ".";
    }

    return result;
}

GameResult BettingPlayer::betOddOrEven(int betAmount, int choice) {
    GameResult result;
    result.win = false;
    result.winnings = 0;

    if (betAmount > balance) {
        result.message = "Insufficient balance.";
        return result;
    }

    balance -= betAmount;

    int winningNumber = rand() % 10 + 1;
    result.correctNumber = winningNumber;

    bool isEven = (winningNumber % 2 == 0);
    if ((choice == 1 && !isEven) || (choice == 2 && isEven)) {
        result.win = true;
        result.winnings = betAmount * 2;
        balance += result.winnings;
        result.message = "You guessed correctly! You won $" + std::to_string(result.winnings) + ".";
    }
    else {
        result.message = "Wrong guess. The correct number was " + std::to_string(winningNumber) + ".";
    }

    return result;
}

GameResult BettingPlayer::betHighLow(int betAmount, int choice) {
    GameResult result;
    result.win = false;
    result.winnings = 0;

    if (betAmount > balance) {
        result.message = "Insufficient balance.";
        return result;
    }

    balance -= betAmount;

    int winningNumber = rand() % 10 + 1;
    result.correctNumber = winningNumber;

    bool isHigh = (winningNumber > 5);
    if ((choice == 1 && !isHigh) || (choice == 2 && isHigh)) {
        result.win = true;
        result.winnings = betAmount * 2;
        balance += result.winnings;
        result.message = "You guessed correctly! You won $" + std::to_string(result.winnings) + ".";
    }
    else {
        result.message = "Wrong guess. The correct number was " + std::to_string(winningNumber) + ".";
    }

    return result;
}
