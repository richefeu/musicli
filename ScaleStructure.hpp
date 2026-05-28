
#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <sstream>
#include <cctype>

struct DegreeInfo {
    int degree;      // 1-7
    int alteration;  // -1, 0, +1 (resp. -, naturel, +)

    bool operator<(const DegreeInfo& other) const {
        return degree < other.degree;
    }
};

class ScaleStructure {
private:
    std::vector<DegreeInfo> degrees;

    static inline const std::vector<int> degreeToSemitones = {0, 0, 2, 4, 5, 7, 9, 11};
    static inline const std::vector<char> degreeToNote = {'X', 'C', 'D', 'E', 'F', 'G', 'A', 'B'};

    static inline const std::map<std::string, int> noteToSemitone = {
        {"C", 0}, {"B#", 0}, {"C#", 1}, {"Db", 1}, {"D", 2}, {"Eb", 3}, {"D#", 3},
        {"E", 4}, {"Fb", 4}, {"F", 5}, {"E#", 5}, {"F#", 6}, {"Gb", 6}, {"G", 7},
        {"G#", 8}, {"Ab", 8}, {"A", 9}, {"A#", 10}, {"Bb", 10}, {"B", 11}, {"Cb", 11}
    };

    DegreeInfo parseDegree(const std::string& degreeStr) const {
        if (degreeStr.empty()) {
            throw std::invalid_argument("Degré vide dans la formule.");
        }

        size_t i = 0;
        while (i < degreeStr.size() && isdigit(degreeStr[i])) {
            i++;
        }
        int degree = std::stoi(degreeStr.substr(0, i));

        if (degree < 1 || degree > 7) {
            throw std::invalid_argument("Degré invalide : " + degreeStr + ". Doit être entre 1 et 7.");
        }

        int alteration = 0;
        if (i < degreeStr.size()) {
            char modifier = degreeStr[i];
            if (modifier == '-') {
                alteration = -1;
            } else if (modifier == '+') {
                alteration = +1;
            } else {
                throw std::invalid_argument("Modificateur invalide : " + degreeStr + ". Utilisez '-' ou '+'.");
            }
        }

        return {degree, alteration};
    }

    void parseFormula(const std::string& formula) {
        degrees.clear();

        if (formula.find(' ') != std::string::npos) {
            throw std::invalid_argument("La formule doit utiliser des virgules comme séparateurs (ex: '1,2-,3-').");
        }

        std::vector<std::string> degreeStrings;
        std::stringstream ss(formula);
        std::string token;
        while (std::getline(ss, token, ',')) {
            degreeStrings.push_back(token);
        }

        bool hasFundamental = false;
        for (const auto& degreeStr : degreeStrings) {
            DegreeInfo di = parseDegree(degreeStr);
            degrees.push_back(di);
            if (di.degree == 1 && di.alteration == 0) {
                hasFundamental = true;
            }
        }

        if (!hasFundamental) {
            degrees.push_back({1, 0});
        }

        std::sort(degrees.begin(), degrees.end());

        // Valider l'unicité des intervalles (en semitones)
        std::vector<int> intervals;
        for (const auto& di : degrees) {
            int interval = degreeToSemitones[di.degree] + di.alteration;
            intervals.push_back(interval);
        }
        std::sort(intervals.begin(), intervals.end());
        if (std::unique(intervals.begin(), intervals.end()) != intervals.end()) {
            throw std::invalid_argument("La formule contient plusieurs notes avec le même intervalle.");
        }
    }

    int getLetterIndex(char letter) const {
        static const std::map<char, int> letterIndex = {
            {'C', 0}, {'D', 1}, {'E', 2}, {'F', 3}, {'G', 4}, {'A', 5}, {'B', 6}
        };
        return letterIndex.at(letter);
    }

    char getLetterForDegree(const std::string& rootNote, int degree) const {
        static const std::vector<char> letters = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
        int rootLetterIndex = getLetterIndex(rootNote[0]);
        int letterIndex = (rootLetterIndex + (degree - 1)) % 7;
        return letters[letterIndex];
    }

    static inline int getLetterSemitone(char letter) {
        static const std::map<char, int> letterSemitone = {
            {'C', 0}, {'D', 2}, {'E', 4}, {'F', 5}, {'G', 7}, {'A', 9}, {'B', 11}
        };
        return letterSemitone.at(letter);
    }

    std::string buildNoteName(char baseLetter, int semitoneOffset) const {
        // Respecter toujours la lettre de base du degré
        // et choisir entre # et b pour l'altération
        // selon les règles musicales standard
        
        if (semitoneOffset == 0) {
            return std::string(1, baseLetter);
        }
        
        // Pour les altérations de +1 ou -1, choisir entre # et b
        // en fonction de règles musicales standard
        if (semitoneOffset == 1) {
            // Pour +1, utiliser # par défaut
            // Ex: degré 3+ = F (pas E#), donc on utilise la lettre suivante
            if (baseLetter == 'E' || baseLetter == 'B') {
                char nextLetter = getNextLetter(baseLetter);
                return std::string(1, nextLetter); // F ou C
            }
            return std::string(1, baseLetter) + "#";
        }
        else if (semitoneOffset == -1) {
            // Pour -1, toujours utiliser b (convention standard)
            // Ex: degré 3- = Eb (pas D#), degré 6- = Ab (pas G#), degré 7- = Bb (pas A#)
            return std::string(1, baseLetter) + "b";
        }
        else if (semitoneOffset > 1) {
            // Pour les altérations multiples, toujours utiliser #
            return std::string(1, baseLetter) + std::string(semitoneOffset, '#');
        }
        else {
            // Pour les altérations multiples négatives, toujours utiliser b
            return std::string(1, baseLetter) + std::string(-semitoneOffset, 'b');
        }
    }

    char getNextLetter(char letter) const {
        static const std::map<char, char> nextLetters = {
            {'C', 'D'}, {'D', 'E'}, {'E', 'F'}, {'F', 'G'}, {'G', 'A'}, {'A', 'B'}, {'B', 'C'}
        };
        auto it = nextLetters.find(letter);
        return it != nextLetters.end() ? it->second : letter;
    }

    char getPreviousLetter(char letter) const {
        static const std::map<char, char> prevLetters = {
            {'C', 'B'}, {'D', 'C'}, {'E', 'D'}, {'F', 'E'}, {'G', 'F'}, {'A', 'G'}, {'B', 'A'}
        };
        auto it = prevLetters.find(letter);
        return it != prevLetters.end() ? it->second : letter;
    }

public:
    ScaleStructure(const std::string& formula) {
        parseFormula(formula);
    }

    std::vector<std::string> getNotes(const std::string& rootNote) const {
        std::vector<std::string> notes;

        auto it = noteToSemitone.find(rootNote);
        if (it == noteToSemitone.end()) {
            throw std::invalid_argument("Note racine invalide : " + rootNote);
        }
        int rootSemitone = it->second;

        for (const auto& di : degrees) {
            char noteLetter = getLetterForDegree(rootNote, di.degree);

            int expectedSemitone = degreeToSemitones[di.degree];
            int targetSemitone = (rootSemitone + expectedSemitone + di.alteration) % 12;

            int letterSemitone = getLetterSemitone(noteLetter);
            int alteration = ((targetSemitone - letterSemitone) % 12 + 12) % 12;

            if (alteration > 6) {
                alteration -= 12;
            }

            // Simplifier : éviter les doubles dièses/bémols en préférant la lettre adjacente
            if (alteration >= 2) {
                char next = getNextLetter(noteLetter);
                int nextSemitone = getLetterSemitone(next);
                int newAlt = ((targetSemitone - nextSemitone) % 12 + 12) % 12;
                if (newAlt > 6) newAlt -= 12;
                if (std::abs(newAlt) < std::abs(alteration)) {
                    noteLetter = next;
                    alteration = newAlt;
                }
            } else if (alteration <= -2) {
                char prev = getPreviousLetter(noteLetter);
                int prevSemitone = getLetterSemitone(prev);
                int newAlt = ((targetSemitone - prevSemitone) % 12 + 12) % 12;
                if (newAlt > 6) newAlt -= 12;
                if (std::abs(newAlt) < std::abs(alteration)) {
                    noteLetter = prev;
                    alteration = newAlt;
                }
            }

            std::string noteName = buildNoteName(noteLetter, alteration);
            notes.push_back(noteName);
        }

        return notes;
    }

    std::string getFormula() const {
        std::string formula;
        for (size_t i = 0; i < degrees.size(); ++i) {
            if (i > 0) formula += ",";
            formula += std::to_string(degrees[i].degree);
            if (degrees[i].alteration > 0) {
                formula += std::string(degrees[i].alteration, '+');
            } else if (degrees[i].alteration < 0) {
                formula += std::string(-degrees[i].alteration, '-');
            }
        }
        return formula;
    }

    // Détecter si cette gamme est probablement une gamme mineure
    bool isMinorScale() const {
        // Les gammes mineures ont généralement un 3e degré mineur (b3)
        for (const auto& di : degrees) {
            if (di.degree == 3 && di.alteration == -1) {
                return true;
            }
        }
        return false;
    }
};
