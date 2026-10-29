# 📂 Structure et description des fichiers

Le projet est organisé en trois couches : un **cœur** de calcul sans affichage, une
**application terminal**, et une **interface web** qui utilise le cœur compilé en
WebAssembly.

```
            ┌──────────── cœur (aucun affichage) ────────────┐
            │ ScaleStructure  ChordStructure                  │
            │ ScalesDatabase  ChordsDatabase                  │
            │ Tuning  Fretboard  ChordDiagrams                │
            └──────────────┬──────────────────┬───────────────┘
                           │                  │
       TextRenderer + Terminal           web/bindings.cpp (embind)
       + Application + main.cpp          → web/musicli.js (WebAssembly)
                 │                                  │
           ./musicli (terminal)        web/index.html + app.js + audio.js
```

## 🎼 Cœur

### ScaleStructure.hpp
Gamme décrite par une formule de degrés (`"1,2,3-,4,5,6-,7-"`).
- `getNotes(root)` : notes en orthographe théorique stricte, une lettre par degré,
  doubles altérations comprises (D# majeur : D# E# F## G# A# B# C##)
- `getDegreeLabels()` : degrés dans le même ordre (`"1"`, `"3-"`, `"4+"`…)
- Valide les degrés (1–7, altérations `-`/`+` cumulables) et l'unicité des intervalles

### ChordStructure.hpp
Accord décrit par ses degrés (`"3,5,7-"`, la fondamentale est implicite).
- `getChordNotes(root)`, `getDegreeLabels()`, `getIntervalsInSemitones()`
- `getChordTypeName()` : reconnaît les types courants (`""`, `m`, `7`, `maj7`, `dim7`…)
- Accepte un degré seul (`"5"`), refuse les degrés hors 1–7 et les notes en double

### ScalesDatabase.hpp / ChordsDatabase.hpp
Catalogues de 24 gammes et 37 accords : nom, formule, description, alias.
- `getScale(key)` / `getChord(key)`, `getFormula(key)`
- `listScales()` / `listChords()` : clés dans un ordre de présentation

### Tuning.hpp
Accordage d'une guitare : noms des cordes et hauteur MIDI de chaque corde à vide.
- `noteToPC("F##")` : classe de hauteur 0–11, toutes altérations acceptées
- `noteAt(corde, case)`, `midiAt(corde, case)`
- `standard()`, `dropD()`, `openG()`, `openD()`, `openE()`, `dadgad()`, `presets()`

### Fretboard.hpp
Place un ensemble de notes sur le manche.
- `map(root, notes, degrees)` → `FretGrid` : grille `[corde][case]` de `FretNote`
  (classe de hauteur, MIDI, nom orthographié, degré, appartenance, fondamentale)
- `mapAll()` : toutes les notes ; `inlayDots(case)` : repères de touche

### ChordDiagrams.hpp
Recherche de positions d'accords jouables. Voir [CHORD_DIAGRAMS.md](CHORD_DIAGRAMS.md).
- `ChordPos`, `ChordDiagram`, `ChordFinder`, `DiagramsDatabase`

## 💻 Application terminal

| Fichier | Rôle |
|---------|------|
| `TextRenderer.hpp` | Rendu ANSI : liste de notes, manche, carte de gamme, diagramme vertical, légende |
| `Terminal.hpp` | Effacement, titres, messages colorés, sélection dans une liste |
| `NcursesMenu.hpp` | Menu déroulant navigable aux flèches |
| `Application.hpp` | Menus et enchaînement des vues |
| `main.cpp` | Point d'entrée |

Voir [CLI.md](CLI.md).

## 🌐 Interface web (`web/`)

| Fichier | Rôle |
|---------|------|
| `bindings.cpp` | Expose le cœur à JavaScript (embind) ; les exceptions C++ deviennent des `Error` |
| `musicli.js` | Module généré par `make wasm` (non versionné), wasm embarqué |
| `index.html`, `style.css` | Page et styles (thèmes clair et sombre), sans ressource externe |
| `app.js` | État, rendu SVG du manche et des diagrammes, clavier ; aucun calcul musical |
| `audio.js` | Synthèse de corde pincée (Karplus-Strong) avec Web Audio |
| `test_bindings.cjs` | Tests du module sous Node |

API exposée : `tunings()`, `scales()`, `chords()`, `scaleNotes(clé, fondamentale)`,
`chordNotes(clé, fondamentale)`, `fretboard(accordage, cases, fondamentale, notes, degrés)`,
`chordDiagrams(accordage, clé, fondamentale, n)`.

Pour rester utilisable en `file://`, la page charge des scripts classiques (pas de
modules ES) et aucune ressource réseau.

## 🧪 Tests

| Fichier | Contenu |
|---------|---------|
| `test_scale.cpp` | 18 cas d'orthographe de gammes |
| `test_chord.cpp` | 17 cas d'accords (notes et type) |
| `test_fretboard.cpp` | 32 tests du cœur, dont la validation des formules et le calcul de toutes les entrées des bases sur 17 fondamentales |
| `test_chord_diagram.cpp` | Démonstration : diagrammes affichés pour plusieurs accordages |
| `test_databases.cpp` | Démonstration : bases de données et progressions |
| `web/test_bindings.cjs` | 9 tests du module WebAssembly |

## ⚙️ Build

`Makefile` : `app`, `run`, `wasm`, `open`, `test`, `test_*`, `test_wasm`, `clean`, `help`.
`.gitignore` : binaires natifs et module WebAssembly générés.

## 📚 Documentation

| Fichier | Contenu |
|---------|---------|
| `README.md` | Présentation, utilisation, exemples |
| `FILES.md` | Ce fichier |
| `CLI.md` | Application terminal |
| `CHORD_DIAGRAMS.md` | Recherche de positions d'accords |
| `PROJECT_STATUS.md` | État du projet |
| `TODO.md` | Feuille de route |

## 📝 Conventions

- Classes : `PascalCase` ; fonctions : `camelCase` ; membres privés suffixés `_`
  dans les fichiers récents (`ChordDiagrams`, `Fretboard`)
- En-têtes seuls (`.hpp`), C++17, pas de dépendance hors ncurses
- Tests : `test_*.cpp`, un exécutable par fichier

---
**Dernière mise à jour** : 2026-10-29
