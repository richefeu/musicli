<p align="center">
<img src="./logo-musicli.png" width="60%"/>
</p>


# 🎵 MusiCLI — Théorie musicale pour guitaristes

Un moteur de théorie musicale en C++ (gammes, accords, manche, positions d'accords)
avec deux interfaces : une **application web locale** (WebAssembly, avec le son) et
une **application terminal**. L'orthographe des notes est théoriquement stricte :
chaque degré garde sa lettre (C# ≠ Db, tierce de C# = E#, quinte augmentée de B = F##).

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
- Réglages mémorisés d'une ouverture à l'autre

## 💻 Application terminal

```bash
make run      # compile et lance ./musicli
```

Menus navigables aux flèches (ncurses) : accord sur le manche, gamme sur le manche
(par noms et par degrés), diagrammes d'accords par accordage, manche complet.
Voir [CLI.md](CLI.md).

## 🎼 Contenu musical

- **24 gammes** : modes de la gamme majeure, mineures naturelle / harmonique /
  mélodique, majeure harmonique, pentatoniques, blues majeure et hexatonique,
  ton entier, chromatique, diminuées octatoniques, Hirajoshi, Insen, égyptienne,
  Freygish, majeure arabe, Bhairav
- **37 accords** : triades, sus, septièmes (7, maj7, m7, m7b5, dim7, mM7…),
  9 / 11 / 13, altérés (7#5, 7b5, 7b9, 7#9), add9 / add11, power chord, quartal, quintal
- **6 accordages** : Standard, Drop D, Open G, Open D, Open E, DADGAD

### Formules

Une gamme ou un accord est décrit par ses degrés, séparés par des virgules :

- `1`–`7` : degrés ; pour les accords, `2`, `4`, `6` valent 9, 11, 13
- `-` / `+` : baisse / hausse d'un demi-ton, cumulables (`7--` = septième diminuée)
- la fondamentale `1` est ajoutée si elle est absente

| Nom | Formule | Notes en C |
|-----|---------|-----------|
| Majeure | `1,2,3,4,5,6,7` | C D E F G A B |
| Mineure harmonique | `1,2,3-,4,5,6-,7` | C D Eb F G Ab B |
| Blues | `1,3-,4,4+,5,7-` | C Eb F F# G Bb |
| Accord majeur | `3,5` | C E G |
| Dominante 7 | `3,5,7-` | C E G Bb |
| Diminué 7 | `3-,5-,7--` | C Eb Gb Bbb |
| Majeur 9 | `3,5,7,2` | C E G B D |

### Exemple de code

```cpp
#include "ScalesDatabase.hpp"
#include "ChordsDatabase.hpp"
#include "ScaleStructure.hpp"
#include "ChordStructure.hpp"
#include "Fretboard.hpp"
#include "ChordDiagrams.hpp"

ScaleStructure scale(ScalesDatabase::getFormula("major"));
scale.getNotes("C#");          // C# D# E# F# G# A# B#
scale.getDegreeLabels();       // 1 2 3 4 5 6 7

ChordStructure chord(ChordsDatabase::getFormula("dominant7"));
chord.getChordNotes("F");      // F A C Eb

// Notes sur le manche : grille [corde][case] (nom, degré, fondamentale, MIDI)
Fretboard fb(Tuning::standard(), 12);
FretGrid grid = fb.map("C#", scale.getNotes("C#"), scale.getDegreeLabels());

// Positions jouables d'un accord
DiagramsDatabase db;
auto diagrams = db.find("Am7", Tuning::dropD(), 3);
diagrams[0].midiNotes();       // hauteurs à jouer, du grave à l'aigu
```

## 📂 Structure du projet

```
MusiCLI/
├── ScaleStructure.hpp, ChordStructure.hpp   # Gammes et accords (formules → notes)
├── ScalesDatabase.hpp, ChordsDatabase.hpp   # 24 gammes, 37 accords
├── Tuning.hpp                               # Accordages, classes de hauteur, MIDI
├── Fretboard.hpp                            # Notes d'un ensemble sur le manche
├── ChordDiagrams.hpp                        # Recherche de positions d'accords
├── TextRenderer.hpp                         # Rendu terminal (ANSI)
├── Terminal.hpp, NcursesMenu.hpp,
│   Application.hpp, main.cpp                # Application terminal
├── web/                                     # Interface web (voir FILES.md)
├── test_*.cpp                               # Tests C++
├── Makefile
└── *.md                                     # Documentation
```

Le **cœur** (`*Structure`, `*Database`, `Tuning`, `Fretboard`, `ChordDiagrams`) ne fait
aucun affichage : il renvoie des données. Seuls `TextRenderer`, `Terminal` et
`Application` écrivent dans le terminal, et `web/` dessine les mêmes données dans le
navigateur. Détails dans [FILES.md](FILES.md).

## 🔧 Build et tests

Requis : C++17 (clang++ ou g++), Make, ncurses. Pour le web : Emscripten et Node.

```bash
make app          # application terminal
make wasm         # module WebAssembly (web/musicli.js)
make test         # tests C++ : gammes, accords, cœur, diagrammes, bases
make test_wasm    # tests du module WebAssembly sous Node
make help         # toutes les cibles
```

| Suite | Contenu |
|-------|---------|
| `test_scale` | 18 cas d'orthographe de gammes (dont tonalités théoriques) |
| `test_chord` | 17 cas d'accords (notes et reconnaissance du type) |
| `test_fretboard` | 32 tests du cœur : accordages, manche, diagrammes, validation des formules, balayage des bases sur 17 fondamentales |
| `test_wasm` | 9 tests du module JavaScript |
| `test_chord_diagram`, `test_databases` | démonstrations affichées |

## 🎯 Prochaines étapes

Voir [TODO.md](TODO.md) : renversements, accords d'une tonalité, progressions,
clavier de piano, reconnaissance d'accords…

## 📜 Licence

Libre d'utilisation.

---

**Auteur** : Vincent Richefeu  
**Dernière mise à jour** : 2026-10-29
