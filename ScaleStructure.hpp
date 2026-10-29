
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
        if (i == 0) {
            throw std::invalid_argument("Degré invalide : \"" + degreeStr + "\".");
        }
        int degree = std::stoi(degreeStr.substr(0, i));

        if (degree < 1 || degree > 7) {
            throw std::invalid_argument("Degré invalide : " + degreeStr + ". Doit être entre 1 et 7.");
        }

        int alteration = 0;
        for (; i < degreeStr.size(); ++i) {
            if (degreeStr[i] == '-') {
                alteration--;
            } else if (degreeStr[i] == '+') {
                alteration++;
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
        std::string name(1, baseLetter);
        if (semitoneOffset > 0) {
            name += std::string(semitoneOffset, '#');
        } else if (semitoneOffset < 0) {
            name += std::string(-semitoneOffset, 'b');
        }
        return name;
    }

    // Altération (en demi-tons, entre -6 et +5) qui amène la lettre sur la hauteur visée.
    static int alterationFor(char letter, int targetSemitone) {
        int alteration = ((targetSemitone - getLetterSemitone(letter)) % 12 + 12) % 12;
        return alteration > 6 ? alteration - 12 : alteration;
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

        // Orthographe théorique stricte : une lettre par degré, avec autant
        // d'altérations que nécessaire (C# majeur : E#, B# ; D# majeur : F##, C##).
        for (const auto& di : degrees) {
            char noteLetter = getLetterForDegree(rootNote, di.degree);
            int targetSemitone = rootSemitone + degreeToSemitones[di.degree] + di.alteration;
            notes.push_back(buildNoteName(noteLetter, alterationFor(noteLetter, targetSemitone)));
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

    // Degrés de la gamme ("1", "3-", "4+" …), dans le même ordre que getNotes().
    std::vector<std::string> getDegreeLabels() const {
        std::vector<std::string> labels;
        for (const auto& di : degrees) {
            std::string l = std::to_string(di.degree);
            if (di.alteration > 0) l += std::string(di.alteration, '+');
            else if (di.alteration < 0) l += std::string(-di.alteration, '-');
            labels.push_back(l);
        }
        return labels;
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
