#include "ScaleStructure.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

struct TestCase {
    std::string name;
    std::string formula;
    std::string rootNote;
    std::vector<std::string> expected;
};

bool compareNotes(const std::vector<std::string>& result, const std::vector<std::string>& expected) {
    if (result.size() != expected.size()) return false;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] != expected[i]) return false;
    }
    return true;
}

int main() {
    std::vector<TestCase> tests = {
        {"Gamme majeure (C)", "1,2,3,4,5,6,7", "C", {"C", "D", "E", "F", "G", "A", "B"}},
        {"Gamme majeure (D)", "1,2,3,4,5,6,7", "D", {"D", "E", "F#", "G", "A", "B", "C#"}},
        {"Gamme majeure (C#)", "1,2,3,4,5,6,7", "C#", {"C#", "D#", "E#", "F#", "G#", "A#", "B#"}},
        {"Gamme majeure (Db)", "1,2,3,4,5,6,7", "Db", {"Db", "Eb", "F", "Gb", "Ab", "Bb", "C"}},
        {"Gamme mineure (A)", "1,2,3-,4,5,6-,7-", "A", {"A", "B", "C", "D", "E", "F", "G"}},
        {"Gamme mineure (E)", "1,2,3-,4,5,6-,7-", "E", {"E", "F#", "G", "A", "B", "C", "D"}},
        {"Pentatonique (C)", "1,2,3,5,6", "C", {"C", "D", "E", "G", "A"}},
        {"Blues (C)", "1,3-,4,4+,5,7-", "C", {"C", "Eb", "F", "F#", "G", "Bb"}},
        {"Ton entier (E)", "1,2,3,4+,5+,6+", "E", {"E", "F#", "G#", "A#", "B#", "C##"}},
        {"Ton entier (A)", "1,2,3,4+,5+,6+", "A", {"A", "B", "C#", "D#", "E#", "F##"}},
        {"Ton entier (B)", "1,2,3,4+,5+,6+", "B", {"B", "C#", "D#", "E#", "F##", "G##"}},
        {"Gamme majeure (F#)", "1,2,3,4,5,6,7", "F#", {"F#", "G#", "A#", "B", "C#", "D#", "E#"}},
        {"Gamme majeure (Gb)", "1,2,3,4,5,6,7", "Gb", {"Gb", "Ab", "Bb", "Cb", "Db", "Eb", "F"}},
        {"Pentatonique (G#)", "1,2,3,5,6", "G#", {"G#", "A#", "B#", "D#", "E#"}},
        {"Lydien (B)", "1,2,3,4+,5,6,7", "B", {"B", "C#", "D#", "E#", "F#", "G#", "A#"}},
        {"Mineure harmonique (D#)", "1,2,3-,4,5,6-,7", "D#", {"D#", "E#", "F#", "G#", "A#", "B", "C##"}},
        {"Gamme majeure (D#)", "1,2,3,4,5,6,7", "D#", {"D#", "E#", "F##", "G#", "A#", "B#", "C##"}},
        {"Mineure naturelle (Db)", "1,2,3-,4,5,6-,7-", "Db", {"Db", "Eb", "Fb", "Gb", "Ab", "Bbb", "Cb"}},
    };

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SCALE STRUCTURE TESTS" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;

    int passed = 0;
    int failed = 0;

    for (const auto& test : tests) {
        try {
            ScaleStructure scale(test.formula);
            auto result = scale.getNotes(test.rootNote);
            bool success = compareNotes(result, test.expected);

            if (success) {
                passed++;
                std::cout << "✓ " << std::left << std::setw(30) << test.name << " : ";
                for (const auto& note : result) {
                    std::cout << note << " ";
                }
                std::cout << std::endl;
            } else {
                failed++;
                std::cout << "✗ " << std::left << std::setw(30) << test.name << " : ";
                for (const auto& note : result) {
                    std::cout << note << " ";
                }
                std::cout << std::endl;
                std::cout << "  Expected : ";
                for (const auto& note : test.expected) {
                    std::cout << note << " ";
                }
                std::cout << std::endl;
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
