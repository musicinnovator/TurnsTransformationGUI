// Turns Transformation GUI (C) 2025
// Version with percentage control, multiple choice variant selection, and GUI without dependencies
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <memory>

// Helper to get note name (from MIDI number)
std::string getNoteName(int noteNumber) {
    static const std::string noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    int octave = noteNumber / 12 - 1;
    int noteIndex = noteNumber % 12;
    return noteNames[noteIndex] + std::to_string(octave);
}

// Helper to get MIDI number from note name
int getNoteNumber(const std::string& noteName) {
    static const std::string noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    // Extract note and octave
    std::string baseNote = noteName.substr(0, noteName.size() - 1);
    int octave = std::stoi(noteName.substr(noteName.size() - 1));

    // Find base note index
    auto it = std::find(std::begin(noteNames), std::end(noteNames), baseNote);
    if (it == std::end(noteNames)) {
        throw std::invalid_argument("Invalid note name: " + noteName);
    }
    int noteIndex = std::distance(std::begin(noteNames), it);

    return (octave + 1) * 12 + noteIndex;
}

// Enum for TimeMeter
enum TimeMeter {
    DUPLE,
    TRIPLE
};

// Helper functions for Turn variants
void handleTurnMeter(std::vector<std::pair<int, int>>& EmbRet, int upper, int principal, int lower, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 4;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 3 * segment}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 4;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 3 * segment}); // Remaining duration
    }
}

void handleTurnMeterFB(std::vector<std::pair<int, int>>& EmbRet, int upper, int pi, int lower, int underlower, int lower2, int pi2, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 8;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({pi, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({underlower, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({pi, durPi - 5 * segment}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 5;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({pi, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({pi, segment});
        EmbRet.push_back({upper, durPi - 4 * segment}); // Remaining duration
    }
}

void handleTurnMeterBetween(std::vector<std::pair<int, int>>& EmbRet, int pi, int upper, int pi2, int lower, int pi3, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 8;
        EmbRet.push_back({pi, durPi / 2});
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({pi2, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({pi3, durPi - (durPi / 2 + 3 * segment)}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 8;
        EmbRet.push_back({pi, segment});
        EmbRet.push_back({upper, durPi / 4});
        EmbRet.push_back({pi2, durPi / 4});
        EmbRet.push_back({lower, durPi / 4});
        EmbRet.push_back({pi3, durPi - (segment + durPi / 4 + durPi / 4 + durPi / 4)}); // Remaining duration
    }
}

void handleTurnMeterTrille(std::vector<std::pair<int, int>>& EmbRet, int upper, int principal, int lower, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 8;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 5 * segment}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 8;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 5 * segment}); // Remaining duration
    }
}

void handleTurnMeterP32(std::vector<std::pair<int, int>>& EmbRet, int upper, int principal, int lower, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = (durPi * 3) / 8;
        int segment2 = durPi / 8;
        EmbRet.push_back({upper, segment1});
        EmbRet.push_back({principal, segment2});
        EmbRet.push_back({lower, segment2});
        EmbRet.push_back({principal, durPi - (segment1 + 2 * segment2)}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment1 = (durPi * 3) / 8;
        int segment2 = durPi / 8;
        EmbRet.push_back({upper, segment1});
        EmbRet.push_back({principal, segment2});
        EmbRet.push_back({lower, segment2});
        EmbRet.push_back({principal, durPi - (segment1 + 2 * segment2)}); // Remaining duration
    }
}

void handleTurnMeterSnapped(std::vector<std::pair<int, int>>& EmbRet, int upper, int principal, int lower, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 6;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 3 * segment}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 6;
        EmbRet.push_back({upper, segment});
        EmbRet.push_back({principal, segment});
        EmbRet.push_back({lower, segment});
        EmbRet.push_back({principal, durPi - 3 * segment}); // Remaining duration
    }
}

// Main function to apply turn variants
std::vector<std::pair<int, int>> applyTurnVariants(int pi, int durPi, TimeMeter meter, const std::string& variant) {
    if (durPi <= 0) {
        throw std::invalid_argument("Duration (durPi) must be greater than 0");
    }
    if (meter != DUPLE && meter != TRIPLE) {
        throw std::invalid_argument("Invalid TimeMeter");
    }

    std::vector<std::pair<int, int>> EmbRet;

    // Basic Turn variants
    if (variant == "Turn") {  // whole step above, the principal note, and the lower auxiliary (1/2 step below) are played rapidly, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeter(EmbRet, upper, pi, lower, pi, durPi, meter);
    } else if (variant == "FTurnF") {  // 1/2 step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeter(EmbRet, upper, pi, lower, pi, durPi, meter);
    } else if (variant == "FTurnS") {  // 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeter(EmbRet, upper, pi, lower, pi, durPi, meter);
    } else if (variant == "BTurnF") {  // whole step above, the principal note, and the lower auxiliary (1/2 step below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeter(EmbRet, upper, pi, lower, pi, durPi, meter);
    } else if (variant == "BTurnS") {  // whole step above, the principal note, and the lower auxiliary (1 whole below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeter(EmbRet, upper, pi, lower, pi, durPi, meter);

    // Front and Back Turn variants
    } else if (variant == "FBTurnF") {  // 1/2 step above, the principle note, and the lower turn is 1/2 step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        int underlower = pi - 4;
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);
    } else if (variant == "FBTurnS") {  // 1/2 step above, the principle note, and the lower turn is 1 whole step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;
        int underlower = pi - 4; // 1 whole step below
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);
    } else if (variant == "FBTurnFF") {  // front and back of turn - both pre and post principal note are 1/2 step above and below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;
        int underlower = pi - 3;// 1/2 step below
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);
    } else if (variant == "FBTurnSS") {  // front and back of turn - both pre and post principal note are 1 whole step above and below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;
        int underlower = pi - 4;// 1 whole step below
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);
    } else if (variant == "FBTurnFS") {  // front and back of turn, the front is 1/2 step above, the back is 1 whole step below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;
        int underlower = pi - 4;// 1 whole step below
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);
    } else if (variant == "FBTurnSF") {  // front and back of turn, the front is 1 whole step above, the back is 1/2 step below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 1;
        int underlower = pi - 4;// 1/2 step below
        handleTurnMeterFB(EmbRet, upper, pi, lower, underlower, lower, pi, durPi, meter);

    // Between Notes Turn variants
    } else if (variant == "BNT") {  // Turn between principal notes, regular
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFTurnF") {  // Turn between Notes 1/2 step above, the principal note, and the lower auxiliary (1/2 step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFTurnS") {  // Turn between Notes 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNBTurn") {  // Between principal notes, 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNBTurnS") {  // Between principal notes, whole step above, the principal note, and the lower auxiliary (1 whole below) are played, resolve and hold for duration
        int upper = pi + 1;  // whole step above
        int lower = pi - 3;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnF") {  // Between principal notes,1/2 step above, the principle note, and the lower turn is 1/2 step below the principal
        int upper = pi + 3;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnS") {  // Between principal notes, 1/2 step above, the principle note, and the lower turn is 1 whole step below the principal
        int upper = pi + 3;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnFF") {  // Between principal notes,front and back of turn - both pre and post principal note are 1/2 step above and below
        int upper = pi + 2;  // 1/2 step above
        int lower = pi - 3;  // 1/2 step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnSS") {  // Between principal notes, front and back of turn - both pre and post principal note are 1 whole step above and below
        int upper = pi + 4;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnFS") {  // Between principal notes,front and back of turn, the front is 1/2 step above, the back is 1 whole step below
        int upper = pi + 2;  // 1/2 step above
        int lower = pi - 4;  // 1 whole step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);
    } else if (variant == "BNFBTurnSF") {  // Between principal notes,front and back of turn, the front is 1 whole step above, the back is 1/2 step below
        int upper = pi + 3;  // 1 whole step above
        int lower = pi - 3;  // 1/2 step below
        handleTurnMeterBetween(EmbRet, pi, upper, pi, lower, pi,  durPi, meter);

    // Trilled Turn variants
    } else if (variant == "TT") {  // Trilled regular turn
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFTurnF") {  // Trilled 1/2 step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFTurnS") {  // Trilled 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TBTurnF") {  // Trilled whole step above, the principal note, and the lower auxiliary (1/2 step below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TBTurnS") {  // Trilled whole step above, the principal note, and the lower auxiliary (1 whole below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnF") {  // Trilled 1/2 step above, the principle note, and the lower turn is 1/2 step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnS") {  // Trilled 1/2 step above, the principle note, and the lower turn is 1 whole step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnFF") {  // Trilled front and back of turn - both pre and post principal note are 1/2 step above and below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnSS") {  // Trilled front and back of turn - both pre and post principal note are 1 whole step above and below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnFS") {  // Trilled front and back of turn, the front is 1/2 step above, the back is 1 whole step below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "TFBTurnSF") {  // Trilled front and back of turn, the front is 1 whole step above, the back is 1/2 step below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterTrille(EmbRet, upper, pi, lower, durPi, meter);

    // P32 Turn variants
    } else if (variant == "P32T") {  // P32 Turn regular turn
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FTurnF") {  // P32 Turn 1/2 step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FTurnS") {  // P32 Turn 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32BTurnF") {  // P32 Turn whole step above, the principal note, and the lower auxiliary (1/2 step below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32BTurnS") {  // P32 Turn whole step above, the principal note, and the lower auxiliary (1 whole below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnF") {  // P32 Turn 1/2 step above, the principle note, and the lower turn is 1/2 step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnS") {  // P32 Turn 1/2 step above, the principle note, and the lower turn is 1 whole step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnFF") {  // P32 Turn front and back of turn - both pre and post principal note are 1/2 step above and below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnSS") {  // P32 Turn front and back of turn - both pre and post principal note are 1 whole step above and below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnFS") {  // P32 Turn front and back of turn, the front is 1/2 step above, the back is 1 whole step below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "P32FBTurnSF") {  // P32 Turn front and back of turn, the front is 1 whole step above, the back is 1/2 step below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterP32(EmbRet, upper, pi, lower, durPi, meter);

    // Snapped Turn variants
    } else if (variant == "ST") {  // Snapped turn regular turn
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFTurnF") {  // Snapped turn 1/2 step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFTurnS") {  // Snapped turn Turn 1 whole step above, the principal note, and the lower auxiliary (1 whole step below) are played, resolve and hold for duration
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SBTurnF") {  // Snapped turn whole step above, the principal note, and the lower auxiliary (1/2 step below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SBTurnS") {  // Snapped turn Turn whole step above, the principal note, and the lower auxiliary (1 whole below) are played, resolve and hold for duration
        int upper = pi + 2;  // whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnF") {  // Snapped turn 1/2 step above, the principle note, and the lower turn is 1/2 step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnS") {  // Snapped turn 1/2 step above, the principle note, and the lower turn is 1 whole step below the principal
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnFF") {  // Snapped turn front and back of turn - both pre and post principal note are 1/2 step above and below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnSS") {  // Snapped turn front and back of turn - both pre and post principal note are 1 whole step above and below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnFS") {  // Snapped turn front and back of turn, the front is 1/2 step above, the back is 1 whole step below
        int upper = pi + 1;  // 1/2 step above
        int lower = pi - 2;  // 1 whole step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else if (variant == "SFBTurnSF") {  // Snapped turn front and back of turn, the front is 1 whole step above, the back is 1/2 step below
        int upper = pi + 2;  // 1 whole step above
        int lower = pi - 1;  // 1/2 step below
        handleTurnMeterSnapped(EmbRet, upper, pi, lower, durPi, meter);
    } else {
        // Handle unknown variant
        throw std::invalid_argument("Unknown turn variant: " + variant);
    }

    return EmbRet;
}

// Structure to represent a turn variant
struct TurnVariant {
    std::string name;
    std::string description;
};

// Generate a random pool of turn variants for user selection
std::vector<TurnVariant> generateRandomTurnVariantPool(int poolSize = 10) {
    // Define the complete pool of turn variants
    const std::vector<TurnVariant> allVariants = {
        // Basic Turn variants
        {"Turn", "Whole step above, principal note, half step below"},
        {"FTurnF", "Half step above, principal note, whole step below"},
        {"FTurnS", "Whole step above, principal note, whole step below"},
        {"BTurnF", "Whole step above, principal note, half step below"},
        {"BTurnS", "Whole step above, principal note, whole step below"},

        // Front and Back Turn variants
        {"FBTurnF", "Half step above, principal, half step below"},
        {"FBTurnS", "Half step above, principal, whole step below"},
        {"FBTurnFF", "Front/back - half step above/below"},
        {"FBTurnSS", "Front/back - whole step above/below"},
        {"FBTurnFS", "Front half step, back whole step"},
        {"FBTurnSF", "Front whole step, back half step"},

        // Between Notes Turn variants
        {"BNT", "Between notes - regular turn"},
        {"BNFTurnF", "Between notes - half step above/below"},
        {"BNFTurnS", "Between notes - whole step above/below"},
        {"BNBTurn", "Between notes - whole step variant"},
        {"BNBTurnS", "Between notes - whole step variant 2"},
        {"BNFBTurnF", "Between notes - front/back half step"},
        {"BNFBTurnS", "Between notes - front/back whole step"},
        {"BNFBTurnFF", "Between notes - front/back half steps"},
        {"BNFBTurnSS", "Between notes - front/back whole steps"},
        {"BNFBTurnFS", "Between notes - front half, back whole"},
        {"BNFBTurnSF", "Between notes - front whole, back half"},

        // Trilled Turn variants
        {"TT", "Trilled regular turn"},
        {"TFTurnF", "Trilled half step above, whole step below"},
        {"TFTurnS", "Trilled whole step above/below"},
        {"TBTurnF", "Trilled whole step above, half step below"},
        {"TBTurnS", "Trilled whole step above/below"},
        {"TFBTurnF", "Trilled front/back half step"},
        {"TFBTurnS", "Trilled front/back whole step"},
        {"TFBTurnFF", "Trilled front/back half steps"},
        {"TFBTurnSS", "Trilled front/back whole steps"},
        {"TFBTurnFS", "Trilled front half, back whole"},
        {"TFBTurnSF", "Trilled front whole, back half"},

        // P32 Turn variants
        {"P32T", "P32 regular turn"},
        {"P32FTurnF", "P32 half step above, whole step below"},
        {"P32FTurnS", "P32 whole step above/below"},
        {"P32BTurnF", "P32 whole step above, half step below"},
        {"P32BTurnS", "P32 whole step above/below"},
        {"P32FBTurnF", "P32 front/back half step"},
        {"P32FBTurnS", "P32 front/back whole step"},
        {"P32FBTurnFF", "P32 front/back half steps"},
        {"P32FBTurnSS", "P32 front/back whole steps"},
        {"P32FBTurnFS", "P32 front half, back whole"},
        {"P32FBTurnSF", "P32 front whole, back half"},

        // Snapped Turn variants
        {"ST", "Snapped regular turn"},
        {"SFTurnF", "Snapped half step above, whole step below"},
        {"SFTurnS", "Snapped whole step above/below"},
        {"SBTurnF", "Snapped whole step above, half step below"},
        {"SBTurnS", "Snapped whole step above/below"},
        {"SFBTurnF", "Snapped front/back half step"},
        {"SFBTurnS", "Snapped front/back whole step"},
        {"SFBTurnFF", "Snapped front/back half steps"},
        {"SFBTurnSS", "Snapped front/back whole steps"},
        {"SFBTurnFS", "Snapped front half, back whole"},
        {"SFBTurnSF", "Snapped front whole, back half"}
    };

    // Create a copy of all variants and shuffle it
    std::vector<TurnVariant> shuffledVariants = allVariants;

    // Use modern random number generation
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledVariants.begin(), shuffledVariants.end(), g);

    // Return the first poolSize variants
    std::vector<TurnVariant> pool;
    for (int i = 0; i < std::min(poolSize, static_cast<int>(shuffledVariants.size())); ++i) {
        pool.push_back(shuffledVariants[i]);
    }

    return pool;
}

// Parse user input for multiple choice selection
std::vector<int> parseUserChoices(const std::string& input, int maxChoice) {
    std::vector<int> choices;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        try {
            int choice = std::stoi(token);
            if (choice >= 1 && choice <= maxChoice) {
                // Avoid duplicates
                if (std::find(choices.begin(), choices.end(), choice) == choices.end()) {
                    choices.push_back(choice);
                }
            }
        } catch (const std::exception&) {
            // Ignore invalid tokens
        }
    }

    return choices;
}

// Check if a label should be transformed based on percentage
bool shouldTransformLabel(double transformationPercentage) {
    // Generate random number between 0 and 100
    double randomValue = (static_cast<double>(rand()) / RAND_MAX) * 100.0;
    return randomValue < transformationPercentage;
}

// Structure to represent a MIDI note event
struct MidiEvent {
    int track;
    int noteNumber;
    int startTime;
    int duration;
    bool isNoteOn;
};

// Application state
struct AppState {
    std::string inputFile;
    std::string outputFile;
    std::string midiOutputFile;
    double transformationPercentage = 50.0;
    std::vector<std::string> selectedVariants;
    bool processingComplete = false;
    std::string statusMessage;
    std::string resultSummary;
    int totalEligibleNotes = 0;
    int transformedNotes = 0;
    std::map<std::string, int> variantUsageCount;
};

// Function to process file with GUI integration
void processFile(const std::string& inputFile, const std::string& outputFile, AppState& state) {
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);

    if (!input.is_open() || !output.is_open()) {
        state.statusMessage = "Error opening files.";
        return;
    }

    // Write header to the output file
    output << std::left << std::setw(11) << "Track"
           << std::setw(11) << "Note"
           << std::setw(20) << "Duration"
           << std::setw(20) << "Label"
           << std::setw(25) << "Turn_Variant"
           << "\n";
    output << "---------------------------------------------------------------------------------\n";

    // Reset statistics
    state.totalEligibleNotes = 0;
    state.transformedNotes = 0;
    state.variantUsageCount.clear();

    std::string line;
    while (std::getline(input, line)) {
        std::istringstream ss(line);

        int track, duration;
        std::string noteName, label;

        // Parse line with Note in string format (e.g., "C4")
        if (!(ss >> track >> noteName >> duration)) {
            output << line << "\n";  // Handle malformed lines
            continue;
        }

        std::getline(ss, label);
        label.erase(0, label.find_first_not_of(" \t"));  // Trim leading whitespace
        // Remove trailing carriage return and whitespace (Windows line endings)
        label.erase(label.find_last_not_of(" \t\r\n") + 1);

        // Check if this label is eligible for transformation
        if (label == "I8" || label == "U2R" || label == "SPD" || label == "CH" ||
            label == "CW" || label == "CD" || label == "HT" || label == "FM" ||
            label == "SLP" || label == "RN" || label == "LAD" || label == "DNW" || label == "SAN"||
            label == "RTR2" || label == "TNTDN"||label == "SMP" || label == "LP"||
            label == "DHT" || label == "LNR"||label == "TNTLN" || label == "TNTTN"||label == "RTD2") {

            state.totalEligibleNotes++;

            // Check if this note should be transformed based on percentage
            if (shouldTransformLabel(state.transformationPercentage)) {
                state.transformedNotes++;

                try {
                    // Convert note name to MIDI number
                    int noteIndex = getNoteNumber(noteName);

                    // Randomly select a variant from the user's choices
                    std::string selectedVariant;
                    if (state.selectedVariants.empty() || (state.selectedVariants.size() == 1 && state.selectedVariants[0] == "RANDOM")) {
                        // Use a random variant from the complete list
                        std::vector<TurnVariant> allVariants = generateRandomTurnVariantPool(100); // Get a large pool
                        selectedVariant = allVariants[rand() % allVariants.size()].name;
                    } else {
                        // Use one of the user's selected variants randomly
                        selectedVariant = state.selectedVariants[rand() % state.selectedVariants.size()];
                    }

                    // Apply turn transformation
                    auto transformed = applyTurnVariants(noteIndex, duration, DUPLE, selectedVariant);

                    // Track variant usage
                    state.variantUsageCount[selectedVariant]++;

                    // Output the transformed notes
                    for (const auto& [transformedNote, transformedDuration] : transformed) {
                        std::string transNote = getNoteName(transformedNote); // Convert MIDI to readable name
                        output << std::left
                               << std::setw(11) << track
                               << std::setw(11) << transNote
                               << std::setw(20) << transformedDuration
                               << std::setw(20) << label
                               << std::setw(25) << selectedVariant
                               << "\n";
                    }
                } catch (const std::exception& e) {
                    // Handle cases where getNoteNumber produces an error
                    state.statusMessage += "Error processing note '" + noteName + "': " + e.what() + "\n";
                }
            } else {
                // Output original data for notes not selected for transformation
                output << std::left
                       << std::setw(11) << track
                       << std::setw(11) << noteName
                       << std::setw(20) << duration
                       << std::setw(20) << label
                       << std::setw(25) << "ORIGINAL" // Mark as original
                       << "\n";
            }
        } else {
            // Output original data for non-eligible labels
            output << std::left
                   << std::setw(11) << track
                   << std::setw(11) << noteName
                   << std::setw(20) << duration
                   << std::setw(20) << label
                   << std::setw(25) << "" // Empty variant column
                   << "\n";
        }
    }

    input.close();
    output.close();

    // Calculate actual percentage
    double actualPercentage = state.totalEligibleNotes > 0 ?
        (static_cast<double>(state.transformedNotes) / state.totalEligibleNotes) * 100.0 : 0.0;

    // Update result summary
    std::stringstream summary;
    summary << "Transformation Statistics:\n"
            << "Total eligible notes found: " << state.totalEligibleNotes << "\n"
            << "Notes transformed: " << state.transformedNotes << "\n"
            << "Actual transformation percentage: " << std::fixed << std::setprecision(1)
            << actualPercentage << "%\n\n";

    if (state.selectedVariants.size() == 1 && state.selectedVariants[0] != "RANDOM") {
        summary << "Variant used: " << state.selectedVariants[0] << "\n";
    } else if (state.selectedVariants.size() > 1) {
        summary << "Variants used (" << state.selectedVariants.size() << " total):\n";
        for (const auto& [variant, count] : state.variantUsageCount) {
            summary << "  " << variant << ": " << count << " times\n";
        }
    } else {
        summary << "Variant selection: Random\n";
    }

    summary << "Processing complete. Transformed results written to " << outputFile << "\n";
    state.resultSummary = summary.str();
    state.statusMessage = "Processing complete!";
    state.processingComplete = true;
}

// Function to convert processed data to MIDI file with MIDI sync fix
void convertToMidi(const std::string& inputFile, const std::string& outputFile, AppState& state) {
    std::ifstream input(inputFile);
    if (!input.is_open()) {
        state.statusMessage += "Error opening input file: " + inputFile + "\n";
        return;
    }

    // Skip header lines
    std::string line;
    std::getline(input, line); // Skip column headers
    std::getline(input, line); // Skip separator line

    // Parse the file and collect note events
    std::map<int, std::vector<MidiEvent>> trackEvents;
    std::map<int, int> trackPositions; // FIXED: Track positions for sequential notes within each track

    while (std::getline(input, line)) {
        std::istringstream ss(line);
        int track;
        std::string noteName;
        int duration;
        std::string label, variant;

        // Skip lines that don't contain note data
        if (line.empty() || line[0] == '-' || line.find("MIDI File Analyzed") != std::string::npos) {
            continue;
        }

        // Parse the line
        if (!(ss >> track >> noteName >> duration)) {
            continue; // Skip malformed lines
        }

        // Skip header or non-note lines
        if (noteName == "Note" || noteName == "Track") {
            continue;
        }

        try {
            int noteNumber = getNoteNumber(noteName);

            // FIXED: Use track-specific positioning for sequential notes within each track
            int& trackPosition = trackPositions[track];

            // Create note-on event at the track's current position
            MidiEvent noteOn{track, noteNumber, trackPosition, duration, true};
            trackEvents[track].push_back(noteOn);

            // Create note-off event
            MidiEvent noteOff{track, noteNumber, trackPosition + duration, 0, false};
            trackEvents[track].push_back(noteOff);

            // Update the position for this track (notes within a track are sequential)
            trackPosition += duration;

        } catch (const std::exception& e) {
            state.statusMessage += "Error processing note '" + noteName + "': " + std::string(e.what()) + "\n";
        }
    }

    input.close();

    // Write MIDI file
    std::ofstream midiFile(outputFile, std::ios::binary);
    if (!midiFile.is_open()) {
        state.statusMessage += "Error opening output MIDI file: " + outputFile + "\n";
        return;
    }

    // Write MIDI header
    // Format: MThd + <length> + <format> + <tracks> + <division>
    midiFile.write("MThd", 4); // Chunk type

    // Header length (always 6 bytes)
    char headerLength[4] = {0, 0, 0, 6};
    midiFile.write(headerLength, 4);

    // Format (0 = single track, 1 = multiple tracks, same timebase)
    char format[2] = {0, 1};
    midiFile.write(format, 2);

    // Number of tracks
    int numTracks = trackEvents.size();
    char tracksCount[2] = {static_cast<char>((numTracks >> 8) & 0xFF),
                          static_cast<char>(numTracks & 0xFF)};
    midiFile.write(tracksCount, 2);

    // Division (ticks per quarter note = 1024)
    char division[2] = {0x04, 0x00}; // 1024 in big-endian
    midiFile.write(division, 2);

    // Write each track
    for (const auto& [trackNum, events] : trackEvents) {
        // Sort events by time
        std::vector<MidiEvent> sortedEvents = events;
        std::sort(sortedEvents.begin(), sortedEvents.end(),
                 [](const MidiEvent& a, const MidiEvent& b) {
                     return a.startTime < b.startTime ||
                            (a.startTime == b.startTime && !a.isNoteOn && b.isNoteOn);
                 });

        // Write track header
        midiFile.write("MTrk", 4);

        // Placeholder for track length (will be filled in later)
        long trackLengthPos = midiFile.tellp();
        midiFile.write("\0\0\0\0", 4);

        // Track start position
        long trackStartPos = midiFile.tellp();

        // Write track events
        int lastTime = 0;

        // Set instrument (program change) - using piano (0) as default
        char programChange[3] = {0x00, static_cast<char>(0xC0), 0x00}; // Delta time, command, program number
        midiFile.write(programChange, 3);

        for (const auto& event : sortedEvents) {
            // Write delta time (variable length)
            int deltaTime = event.startTime - lastTime;
            lastTime = event.startTime;

            // Convert delta time to variable length quantity
            std::vector<char> vlq;
            if (deltaTime == 0) {
                vlq.push_back(0);
            } else {
                while (deltaTime > 0) {
                    char byte = deltaTime & 0x7F;
                    deltaTime >>= 7;
                    if (!vlq.empty()) {
                        byte |= 0x80;
                    }
                    vlq.push_back(byte);
                }
                std::reverse(vlq.begin(), vlq.end());
            }

            for (char byte : vlq) {
                midiFile.put(byte);
            }

            // Write note event
            if (event.isNoteOn) {
                // Note on: 0x90 | channel, note, velocity
                midiFile.put(0x90);
                midiFile.put(static_cast<char>(event.noteNumber));
                midiFile.put(0x64); // Velocity (100)
            } else {
                // Note off: 0x80 | channel, note, velocity
                midiFile.put(0x80);
                midiFile.put(static_cast<char>(event.noteNumber));
                midiFile.put(0x00); // Velocity (0)
            }
        }

        // Write end of track
        midiFile.put(0x00); // Delta time
        midiFile.put(0xFF); // Meta event
        midiFile.put(0x2F); // End of track
        midiFile.put(0x00); // Length

        // Calculate and write track length
        long trackEndPos = midiFile.tellp();
        long trackLength = trackEndPos - trackStartPos;

        midiFile.seekp(trackLengthPos);
        char trackLengthBytes[4] = {
            static_cast<char>((trackLength >> 24) & 0xFF),
            static_cast<char>((trackLength >> 16) & 0xFF),
            static_cast<char>((trackLength >> 8) & 0xFF),
            static_cast<char>(trackLength & 0xFF)
        };
        midiFile.write(trackLengthBytes, 4);
        midiFile.seekp(trackEndPos);
    }

    midiFile.close();
    state.statusMessage += "MIDI file created successfully: " + outputFile + "\n";
}