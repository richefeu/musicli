#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>

struct ScaleEntry {
    std::string name;
    std::string formula;
    std::string description;
    std::vector<std::string> aliases;
};

class ScalesDatabase {
private:
    static const std::map<std::string, ScaleEntry> database;

public:
    // Récupérer une gamme par son nom
    static const ScaleEntry& getScale(const std::string& name) {
        auto it = database.find(name);
        if (it == database.end()) {
            throw std::invalid_argument("Gamme non trouvée : " + name);
        }
        return it->second;
    }

    // Récupérer la formule d'une gamme
    static std::string getFormula(const std::string& name) {
        return getScale(name).formula;
    }

    // Lister tous les noms de gammes disponibles (ordonnés par importance)
    static std::vector<std::string> listScales() {
        // Ordre personnalisé par importance
        std::vector<std::string> orderedScales = {
            "major",           // Gamme majeure - la plus importante
            "minor_natural",   // Gamme mineure naturelle
            "minor_harmonic",  // Gamme mineure harmonique
            "minor_melodic",   // Gamme mineure mélodique
            "pentatonic_major", // Pentatonique majeure
            "pentatonic_minor", // Pentatonique mineure
            "pentatonic_blues", // Blues majeure
            "blues_scale",      // Gamme blues
            "dorian",          // Mode dorien
            "phrygian",        // Mode phrygien
            "lydian",          // Mode lydien
            "mixolydian",      // Mode mixolydien
            "locrian",         // Mode locrien
            "wholeTone",       // Gamme par tons entiers
            "chromatic",        // Gamme chromatique
            "aegean",          // Majeure harmonique
            "egyptian",        // Gamme égyptienne
            "japanese_hirajoshi", // Gamme japonaise
            "japanese_insen",   // Gamme japonaise
            "jewish_heptahard", // Freygish
            "diminished_wholetone", // Diminuée ton / demi-ton
            "diminished_tonehalfTone", // Diminuée demi-ton / ton
            "harmonic_minor_arabic", // Majeure arabe
            "bhairav"          // Gamme indienne
        };
        
        // Vérifier que toutes les gammes existent et ajouter celles qui manquent
        std::vector<std::string> result;
        for (const auto& scaleName : orderedScales) {
            if (database.find(scaleName) != database.end()) {
                result.push_back(scaleName);
            }
        }
        
        // Ajouter les gammes qui n'étaient pas dans la liste ordonnée
        for (const auto& [name, _] : database) {
            if (std::find(result.begin(), result.end(), name) == result.end()) {
                result.push_back(name);
            }
        }
        
        return result;
    }

};

// ==================== BASE DE DONNÉES ====================

const std::map<std::string, ScaleEntry> ScalesDatabase::database = {
    // Modes majeurs (gamme majeure et ses modes)
    {"major", {"Majeure (Ionien)", "1,2,3,4,5,6,7", "Gamme majeure naturelle", {"ionien", "ionian"}}},
    {"dorian", {"Dorien", "1,2,3-,4,5,6,7-", "Mode dorien (ii de la gamme majeure)", {"dorien"}}},
    {"phrygian", {"Phrygien", "1,2-,3-,4,5,6-,7-", "Mode phrygien (iii de la gamme majeure)", {"phrygien"}}},
    {"lydian", {"Lydien", "1,2,3,4+,5,6,7", "Mode lydien (iv de la gamme majeure)", {"lydien"}}},
    {"mixolydian", {"Mixolydien", "1,2,3,4,5,6,7-", "Mode mixolydien (v de la gamme majeure)", {"mixolydien"}}},
    {"minor_natural", {"Mineure naturelle (Éolien)", "1,2,3-,4,5,6-,7-", "Mode éolien (vi de la gamme majeure)", {"aeolian", "minor_natural", "aeolien"}}},
    {"locrian", {"Locrien", "1,2-,3-,4,5-,6-,7-", "Mode locrien (vii de la gamme majeure)", {"locrien"}}},

    // Gammes mineures
    {"minor_harmonic", {"Mineure harmonique", "1,2,3-,4,5,6-,7", "Mineure avec 7ème majeure", {"harmonic_minor"}}},
    {"minor_melodic", {"Mineure mélodique", "1,2,3-,4,5,6,7", "Mineure avec 6ème et 7ème majeure", {"melodic_minor"}}},

    // Gammes pentatoniques
    {"pentatonic_major", {"Pentatonique majeure", "1,2,3,5,6", "5 notes, sons majeurs", {"major_pentatonic", "pentatonique"}}},
    {"pentatonic_minor", {"Pentatonique mineure", "1,3-,4,5,7-", "5 notes, sons mineurs", {"minor_pentatonic"}}},
    {"pentatonic_blues", {"Blues majeure", "1,2,3-,3,5,6", "Pentatonique majeure avec la tierce mineure (note bleue)", {"major_blues", "blues_majeur"}}},

    // Gammes exotiques et non-occidentales
    {"blues_scale", {"Blues (hexatonique)", "1,3-,4,4+,5,7-", "Pentatonique mineure avec la quarte augmentée (note bleue)", {"blues", "blues_hexatonic"}}},
    {"wholeTone", {"Ton entier (Whole tone)", "1,2,3,4+,5+,6+", "Tous les intervalles d'un ton", {"whole_tone"}}},
    {"chromatic", {"Chromatique", "1,1+,2,2+,3,4,4+,5,5+,6,6+,7", "Les 12 notes chromatiques", {"chromatic", "chromatic_scale"}}},

    // Modes grecs spécifiques
    {"aegean", {"Majeure harmonique", "1,2,3,4,5,6-,7", "Majeure avec 6ème mineure (miroir de la mineure harmonique)", {"harmonic_major", "aegean"}}},
    {"egyptian", {"Égyptienne", "1,2,4,5,7-", "Pentatonique suspendue (sans tierce)", {"egyptian", "suspended_pentatonic"}}},
    {"japanese_hirajoshi", {"Hirajoshi (Japonaise)", "1,2,3-,5,6-", "Gamme traditionnelle japonaise", {"hirajoshi"}}},
    {"japanese_insen", {"Insen (Japonaise)", "1,2-,4,5,7-", "Gamme pentatonique japonaise", {"insen"}}},
    {"jewish_heptahard", {"Freygish (Ahava Rabbah)", "1,2-,3,4,5,6-,7-", "Gamme juive / klezmer (phrygien dominant, 5e mode de la mineure harmonique)", {"freygish", "phrygian_dominant", "jewish"}}},

    // Gammes symétriques
    {"diminished_wholetone", {"Diminuée (ton / demi-ton)", "1,2,3-,4,5-,6-,6,7", "Octatonique : alternance ton, demi-ton", {"whole_half_diminished", "diminished_whole_tone"}}},
    {"diminished_tonehalfTone", {"Diminuée (demi-ton / ton)", "1,2-,3-,3,4+,5,6,7-", "Octatonique : alternance demi-ton, ton (accords 7b9)", {"half_whole_diminished", "diminished_tone_half_tone"}}},

    // Gammes arabes/moyen-orient
    {"harmonic_minor_arabic", {"Majeure arabe (locrien majeur)", "1,2,3,4,5-,6-,7-", "Majeure avec quinte, sixte et septième abaissées", {"arabian", "major_locrian", "arabic"}}},

    // Gammes hindoues
    {"bhairav", {"Bhairav (Indienne)", "1,2-,3,4,5,6-,7", "Mode traditionnel de la musique classique indienne", {"bhairav"}}},
};
