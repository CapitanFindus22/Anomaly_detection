#include "main.hpp"

std::atomic<bool> running{false};
std::atomic<size_t> done = 0;
size_t windowLength;
float difference = 0.3;
bool firstRound = true;