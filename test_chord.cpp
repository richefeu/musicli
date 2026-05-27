#include "ChordStructure.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

struct ChordTestCase {
    std::string name;
    std::string formula;
    std::string rootNote;
    std::vector<std::string> expectedNotes;
    std::string expectedType;
};

bool compareNotes(const std::vector<std::string>& result, const std::vector<std::string>& expected) {
    if (result.size() != expected.size()) return false;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] != expected[i]) return false;
    }
    return true;
}

int main() {
    std::vector<ChordTestCase> tests = {
        {"Accord majeur (C)", "3,5", "C", {"C", "E", "G"}, ""},
        {"Accord majeur (G)", "3,5", "G", {"G", "B", "D"}, ""},
        {"Accord mineur (A)", "3-,5", "A", {"A", "C", "E"}, "m"},
        {"Accord mineur (E)", "3-,5", "E", {"E", "G", "B"}, "m"},
        {"Accord dominant 7 (C)", "3,5,7-", "C", {"C", "E", "G", "Bb"}, "7"},
        {"Accord mineur 7 (A)", "3-,5,7-", "A", {"A", "C", "E", "G"}, "m7"},
        {"Accord majeur 7 (C)", "3,5,7", "C", {"C", "E", "G", "B"}, "maj7"},
        {"Accord augmenté (C)", "3,5+", "C", {"C", "E", "G#"}, "aug"},
        {"Sus2 (C)", "2,5", "C", {"C", "D", "G"}, "sus2"},
        {"Sus4 (C)", "4,5", "C", {"C", "F", "G"}, "sus4"},
    };

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "CHORD STRUCTURE TESTS" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;

    int passed = 0;
    int failed = 0;

    for (const auto& test : tests) {
        try {
            ChordStructure chord(test.formula);
            auto result = chord.getChordNotes(test.rootNote);
            std::string chordType = chord.getChordTypeName();

            bool notesMatch = compareNotes(result, test.expectedNotes);
            bool typeMatch = chordType == test.expectedType;
            bool success = notesMatch && typeMatch;

            if (success) {
                passed++;
                std::cout << "✓ " << std::left << std::setw(30) << test.name << " : ";
                for (const auto& note : result) {
                    std::cout << note << " ";
                }
                std::cout << "(" << chordType << ")" << std::endl;
            } else {
                failed++;
                std::cout << "✗ " << std::left << std::setw(30) << test.name << std::endl;
                if (!notesMatch) {
                    std::cout << "  Got notes : ";
                    for (const auto& note : result) {
                        std::cout << note << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "  Expected  : ";
                    for (const auto& note : test.expectedNotes) {
                        std::cout << note << " ";
                    }
                    std::cout << std::endl;
                }
                if (!typeMatch) {
                    std::cout << "  Got type : " << chordType << std::endl;
                    std::cout << "  Expected : " << test.expectedType << std::endl;
                }
            }
        } catch (const std::exception& e) {
            failed++;
            std::cout << "✗ " << std::left << std::setw(30) << test.name << " : ERROR" << std::endl;
            std::cout << "  " << e.what() << std::endl;
        }
    }

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;

    return failed > 0 ? 1 : 0;
}
