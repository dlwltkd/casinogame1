#ifndef BETTINGPLAYER_H
#define BETTINGPLAYER_H

#include <string>

struct GameResult {
    bool win;
    int winnings;
    int correctNumber;
    std::string message;
};

class BettingPlayer {
private:
    int balance;

public:
    BettingPlayer();

    void setBalance(int amount);
    int getBalance() const;

    GameResult betExactNum(int betAmount, int guessedNumber);
    GameResult betOddOrEven(int betAmount, int choice);
    GameResult betHighLow(int betAmount, int choice);
};

#endif // BETTINGPLAYER_H
