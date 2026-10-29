#pragma once

/**
 * @file Tuning.hpp
 * @brief Accordage d'une guitare et conversions de hauteurs (sans affichage).
 *
 *   pc   — classe de hauteur 0–11 (C = 0)
 *   midi — hauteur absolue MIDI (E2 = 40), utile pour la synthèse sonore
 */

#include <cctype>
#include <string>
#include <vector>

struct Tuning {
    std::string              name;
    std::vector<std::string> strings; // [0]=corde grave, [5]=corde aiguë
    std::vector<int>         midi;    // hauteur MIDI de chaque corde à vide

    // Convertit un nom de note ("C", "F#", "Bb", "E#", "Bbb", "e" …) en classe
    // de hauteur 0–11. Accepte la première lettre en minuscule. Retourne -1 si invalide.
    static int noteToPC(const std::string &note) {
        static const int LETTER_PC[7] = {9, 11, 0, 2, 4, 5, 7}; // A B C D E F G
        if (note.empty()) return -1;
        char l = static_cast<char>(std::toupper(static_cast<unsigned char>(note[0])));
        if (l < 'A' || l > 'G') return -1;
        int pc = LETTER_PC[l - 'A'];
        for (size_t i = 1; i < note.size(); ++i) {
            if      (note[i] == '#') pc++;
            else if (note[i] == 'b') pc--;
            else return -1;
        }
        return ((pc % 12) + 12) % 12;
    }

    // Nom par défaut (dièses) d'une classe de hauteur.
    static const std::string &pcName(int pc) {
        static const std::string names[12] = {
            "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
        };
        return names[((pc % 12) + 12) % 12];
    }

    int stringCount()            const { return static_cast<int>(strings.size()); }
    int openPC(int s)            const { return noteToPC(strings[s]); }
    int noteAt(int s, int fret)  const { return (openPC(s) + fret) % 12; }
    int midiAt(int s, int fret)  const { return midi[s] + fret; }

    static Tuning standard() { return {"Standard (EADGBe)", {"E","A","D","G","B","e"}, {40,45,50,55,59,64}}; }
    static Tuning dropD()    { return {"Drop D (DADGBe)",   {"D","A","D","G","B","e"}, {38,45,50,55,59,64}}; }
    static Tuning openG()    { return {"Open G (DGDGBd)",   {"D","G","D","G","B","d"}, {38,43,50,55,59,62}}; }
    static Tuning openD()    { return {"Open D (DADF#Ad)",  {"D","A","D","F#","A","d"}, {38,45,50,54,57,62}}; }
    static Tuning openE()    { return {"Open E (EBEG#Be)",  {"E","B","E","G#","B","e"}, {40,47,52,56,59,64}}; }
    static Tuning dadgad()   { return {"DADGAD",             {"D","A","D","G","A","d"}, {38,45,50,55,57,62}}; }

};
