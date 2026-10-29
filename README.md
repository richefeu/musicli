<p align="center">
<img src="./logo-musicli.png" width="60%"/>
</p>


# 🎵 Outil de Théorie Musicale en C++

Un set d'outils pour explorer la théorie musicale : gammes, accords, et tonalités avec gestion correcte des enharmoniques (C# vs Db).

## 🚀 Fonctionnalités

### ScaleStructure
- **Gammes musicales** : 27 gammes différentes préconfigurées
  - Modes majeurs (Ionien, Dorien, Phrygien, Lydien, Mixolydien, Éolien, Locrien)
  - Modes mineurs (mineure naturelle, harmonique, mélodique)
  - Pentatoniques (majeure, mineure, blues)
  - Exotiques (japonaise, arabe, hindoue, etc.)
  - Ton entier, chromatique, etc.
- **Gestion des enharmoniques** : C# ≠ Db musicalement
- **Construction dynamique** : créer des gammes avec formules personnalisées
- **Formules intuitives** : `"1,2,3,4,5,6,7"` = gamme majeure

### ChordStructure
- **Accords** : 37 accords préconfigurés
  - Triades (maj, min, dim, aug)
  - Septièmes (7, maj7, m7, m7b5, dim7)
  - Neuvièmes, onzièmes, treizièmes
  - Sus chords (sus2, sus4)
  - Add chords (add9, add11)
  - Accords jazz (7#9, 7b9, min-maj7, etc.)
- **Enharmoniques correctes** : C7 = C E G Bb (pas A#)
- **Formules** : `"3,5"` = accord majeur

### Bases de données
- **ScalesDatabase** : Accès facile aux 27 gammes préconfigurées
- **ChordsDatabase** : Accès facile aux 37 accords préconfigurés
- API simple : `ScalesDatabase::getFormula("major")`

## 📋 Utilisation

### Compiler et tester
```bash
make test              # Tous les tests
make test_scale        # Tests des gammes
make test_chord        # Tests des accords
make test_databases    # Tests des bases de données
make clean             # Nettoyer
```

### Exemple de code

```cpp
#include "ScalesDatabase.hpp"
#include "ChordsDatabase.hpp"
#include "ScaleStructure.hpp"
#include "ChordStructure.hpp"

// Créer une gamme majeure en C
ScaleStructure scale(ScalesDatabase::getFormula("major"));
auto notes = scale.getNotes("C");
// Résultat: C D E F G A B

// Créer un accord dominant 7 en C
ChordStructure chord(ChordsDatabase::getFormula("dominant7"));
auto chord_notes = chord.getChordNotes("C");
// Résultat: C E G Bb

// Avec différentes tonalités
scale.getNotes("C#");  // C# D# E# F# G# A# B#
scale.getNotes("Db");  // Db Eb F Gb Ab Bb C

chord.getChordNotes("F");  // F A C Eb
```

## 🎼 Formules disponibles

### Symboles
- `1-7` : degrés musicaux (I-VII)
- `-` : baisse d'un demi-ton
- `+` : augmente d'un demi-ton
- `,` : séparateur

### Exemples
| Nom | Formule | Notes en C |
|-----|---------|-----------|
| Majeure | `1,2,3,4,5,6,7` | C D E F G A B |
| Mineure naturelle | `1,2,3-,4,5,6-,7-` | C D Eb F G Ab Bb |
| Mineure harmonique | `1,2,3-,4,5,6-,7` | C D Eb F G Ab B |
| Pentatonique blues | `1,3-,4,4+,5,7-` | C Eb F F# G Bb |
| Accord majeur | `3,5` | C E G |
| Accord dominant 7 | `3,5,7-` | C E G Bb |
| Accord majeur 9 | `3,5,7,2` | C E G B D |

## 📂 Structure du projet

```
MaMusique/
├── ScaleStructure.hpp          # Classe des gammes
├── ScalesDatabase.hpp          # Base de données de gammes (27 entrées)
├── ChordStructure.hpp          # Classe des accords
├── ChordsDatabase.hpp          # Base de données d'accords (37 entrées)
├── Tuning.hpp                  # Accordages, hauteurs (classe de hauteur, MIDI)
├── Fretboard.hpp               # Notes d'une gamme/accord sur le manche (grille de données)
├── ChordDiagrams.hpp           # Recherche de positions d'accords
├── TextRenderer.hpp            # Rendu terminal (ANSI) du manche et des diagrammes
├── Terminal.hpp, NcursesMenu.hpp, Application.hpp, main.cpp  # Interface CLI
├── test_scale.cpp              # Tests des gammes (8 cas)
├── test_chord.cpp              # Tests des accords (10 cas)
├── test_fretboard.cpp          # Tests du cœur manche/diagrammes (sans affichage)
├── test_databases.cpp          # Démonstration des bases de données
├── Makefile                    # Build et tests
├── TODO.md                     # Tâches futures
└── README.md                   # Ce fichier
```

Le **cœur** (`*Structure`, `*Database`, `Tuning`, `Fretboard`, `ChordDiagrams`) ne fait
aucun affichage : il renvoie des données. Seuls `TextRenderer`, `Terminal` et
`Application` écrivent dans le terminal, ce qui permet d'ajouter une autre interface
(graphique, web) sans toucher au calcul.

## 🌐 Interface web

```bash
make open     # compile le moteur en WebAssembly et ouvre web/index.html
```

Aucun serveur : la page s'ouvre directement depuis le disque (`file://`), le moteur
WebAssembly étant embarqué dans `web/musicli.js`. Nécessite Emscripten
(`brew install emscripten`).

- Gammes et accords, 12 fondamentales (♯ ou ♭), 6 accordages, 12 à 24 cases
- Manche coloré par fonction (fondamentale, tierce, quinte, septième), notes ou degrés
- Positions d'accords ; un clic sur un diagramme le joue et le montre sur le manche
- Son de corde pincée (Karplus-Strong) : clic sur une case, une note, ou **Espace**
- Clavier : ↑/↓ type suivant/précédent, ←/→ fondamentale, Échap désélectionne

## ✅ Points forts

- ✅ **Musicalement correct** : gestion des enharmoniques (C# ≠ Db)
- ✅ **Bien testé** : 28 tests unitaires
- ✅ **Base de données riche** : 27 gammes + 37 accords
- ✅ **API simple** : formules intuitives et faciles à retenir
- ✅ **Extensible** : ajouter facilement de nouvelles gammes/accords
- ✅ **Sans dépendances** : C++ pur (C++17)

## 🎯 Prochaines étapes (TODO.md)

- Voicings : positions variées d'accords
- Renversements : inversions d'accords
- Progressions d'accords : prédéfinies (I-IV-V, ii-V-I, etc.)
- Recherche par sonorité : trouver les accords par leurs caractéristiques
- Support JSON : exporter/importer les progressions

## 🔧 Build

Requis:
- C++17 (clang++ ou g++)
- Make

```bash
make test_scale        # Compile et teste
```

## 📜 Licence

Libre d'utilisation.

---

**Auteur** : Vincent Richefeu  
**Dernière mise à jour** : 2026-05-25
