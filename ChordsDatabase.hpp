#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>

struct ChordEntry {
    std::string name;
    std::string formula;
    std::string description;
    std::vector<std::string> aliases;
};

class ChordsDatabase {
private:
    static const std::map<std::string, ChordEntry> database;

public:
    // Récupérer un accord par son nom
    static const ChordEntry& getChord(const std::string& name) {
        auto it = database.find(name);
        if (it == database.end()) {
            throw std::invalid_argument("Accord non trouvé : " + name);
        }
        return it->second;
    }

    // Récupérer la formule d'un accord
    static std::string getFormula(const std::string& name) {
        return getChord(name).formula;
    }

    // Lister tous les noms d'accords disponibles (ordonnés par importance)
    static std::vector<std::string> listChords() {
        // Ordre personnalisé par importance
        std::vector<std::string> orderedChords = {
            "major",           // Accord majeur - le plus important
            "minor",           // Accord mineur
            "dominant7",       // Accord de dominante 7
            "major7",          // Accord majeur 7
            "minor7",          // Accord mineur 7
            "augmented",       // Accord augmenté
            "diminished",      // Accord diminué
            "sus2",            // Accord sus2
            "sus4",            // Accord sus4
            "dominant9",       // Accord 9
            "major9",          // Accord majeur 9
            "minor9",          // Accord mineur 9
            "dominant11",      // Accord 11
            "major11",         // Accord majeur 11
            "minor11",         // Accord mineur 11
            "dominant13",      // Accord 13
            "major13",         // Accord majeur 13
            "minor13",         // Accord mineur 13
            "minor7flat5",     // Accord demi-diminué
            "diminished7",      // Accord diminué 7
            "major7sharp5",    // Accord majeur 7#5
            "dominant7sharp5", // Accord 7#5
            "dominant7flat5",  // Accord 7b5
            "sus2_7",          // Accord sus2 7
            "sus4_7",          // Accord sus4 7
            "sus4_major7",     // Accord sus4 majeur 7
            "add9",            // Accord add9
            "minor_add9",      // Accord mineur add9
            "add11",           // Accord add11
            "minor_major7",     // Accord mineur majeur 7
            "7flat9",          // Accord 7b9
            "7sharp9",          // Accord 7#9 (Hendrix)
            "power_chord",      // Power chord
            "root_and_third",   // Racine et tierce
            "polychord_maj_min", // Polychord
            "quartal",          // Accord quartal
            "quintal"           // Accord quintal
        };
        
        // Vérifier que tous les accords existent et ajouter ceux qui manquent
        std::vector<std::string> result;
        for (const auto& chordName : orderedChords) {
            if (database.find(chordName) != database.end()) {
                result.push_back(chordName);
            }
        }
        
        // Ajouter les accords qui n'étaient pas dans la liste ordonnée
        for (const auto& [name, _] : database) {
            if (std::find(result.begin(), result.end(), name) == result.end()) {
                result.push_back(name);
            }
        }
        
        return result;
    }

};

// ==================== BASE DE DONNÉES ====================

const std::map<std::string, ChordEntry> ChordsDatabase::database = {
    // Accords triades (3 notes)
    {"major", {"Majeur", "3,5", "Accord majeur (Do, Mi, Sol)", {"maj", "major_triad"}}},
    {"minor", {"Mineur", "3-,5", "Accord mineur (Do, Mib, Sol)", {"min", "minor_triad"}}},
    {"augmented", {"Augmenté", "3,5+", "Accord augmenté (Do, Mi, Sol#)", {"aug", "plus", "sharp5"}}},
    {"diminished", {"Diminué", "3-,5-", "Accord diminué (Do, Mib, Solb)", {"dim"}}},

    // Accords sus (suspended)
    {"sus2", {"Sus2", "2,5", "Accord suspendu 2 (Do, Ré, Sol)", {"sus2"}}},
    {"sus4", {"Sus4", "4,5", "Accord suspendu 4 (Do, Fa, Sol)", {"sus4"}}},

    // Accords de dominante (7ème mineure)
    {"dominant7", {"Dominante 7", "3,5,7-", "Accord majeur avec 7ème mineure", {"7", "dominant"}}},
    {"minor7", {"Mineur 7", "3-,5,7-", "Accord mineur avec 7ème mineure", {"m7", "min7"}}},
    {"minor7flat5", {"Mineur 7b5 (demi-diminué)", "3-,5-,7-", "Accord demi-diminué", {"m7b5", "min7b5", "half_diminished"}}},
    {"diminished7", {"Diminué 7", "3-,5-,7--", "Accord diminué avec 7ème diminuée (Do, Mib, Solb, Sibb)", {"dim7", "o7"}}},

    // Accords majeurs (7ème majeure)
    {"major7", {"Majeur 7", "3,5,7", "Accord majeur avec 7ème majeure", {"maj7", "M7"}}},
    {"major9", {"Majeur 9", "3,5,7,2", "Accord majeur 7 avec 9ème", {"maj9", "M9"}}},
    {"major11", {"Majeur 11", "3,5,7,4", "Accord majeur 7 avec 11ème", {"maj11", "M11"}}},
    {"major13", {"Majeur 13", "3,5,7,6", "Accord majeur 7 avec 13ème", {"maj13", "M13"}}},

    // Accords 9ème, 11ème, 13ème (dominante)
    {"dominant9", {"Dominante 9", "3,5,7-,2", "Accord 7 avec 9ème", {"9", "dom9"}}},
    {"dominant11", {"Dominante 11", "3,5,7-,4", "Accord 7 avec 11ème", {"11", "dom11"}}},
    {"dominant13", {"Dominante 13", "3,5,7-,6", "Accord 7 avec 13ème", {"13", "dom13"}}},

    // Accords mineurs 9, 11, 13
    {"minor9", {"Mineur 9", "3-,5,7-,2", "Accord mineur 7 avec 9ème", {"m9", "min9"}}},
    {"minor11", {"Mineur 11", "3-,5,7-,4", "Accord mineur 7 avec 11ème", {"m11", "min11"}}},
    {"minor13", {"Mineur 13", "3-,5,7-,6", "Accord mineur 7 avec 13ème", {"m13", "min13"}}},

    // Accords avec extensions
    {"major7sharp5", {"Majeur 7#5", "3,5+,7", "Accord majeur 7 augmenté", {"maj7+5", "M7+5"}}},
    {"dominant7sharp5", {"Dominante 7#5", "3,5+,7-", "Accord 7 augmenté", {"7+5", "7#5"}}},
    {"dominant7flat5", {"Dominante 7b5", "3,5-,7-", "Accord 7 avec quinte diminuée", {"7b5", "7-5"}}},

    // Accords sus avec 7ème
    {"sus2_7", {"Sus2 7", "2,5,7-", "Accord sus2 avec 7ème mineure", {"sus2_7"}}},
    {"sus4_7", {"Sus4 7", "4,5,7-", "Accord sus4 avec 7ème mineure", {"sus4_7", "sus4_7"}}},
    {"sus4_major7", {"Sus4 Majeur 7", "4,5,7", "Accord sus4 avec 7ème majeure", {"sus4_maj7", "sus4_M7"}}},

    // Accords add (avec note additionnelle)
    {"add9", {"Add9", "3,5,2", "Accord majeur avec 9ème ajoutée", {"add9", "add2"}}},
    {"minor_add9", {"Mineur Add9", "3-,5,2", "Accord mineur avec 9ème ajoutée", {"m_add9", "min_add9"}}},
    {"add11", {"Add11", "3,5,4", "Accord majeur avec 11ème ajoutée", {"add11", "add4"}}},

    // Accords avec basse (bass notes)
    {"power_chord", {"Power chord (quinte)", "5", "Seulement la quinte (no 3)", {"power", "5_chord"}}},
    {"root_and_third", {"Racine et tierce", "3", "Seulement la tierce", {"third_only"}}},

    // Accords Jazz
    {"minor_major7", {"Mineur Majeur 7", "3-,5,7", "Accord mineur avec 7ème majeure", {"m_maj7", "min_maj7"}}},
    {"7flat9", {"7b9", "3,5,7-,2-", "Accord 7 avec 9ème diminuée", {"7b9", "7-9"}}},
    {"7sharp9", {"7#9 (Hendrix)", "3,5,7-,2+", "Accord 7 avec 9ème augmentée", {"7#9", "7+9"}}},

    // Accords spéciaux
    {"polychord_maj_min", {"Tierce majeure et mineure", "3,5,3-", "Tierces majeure et mineure simultanées (son blues, cf. 7#9)", {"split_third"}}},
    {"quartal", {"Quartal", "4,7-", "Empilement de quartes justes (Do, Fa, Sib)", {"quartal"}}},
    {"quintal", {"Quintal", "5,2", "Empilement de quintes justes (Do, Sol, Ré)", {"quintal"}}},
};
