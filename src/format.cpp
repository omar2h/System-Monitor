#include <string>
#include <sstream>
#include <iomanip> 
#include "format.h"

using std::string;

// https://stackoverflow.com/questions/58695875/how-to-convert-seconds-to-hhmmss-millisecond-format-c
string Format::ElapsedTime(long seconds)
{
    int hours{seconds/(60*60)};
    int mins{seconds/(60*60)/60};
    int sec{seconds%60};
    std::stringstream elapsedTime{};
    elapsedTime << std::setfill('0') << std::setw(2) << hours << ':' << std::setw(2) << mins
              << ':' << std::setw(2) << sec;
    return elapsedTime.str();

}