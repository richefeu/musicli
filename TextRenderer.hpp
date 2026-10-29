#pragma once

/**
 * @file TextRenderer.hpp
 * @brief Rendu texte (ANSI) des structures calculées par le cœur.
 *
 * Seul fichier, avec Terminal.hpp, à écrire dans un flux. Chaque fonction
 * reçoit des données déjà calculées (FretGrid, ChordDiagram) et les dessine ;
 * une autre interface (graphique, web) peut les dessiner à sa façon.
 */

#include "ChordDiagrams.hpp"
#include "Fretboard.hpp"
#include <iomanip>
#include <ostream>
#include <string>
#include <vector>

namespace TextRenderer {

namespace ansi {
inline const char *RST = "\033[0m";
inline const char *RED = "\033[91m";
inline const char *GRN = "\033[92m";
inline const char *BOLD_RED = "\033[1;31m";
inline const char *BOLD_GRN = "\033[1;32m";
inline const char *BRIGHT_GRN = "\033[1;92m";
inline const char *REV_RED = "\033[7;91m";
inline const char *DIM = "\033[2m";
} // namespace ansi

// En-tête "=== TITRE ===" suivi de la liste des notes.
inline void noteList(std::ostream &os, const std::string &title,
                     const std::vector<std::string> &notes) {
  os << "\n=== " << title << " ===\n";
  os << "Notes: ";
  for (const auto &n : notes) os << n << " ";
  os << "\n";
}

// Lignes "Fret: 0 1 2 …" et repères de touche, alignées sur des cellules de 5.
inline void fretHeader_(std::ostream &os, int maxFret) {
  os << "Fret:";
  for (int i = 0; i <= maxFret; ++i) os << std::setw(5) << std::left << i;
  os << "\n ";
  for (int i = 0; i <= maxFret; ++i) {
    switch (Fretboard::inlayDots(i)) {
    case 1:  os << "   •|"; break;
    case 2:  os << "  ••|"; break;
    default: os << "    |"; break;
    }
  }
  os << "\n";
}

// Manche complet : notes de l'ensemble en rouge (fondamentale en inverse),
// autres notes en vert.
inline void neck(std::ostream &os, const Fretboard &fb, const FretGrid &grid) {
  os << "\n";
  fretHeader_(os, fb.maxFret());
  for (int s = fb.tuning().stringCount() - 1; s >= 0; --s) {
    os << "  " << std::left << std::setw(3) << fb.tuning().strings[s];
    for (const FretNote &n : grid[s]) {
      if (n.isRoot) {
        os << ansi::REV_RED << n.name << ansi::RST
           << std::string(5 - n.name.size(), ' ');
      } else if (n.inSet) {
        os << ansi::RED << std::setw(5) << n.name << ansi::RST;
      } else {
        os << ansi::BRIGHT_GRN << std::setw(5) << n.name << ansi::RST;
      }
    }
    os << "\n";
  }
  os << "\n";
}

// Carte d'une gamme : seules les notes de l'ensemble sont affichées,
// par nom ou par degré (fondamentale = R).
inline void scaleMap(std::ostream &os, const Fretboard &fb,
                     const FretGrid &grid, const std::string &title,
                     bool showDegrees) {
  os << "\n--- SCALE MAP " << title << "("
     << (showDegrees ? "degrés" : "nom des notes") << ") ---\n";
  fretHeader_(os, fb.maxFret());
  for (int s = fb.tuning().stringCount() - 1; s >= 0; --s) {
    os << "  " << std::left << std::setw(3) << fb.tuning().strings[s];
    for (const FretNote &n : grid[s]) {
      if (!n.inSet) {
        os << std::setw(5) << ".";
        continue;
      }
      std::string label = !showDegrees ? n.name : n.isRoot ? "R" : n.degree;
      os << (n.isRoot ? ansi::RED : ansi::GRN) << std::setw(5) << label
         << ansi::RST;
    }
    os << "\n";
  }
  os << "\n";
}

// Contenu centré dans une cellule de 3 caractères.
inline std::string cell3_(const std::string &s) {
  if (s.size() == 1) return " " + s + " ";
  return s.size() == 2 ? s + " " : s;
}

// Diagramme d'accord vertical :
//   • les cordes sont des lignes verticales │ continues,
//   • les frettes sont des traits horizontaux ─┼─ qui les traversent,
//   • une note pressée remplace le │ par son nom (rouge = fondamentale, vert = autre),
//   • une corde étouffée est affichée en mode atténué.
inline void chordDiagram(std::ostream &os, const ChordDiagram &d,
                         const std::string &title = "") {
  const ChordPos &pos = d.pos();
  const std::vector<std::string> &STR = d.tuning().strings;
  const int nStr = d.tuning().stringCount();
  const std::string &t = title.empty() ? d.chordName() : title;
  if (!t.empty()) os << "\n  " << t << "\n";

  int startFret = d.startFret();

  // Chaque corde : 3 chars. Préfixe : 4 chars (pour le label "XXfr").
  const std::string PFX = "    ";

  // Noms des cordes (haut)
  os << PFX;
  for (int s = 0; s < nStr; ++s) os << cell3_(STR[s]);
  os << "\n";

  // Marqueurs x (étouffée, atténué) / o (à vide) / espace
  os << PFX;
  for (int s = 0; s < nStr; ++s) {
    if      (pos.frets[s] <  0) os << ansi::DIM << " x " << ansi::RST;
    else if (pos.frets[s] == 0) os << " o ";
    else                        os << "   ";
  }
  os << "\n";

  // Sillet : label de position ("XXfr") ou espaces si on est en case 1
  if (startFret > 1) os << std::right << std::setw(2) << startFret << "fr";
  else               os << PFX;
  for (int i = 0; i < 3 * nStr; ++i) os << "═";
  os << "\n";

  // Slots de cases
  for (int row = 0; row < ChordDiagram::ROWS; ++row) {
    int fret = startFret + row;

    // Cordes + notes
    os << PFX;
    for (int s = 0; s < nStr; ++s) {
      if (pos.frets[s] < 0) {
        os << ansi::DIM << " │ " << ansi::RST;
      } else if (pos.frets[s] == fret) {
        os << (d.isRoot(s) ? ansi::BOLD_RED : ansi::BOLD_GRN)
           << cell3_(d.noteName(s)) << ansi::RST;
      } else {
        os << " │ ";
      }
    }
    os << " " << fret << "\n";

    // Frette (┼ atténué sur cordes étouffées)
    os << PFX;
    for (int s = 0; s < nStr; ++s) {
      if (pos.frets[s] < 0) os << ansi::DIM << "─┼─" << ansi::RST;
      else                  os << "─┼─";
    }
    os << "\n";
  }

  // Noms des cordes (bas)
  os << PFX;
  for (int s = 0; s < nStr; ++s) os << cell3_(STR[s]);
  os << "\n";
}

inline void diagramLegend(std::ostream &os) {
  os << "\n  " << ansi::BOLD_RED << "XX" << ansi::RST << " = fondamentale   "
     << ansi::BOLD_GRN << "XX" << ansi::RST << " = autre note   "
     << ansi::DIM << "x" << ansi::RST << " = étouffée   o = à vide\n";
}

} // namespace TextRenderer
