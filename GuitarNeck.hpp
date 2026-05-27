#pragma once

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ChordDiagrams.hpp"

class GuitarNeck {
private:
  std::vector<std::string> notes;
  std::vector<int> tuning;
  std::vector<std::string> stringLabels;
  int maxFret;
  std::unordered_map<std::string, std::vector<int>> chordFormulas;

  int noteToIndex(const std::string &note) const {
    static const std::unordered_map<std::string, int> noteMap = {
        {"E", 0},  {"F", 1},  {"F#", 2}, {"Gb", 2},  {"G", 3},  {"G#", 4},
        {"Ab", 4}, {"A", 5},  {"A#", 6}, {"Bb", 6},  {"B", 7},  {"C", 8},
        {"C#", 9}, {"Db", 9}, {"D", 10}, {"D#", 11}, {"Eb", 11}};
    auto it = noteMap.find(note);
    return (it != noteMap.end()) ? it->second : -1;
  }

  std::vector<int> getChordIndices(const std::string &root,
                                   const std::vector<int> &intervals) const {
    std::vector<int> result;
    int rootIndex = noteToIndex(root);
    if (rootIndex == -1)
      return result;

    for (int interval : intervals) {
      result.push_back((rootIndex + interval) % 12);
    }
    return result;
  }

public:
  GuitarNeck(const std::vector<std::string> &tuningNotes = {"E", "A", "D", "G",
                                                            "B", "E"},
             int maxFret = 12)
      : notes(
            {"E", "F", "F#", "G", "G#", "A", "A#", "B", "C", "C#", "D", "D#"}),
        stringLabels({"E", "A", "D", "G", "B", "e"}), maxFret(maxFret) {

    for (const auto &note : tuningNotes) {
      tuning.push_back(noteToIndex(note));
    }

    chordFormulas = {
        {"", {0, 4, 7}},         {"m", {0, 3, 7}},      {"-", {0, 3, 7}},
        {"7", {0, 4, 7, 10}},    {"m7", {0, 3, 7, 10}}, {"-", {0, 3, 7, 10}},
        {"maj7", {0, 4, 7, 11}}, {"Δ7", {0, 4, 7, 11}}, {"dim", {0, 3, 6}},
        {"dim7", {0, 3, 6, 9}},  {"aug", {0, 4, 8}},    {"sus4", {0, 5, 7}},
        {"sus2", {0, 2, 7}}};
  }

  void displayChordDiagram(const std::string &chordName,
                           const ChordStructure & /*chordStructure*/,
                           const std::string & /*root*/,
                           const ChordDiagram &diagram) const {
    std::cout << "\n=== CHORD DIAGRAM: " << chordName << " ===\n";
    std::cout << "Position: " << diagram.name << "\n";
    std::cout << "Fundamental on string: " << stringLabels[diagram.rootString]
              << " (fret " << diagram.rootFret << ")\n";
    std::cout << "Difficulty: " << diagram.difficulty << "/10\n\n";

    // Ligne de repères pour les frettes (marqueur à la 1ère frette)
    std::cout << "    ";
    for (int i = 1; i <= 4; ++i) {
      if (i == 1) {
        std::cout << "  •  ";
      } else {
        std::cout << "     ";
      }
    }
    std::cout << "\n";

    // En-tête avec les noms des cordes (EADGBE - de grave à aigu)
    std::cout << "    ";
    for (int i = 0; i <= 5; ++i) {
      std::cout << std::setw(5) << stringLabels[i];
    }
    std::cout << "\n";

    // Affichage des 4 premières frettes
    int startFret = 1;
    for (int string = 0; string <= 5; ++string) {
      if (diagram.frets[string] > 0) {
        startFret = diagram.frets[string];
        break;
      }
    }

    for (int fretOffset = 0; fretOffset < 4; ++fretOffset) {
      int actualFret = startFret + fretOffset;
      std::cout << std::setw(4) << actualFret << " ";

      for (int string = 0; string <= 5; ++string) {
        int fretPos = diagram.frets[string];

        if (fretPos >= 0 && fretPos == actualFret) {
          bool isRoot =
              (string == diagram.rootString && actualFret == diagram.rootFret);
          int finger = diagram.fingering[string];

          if (isRoot) {
            std::cout << "\033[1;91m  " << finger << "  \033[0m";
          } else {
            std::cout << "\033[1;92m  " << finger << "  \033[0m";
          }
        } else if (fretPos == 0) {
          std::cout << "  X  ";
        } else if (fretPos == -1 && actualFret == startFret) {
          std::cout << "  O  ";
        } else {
          std::cout << "  .  ";
        }
      }
      std::cout << "\n";
    }

    std::cout << "\nLégende: ";
    std::cout << "\033[1;91mX\033[0m = Fondamentale, ";
    std::cout << "\033[1;92mN\033[0m = Autres notes, ";
    std::cout << "X = Corde étouffée, O = Corde à vide\n";
    std::cout << "Doigté: 1=Index, 2=Majeur, 3=Annulaire, 4=Auriculaire\n";
    std::cout << "\n";
  }

  void showChord(const std::string &chordName,
                 const ChordStructure &chordStructure,
                 const std::string &root) {
    std::vector<std::string> chordNotes = chordStructure.getChordNotes(root);

    std::cout << "\n=== ACCORD: " << chordName << " ===\n";
    std::cout << "Notes: ";
    for (const auto &note : chordNotes) {
      std::cout << note << " ";
    }
    std::cout << "\n";

    std::unordered_set<int> highlightedNotes;
    int rootIndex = noteToIndex(root);
    for (const auto &note : chordNotes) {
      int idx = noteToIndex(note);
      if (idx >= 0) {
        highlightedNotes.insert(idx);
      }
    }

    displayNeck(highlightedNotes, rootIndex);
  }

  void showChord(const std::string &chordName) {
    if (chordName.empty()) {
      std::cerr << "Erreur : nom d'accord vide.\n";
      return;
    }

    std::string root;
    std::string type;
    if (chordName.size() >= 2 && (chordName[1] == '#' || chordName[1] == 'b')) {
      root = chordName.substr(0, 2);
      type = chordName.substr(2);
    } else {
      root = chordName.substr(0, 1);
      type = chordName.substr(1);
    }

    auto it = chordFormulas.find(type);
    if (it == chordFormulas.end() && type == "major") {
      it = chordFormulas.find("");
    }
    if (it == chordFormulas.end()) {
      std::cerr << "Erreur : type d'accord inconnu '" << type << "'.\n";
      return;
    }

    std::vector<int> chordIndices = getChordIndices(root, it->second);
    if (chordIndices.empty()) {
      std::cerr << "Erreur : fondamentale invalide '" << root << "'\n";
      return;
    }

    std::unordered_set<int> highlightedNotes(chordIndices.begin(),
                                             chordIndices.end());
    int rootIndex = noteToIndex(root);

    std::cout << "\n=== ACCORD: " << chordName << " ===\n";
    std::cout << "Notes: ";
    for (int idx : chordIndices) {
      std::cout << notes[idx] << " ";
    }
    std::cout << "\n";

    displayNeck(highlightedNotes, rootIndex);
  }

  void showScale(const std::string &scaleName, const std::string &root,
                 const std::vector<std::string> &scaleNotes) {
    std::cout << "\n=== SCALE: " << scaleName << " (" << root << ") ===\n";
    std::cout << "Notes: ";
    for (const auto &note : scaleNotes) {
      std::cout << note << " ";
    }
    std::cout << "\n";

    /*
    std::unordered_set<int> scaleIndices;
    int rootIndex = noteToIndex(root);
    for (const auto &note : scaleNotes) {
      int idx = noteToIndex(note);
      if (idx >= 0) {
        scaleIndices.insert(idx);
      }
    }

    
    displayNeck(scaleIndices, rootIndex);
*/
    
    displayScaleVisualization(scaleName, root, scaleNotes, false);
    displayScaleVisualization(scaleName, root, scaleNotes, true);
  }

  void showAllNotes() const {
    std::cout << "\n=== COMPLETE FRETBOARD ===\n";
    std::unordered_set<int> scaleIndices;

    for (int idx = 0; idx <= maxFret; idx++) {
      scaleIndices.insert(idx);
    }
    displayNeck(scaleIndices);
  }

  void setMaxFret(int frets) { maxFret = frets; }

  void setTuning(const std::vector<std::string> &tuningNotes) {
    tuning.clear();
    for (const auto &note : tuningNotes) {
      tuning.push_back(noteToIndex(note));
    }
  }

  void displayScaleVisualization(const std::string &scaleName,
                                 const std::string &root,
                                 const std::vector<std::string> &scaleNotes,
                                 bool showDegrees = true) const {
    std::cout << "\n--- SCALE MAP " << scaleName << "(";
    if (showDegrees)
      std::cout << "degrés";
    else
      std::cout << "nom des notes";
    std::cout << ") ---\n";

    // Ligne des numéros de frette avec alignement
    std::cout << "Fret:";
    for (int i = 0; i <= 12; ++i) {
      std::cout << std::setw(5) << std::left << i;
    }
    std::cout << "\n";

    // Ligne de repères pour les frettes
    std::cout << " ";
    for (int i = 0; i <= 12; ++i) {
      if (i == 5 || i == 7 || i == 9) {
        std::cout << "   •|";
      } else if (i == 12) {
        std::cout << "  ••|";
      } else {
        std::cout << "    |";
      }
    }
    std::cout << "\n";

    const char *colors[] = {
        "\033[0m",  // Normal
        "\033[92m", // Green
        "\033[91m"  // Red (root)
    };

    std::unordered_set<int> scaleIndices;
    std::unordered_map<int, std::string>
        degreeMap; // Note index -> degree symbol
    std::unordered_map<int, std::string>
        noteNameMap; // Note index -> correct note name
    int rootIndex = noteToIndex(root);

    // Créer une map des degrés et des noms de notes corrects
    for (size_t i = 0; i < scaleNotes.size(); ++i) {
      int noteIndex = noteToIndex(scaleNotes[i]);
      if (noteIndex == -1)
        continue;

      // Stocker le nom correct de la note pour cet index
      noteNameMap[noteIndex] = scaleNotes[i];

      // Calculer l'intervalle par rapport à la fondamentale
      int interval = (noteIndex - rootIndex + 12) % 12;

      // Déterminer le degré en fonction de l'intervalle
      std::string degreeSymbol;
      switch (interval) {
      case 0:
        degreeSymbol = "F";
        break; // Fondamentale
      case 1:
        degreeSymbol = "2-";
        break; // Seconde mineure
      case 2:
        degreeSymbol = "2";
        break; // Seconde majeure
      case 3:
        degreeSymbol = "3-";
        break; // Tierce mineure
      case 4:
        degreeSymbol = "3";
        break; // Tierce majeure
      case 5:
        degreeSymbol = "4";
        break; // Quarte juste
      case 6:
        degreeSymbol = "4+";
        break; // Quarte augmentée
      case 7:
        degreeSymbol = "5";
        break; // Quinte juste
      case 8:
        degreeSymbol = "6-";
        break; // Sixte mineure (ou quinte augmentée)
      case 9:
        degreeSymbol = "6";
        break; // Sixte majeure
      case 10:
        degreeSymbol = "7-";
        break; // Septième mineure
      case 11:
        degreeSymbol = "7";
        break; // Septième majeure
      default:
        degreeSymbol = "?";
        break;
      }

      // Pour les gammes à 7 notes, utiliser des symboles plus simples
      if (scaleNotes.size() == 7) {
        switch (i) {
        case 0:
          degreeSymbol = "R";
          break;
        case 1:
          degreeSymbol = "2";
          break;
        case 2:
          // Tierce: majeure (4) ou mineure (3)
          degreeSymbol = (interval == 4) ? "3" : "3-";
          break;
        case 3:
          degreeSymbol = "4";
          break;
        case 4:
          degreeSymbol = "5";
          break;
        case 5:
          // Sixte: majeure (9) ou mineure (8)
          degreeSymbol = (interval == 9) ? "6" : "6-";
          break;
        case 6:
          // Septième: majeure (11) ou mineure (10)
          degreeSymbol = (interval == 11) ? "7" : "7-";
          break;
        }
      }

      degreeMap[noteIndex] = degreeSymbol;
      scaleIndices.insert(noteIndex);
    }

    for (int string = 5; string >= 0; --string) {
      std::cout << "  " << std::left << std::setw(3) << stringLabels[string];
      for (int fret = 0; fret <= 12; ++fret) {
        int noteIndex = (tuning[string] + fret) % 12;

        if (noteIndex == rootIndex) {
          if (showDegrees) {
            std::cout << colors[2] << std::setw(5) << "R" << colors[0];
          } else {
            // Utiliser le nom correct de la note si disponible, sinon utiliser
            // le nom par défaut
            std::string noteName = noteNameMap.count(noteIndex)
                                       ? noteNameMap[noteIndex]
                                       : notes[noteIndex];
            std::cout << colors[2] << std::setw(5) << noteName << colors[0];
          }
        } else if (degreeMap.find(noteIndex) != degreeMap.end()) {
          if (showDegrees) {
            std::cout << colors[1] << std::setw(5) << degreeMap[noteIndex]
                      << colors[0];
          } else {
            // Utiliser le nom correct de la note si disponible, sinon utiliser
            // le nom par défaut
            std::string noteName = noteNameMap.count(noteIndex)
                                       ? noteNameMap[noteIndex]
                                       : notes[noteIndex];
            std::cout << colors[1] << std::setw(5) << noteName << colors[0];
          }
        } else {
          std::cout << std::setw(5) << ".";
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  void displayNeck(const std::unordered_set<int> &highlightedNotes,
                   int rootIndex = -1) const {
    std::cout << "\n";

    // Ligne des numéros de frette avec alignement
    std::cout << "Fret:";
    for (int i = 0; i <= maxFret; ++i) {
      std::cout << std::setw(5) << std::left << i;
    }
    std::cout << "\n";

    // Ligne de repères pour les frettes
    std::cout << " ";
    for (int i = 0; i <= maxFret; ++i) {
      if (i == 5 || i == 7 || i == 9 || i == 15 || i == 17) {
        std::cout << "   •|";
      } else if (i == 12) {
        std::cout << "  ••|";
      } else {
        std::cout << "    |";
      }
    }
    std::cout << "\n";

    for (int string = 5; string >= 0; --string) {
      std::cout << "  " << std::left << std::setw(3) << stringLabels[string];
      for (int fret = 0; fret <= maxFret; ++fret) {
        int noteIndex = (tuning[string] + fret) % maxFret;

        if (highlightedNotes.find(noteIndex) != highlightedNotes.end()) {
          // Notes mis en valeur
          if (rootIndex == noteIndex) {
            std::cout << "\033[7;91m" << notes[noteIndex] << "\033[0m"
                      << std::string(5 - notes[noteIndex].size(), ' ');
          } else {
            std::cout << "\033[91m" << std::setw(5) << notes[noteIndex]
                      << "\033[0m";
          }

        } else {
          // Autres notes en gris
          std::cout << "\033[1;92m" << std::setw(5) << notes[noteIndex]
                    << "\033[0m";
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
};
