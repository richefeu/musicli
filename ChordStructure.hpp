#pragma once

/**
 * @file ChordStructure.hpp
 * @brief Structure et calcul des accords musicaux
 * 
 * Ce fichier contient la classe ChordStructure qui gère la représentation
 * et le calcul des accords musicaux à partir de formules.
 * 
 * La classe permet de :
 * - Parser des formules d'accords (ex: "1,3,5" pour un accord majeur)
 * - Calculer les notes d'un accord pour une fondamentale donnée
 * - Gérer les enharmonies et les noms de notes alternatifs
 * - Valider les formules d'accords
 * 
 * @author Mistral Vibe
 * @date 2024
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <stdexcept>

/**
 * @class ChordStructure
 * @brief Représente la structure musicale d'un accord
 * 
 * Cette classe encapsule la logique musicale pour analyser et calculer
 * les notes d'un accord à partir de sa formule.
 * 
 * Exemples de formules supportées :
 * - "1,3,5" : Accord majeur (fondamentale, tierce majeure, quinte)
 * - "1,3-,5" : Accord mineur (fondamentale, tierce mineure, quinte)
 * - "1,3,5,7-" : Accord de septième (fondamentale, tierce majeure, quinte, septième mineure)
 */
class ChordStructure {
private:
    std::string formula; ///< Formule de l'accord (ex: "1,3,5")
    std::vector<std::pair<int, int>> degrees; ///< Degrés parsés avec modifications
    static const int degreeToSemitones[8]; ///< Conversion degré -> demi-tons

    /** Table de conversion des degrés en noms de notes */
    static inline const std::vector<char> degreeToNote = {'X', 'C', 'D', 'E', 'F', 'G', 'A', 'B'};

    /** Table de conversion des noms de notes en demi-tons (avec enharmonies) */
    static inline const std::map<std::string, int> noteToSemitone = {
        {"C", 0}, {"B#", 0}, {"C#", 1}, {"Db", 1}, {"D", 2}, {"Eb", 3}, {"D#", 3},
        {"E", 4}, {"Fb", 4}, {"F", 5}, {"E#", 5}, {"F#", 6}, {"Gb", 6}, {"G", 7},
        {"G#", 8}, {"Ab", 8}, {"A", 9}, {"A#", 10}, {"Bb", 10}, {"B", 11}, {"Cb", 11}
    };

    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t");
        return str.substr(start, end - start + 1);
    }

    void parseToken(const std::string& token) {
        size_t pos = 0;
        int degree = std::stoi(token, &pos);
        int modification = 0;
        if (pos < token.size()) {
            char mod = token[pos];
            if (mod == '-') modification = -1;
            else if (mod == '+') modification = +1;
        }
        degrees.emplace_back(degree, modification);
    }

    int degreeToInterval(int degree, int modification) const {
        if (degree < 1 || degree > 7) return -1;
        return degreeToSemitones[degree] + modification;
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

    std::string generateKey(const std::vector<int>& intervals) const {
        std::vector<int> sorted = intervals;
        std::sort(sorted.begin(), sorted.end());
        std::string key;
        for (size_t i = 0; i < sorted.size(); ++i) {
            if (i > 0) key += ",";
            key += std::to_string(sorted[i]);
        }
        return key;
    }

public:
    /**
     * @brief Constructeur de ChordStructure
     * @param formula Formule de l'accord (ex: "1,3,5" ou "1,3-,5")
     * @throw std::invalid_argument Si la formule est invalide
     */
    explicit ChordStructure(const std::string& formula) : formula(formula) {
        parseFormula();
    }

    /**
     * @brief Parse la formule de l'accord et valide sa structure
     * 
     * Cette méthode analyse la formule et la décompose en degrés.
     * Elle vérifie que la formule est valide et contient au moins
     * la fondamentale (degré 1).
     * 
     * @throw std::invalid_argument Si la formule ne contient pas le degré 1
     *        ou si la syntaxe est invalide
     */
    void parseFormula() {
        degrees.clear();
        if (formula.find(',') == std::string::npos) {
            throw std::invalid_argument(
                "Formule invalide : les degrés doivent être séparés par des virgules (ex: \"3,5\")."
            );
        }

        size_t start = 0;
        size_t end = formula.find(',');
        while (end != std::string::npos) {
            std::string token = trim(formula.substr(start, end - start));
            if (!token.empty()) parseToken(token);
            start = end + 1;
            end = formula.find(',', start);
        }
        std::string token = trim(formula.substr(start));
        if (!token.empty()) parseToken(token);

        // Ajouter la fondamentale si absente
        bool hasRoot = false;
        for (const auto& [degree, mod] : degrees) {
            if (degree == 1) {
                hasRoot = true;
                break;
            }
        }
        if (!hasRoot) {
            degrees.insert(degrees.begin(), {1, 0});
        }
    }

    std::vector<int> getIntervalsInSemitones() const {
        std::vector<int> intervals;
        for (const auto& [degree, mod] : degrees) {
            intervals.push_back(degreeToInterval(degree, mod));
        }
        return intervals;
    }

    std::string getChordTypeName() const {
        static const std::unordered_map<std::string, std::string> chordSignatures = {
            {"0,4,7", ""},        // Majeur
            {"0,3,7", "m"},       // Mineur
            {"0,4,7,10", "7"},    // Dominante 7
            {"0,3,7,10", "m7"},   // Mineur 7
            {"0,4,7,11", "maj7"}, // Majeur 7
            {"0,4,8", "aug"},     // Augmenté
            {"0,3,6", "dim"},     // Diminué
            {"0,3,6,9", "dim7"},  // Diminué 7
            {"0,2,7", "sus2"},    // Sus2
            {"0,5,7", "sus4"}     // Sus4
        };
        std::vector<int> intervals = getIntervalsInSemitones();
        std::string key = generateKey(intervals);
        auto it = chordSignatures.find(key);
        return (it != chordSignatures.end()) ? it->second : "custom";
    }

    std::string getFormula() const { return formula; }

    void print() const {
        std::cout << "Formule: " << formula << "\n";
        std::cout << "Degrés: ";
        for (const auto& [degree, mod] : degrees) {
            std::cout << degree;
            if (mod != 0) std::cout << (mod > 0 ? "+" : "") << mod;
            std::cout << " ";
        }
        std::cout << "\nIntervalles (demi-tons): ";
        for (int interval : getIntervalsInSemitones()) {
            std::cout << interval << " ";
        }
        std::cout << "\nType: " << getChordTypeName() << "\n";
    }

    /**
     * @brief Calcule les notes d'un accord pour une fondamentale donnée
     * 
     * Cette méthode applique la formule de l'accord à une note fondamentale
     * pour calculer les notes spécifiques de l'accord.
     * 
     * Exemple :
     * - Formule "1,3,5" avec fondamentale "C" -> notes ["C", "E", "G"]
     * - Formule "1,3-,5" avec fondamentale "C" -> notes ["C", "Eb", "G"]
     * 
     * @param rootNote Note fondamentale de l'accord (ex: "C", "G#", "Bb")
     * @return Vecteur de noms de notes (ex: ["C", "E", "G"])
     * @throw std::invalid_argument Si la note fondamentale est invalide
     * 
     * @note La méthode gère automatiquement les enharmonies pour choisir
     *       les noms de notes les plus appropriés (ex: "F#" au lieu de "Gb")
     */
    std::vector<std::string> getChordNotes(const std::string& rootNote) const {
        auto it = noteToSemitone.find(rootNote);
        if (it == noteToSemitone.end()) {
            throw std::invalid_argument("Note racine invalide : " + rootNote);
        }
        int rootSemitone = it->second;

        std::vector<std::string> notes;
        for (const auto& [degree, modification] : degrees) {
            char noteLetter = getLetterForDegree(rootNote, degree);

            int expectedSemitone = degreeToSemitones[degree];
            int targetSemitone = (rootSemitone + expectedSemitone + modification) % 12;

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
};

const int ChordStructure::degreeToSemitones[8] = {0, 0, 2, 4, 5, 7, 9, 11};