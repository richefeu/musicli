#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <iostream>
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
            "pentatonic_blues", // Pentatonique blues
            "blues_scale",      // Gamme blues
            "dorian",          // Mode dorien
            "phrygian",        // Mode phrygien
            "lydian",          // Mode lydien
            "mixolydian",      // Mode mixolydien
            "locrian",         // Mode locrien
            "wholeTone",       // Gamme par tons entiers
            "chromatic",        // Gamme chromatique
            "aegean",          // Gamme égéenne
            "egyptian",        // Gamme égyptienne
            "japanese_hirajoshi", // Gamme japonaise
            "japanese_insen",   // Gamme japonaise
            "jewish_heptahard", // Gamme juive
            "diminished_wholetone", // Gamme diminuée-ton entier
            "diminished_tonehalfTone", // Gamme diminuée-demi-ton entier
            "harmonic_minor_arabic", // Gamme arabe
            "bhairav",         // Gamme indienne
            "khamaraj",        // Gamme indienne
            "sus2_scale",      // Gamme sus2
            "sus4_scale"       // Gamme sus4
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

    // Afficher la base de données
    static void printDatabase() {
        for (const auto& [name, entry] : database) {
            std::cout << name << " : " << entry.formula << " - " << entry.description << std::endl;
        }
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
    {"pentatonic_blues", {"Pentatonique blues", "1,3-,4,4+,5,7-", "Pentatonique mineure avec la note bleue (F#/Gb)", {"blues"}}},

    // Gammes exotiques et non-occidentales
    {"blues_scale", {"Blues (hexatonique)", "1,3-,4,4+,5,7-", "La gamme bleue complète", {"blues_hexatonic"}}},
    {"wholeTone", {"Ton entier (Whole tone)", "1,2,3,4+,5+,6+", "Tous les intervalles d'un ton", {"whole_tone"}}},
    {"chromatic", {"Chromatique", "1,1+,2,2+,3,4,4+,5,5+,6,6+,7", "Les 12 notes chromatiques", {"chromatic", "chromatic_scale"}}},

    // Modes grecs spécifiques
    {"aegean", {"Égéen", "1,2,3,4,5,6-,7", "Intervalle caractéristique : 3ème majeure puis mineure", {"aegean"}}},
    {"egyptian", {"Égyptienne", "1,3-,4,5,7-", "Pentatonique ancienne égyptienne", {"egyptian"}}},
    {"japanese_hirajoshi", {"Hirajoshi (Japonaise)", "1,2,3-,5-,6-", "Gamme traditionnelle japonaise", {"hirajoshi"}}},
    {"japanese_insen", {"Insen (Japonaise)", "1,2-,4,5,7-", "Gamme pentatonique japonaise", {"insen"}}},
    {"jewish_heptahard", {"Heptahard (Juive)", "1,2,3,4,5,6,7-", "Gamme traditionnelle juive", {"jewish"}}},

    // Gammes symétriques
    {"diminished_wholetone", {"Diminuée-Ton entier", "1,2,3-,4,5-,6-,7", "Alternance tons/demi-tons", {"diminished_whole_tone"}}},
    {"diminished_tonehalfTone", {"Diminuée-Demi-ton entier", "1,2-,3,4,5-,6,7-", "Alternance demi-tons/tons", {"diminished_tone_half_tone"}}},

    // Gammes arabes/moyen-orient
    {"harmonic_minor_arabic", {"Mineure harmonique (arabe)", "1,2,3-,4,5,6-,7", "Avec la tierce mineure et septième majeure", {"arabic"}}},

    // Gammes hindoues
    {"bhairav", {"Bhairav (Indienne)", "1,2-,3,4,5,6-,7", "Mode traditionnel de la musique classique indienne", {"bhairav"}}},
    {"khamaraj", {"Khamaraj (Indienne)", "1,2-,3,4,5-,6,7-", "Autre mode indien", {"khamaraj"}}},

    // Autres
    {"sus2_scale", {"Sus2 (mineur)", "1,2,3-,4,5", "Pour construire des accords sus", {"sus2"}}},
    {"sus4_scale", {"Sus4 (mineur)", "1,4,5,6-", "Pour construire des accords sus", {"sus4"}}},
};
