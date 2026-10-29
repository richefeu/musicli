#pragma once

/**
 * @file Fretboard.hpp
 * @brief Cartographie des notes sur le manche (calcul pur, sans affichage).
 *
 * Fretboard::map() produit une grille [corde][case] de FretNote décrivant,
 * pour chaque position, la note jouée et son rôle dans un ensemble de notes
 * (gamme ou accord). L'affichage est laissé au renderer (voir TextRenderer.hpp).
 *
 * Exemple :
 *   Fretboard fb(Tuning::standard(), 12);
 *   auto grid = fb.map("A", {"A","C","E"}, {"1","3-","5"});
 *   grid[0][5].isRoot  // true : corde de Mi grave, case 5 = La
 */

#include "Tuning.hpp"
#include <string>
#include <vector>

struct FretNote {
    int         string = 0;  // 0 = corde grave
    int         fret   = 0;  // 0 = corde à vide
    int         pc     = 0;  // classe de hauteur 0–11
    int         midi   = 0;  // hauteur absolue
    std::string name;        // orthographe de l'ensemble si la note en fait partie
    std::string degree;      // degré dans l'ensemble ("1", "3-", …), vide sinon
    bool        inSet  = false;
    bool        isRoot = false;
};

using FretGrid = std::vector<std::vector<FretNote>>; // [corde][case]

class Fretboard {
public:
    explicit Fretboard(const Tuning &tuning = Tuning::standard(), int maxFret = 12)
        : tuning_(tuning), maxFret_(maxFret) {}

    const Tuning &tuning()  const { return tuning_; }
    int           maxFret() const { return maxFret_; }

    // Repères de touche : nombre de points incrustés sur la case (0, 1 ou 2).
    static int inlayDots(int fret) {
        int f = fret % 12;
        if (fret > 0 && f == 0) return 2;
        return (f == 3 || f == 5 || f == 7 || f == 9) ? 1 : 0;
    }

    // Place un ensemble de notes sur le manche.
    //   root    — fondamentale ("A", "Bb" …)
    //   notes   — noms des notes de l'ensemble, dans l'ordre de la formule
    //   degrees — degrés correspondants (optionnel ; déduits des intervalles sinon)
    FretGrid map(const std::string              &root,
                 const std::vector<std::string> &notes,
                 const std::vector<std::string> &degrees = {}) const {
        std::string name[12];
        std::string degree[12];
        bool        inSet[12] = {};
        int rootPC = Tuning::noteToPC(root);

        for (size_t i = 0; i < notes.size(); ++i) {
            int pc = Tuning::noteToPC(notes[i]);
            if (pc < 0 || inSet[pc]) continue;
            inSet[pc]  = true;
            name[pc]   = notes[i];
            degree[pc] = i < degrees.size() ? degrees[i]
                                            : intervalDegree((pc - rootPC + 12) % 12);
        }

        FretGrid grid(tuning_.stringCount(), std::vector<FretNote>(maxFret_ + 1));
        for (int s = 0; s < tuning_.stringCount(); ++s) {
            for (int f = 0; f <= maxFret_; ++f) {
                FretNote &n = grid[s][f];
                n.string = s;
                n.fret   = f;
                n.pc     = tuning_.noteAt(s, f);
                n.midi   = tuning_.midiAt(s, f);
                n.inSet  = inSet[n.pc];
                n.isRoot = n.inSet && n.pc == rootPC;
                n.name   = n.inSet ? name[n.pc] : Tuning::pcName(n.pc);
                n.degree = n.inSet ? degree[n.pc] : "";
            }
        }
        return grid;
    }

    // Toutes les notes du manche (chromatique, sans fondamentale).
    FretGrid mapAll() const {
        std::vector<std::string> all;
        for (int pc = 0; pc < 12; ++pc) all.push_back(Tuning::pcName(pc));
        FretGrid grid = map("", all);
        for (auto &row : grid)
            for (auto &n : row) n.degree.clear();
        return grid;
    }

    // Degré par défaut d'un intervalle en demi-tons par rapport à la fondamentale.
    static std::string intervalDegree(int semitones) {
        static const std::string D[12] = {
            "1","2-","2","3-","3","4","4+","5","6-","6","7-","7"
        };
        return D[((semitones % 12) + 12) % 12];
    }

private:
    Tuning tuning_;
    int    maxFret_;
};
