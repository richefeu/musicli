# 💻 MusiCLI — Application terminal

L'application terminal affiche gammes, accords et positions sur le manche avec des
couleurs ANSI. Elle utilise le même cœur que l'interface web (voir [FILES.md](FILES.md)).

## 🚀 Utilisation

```bash
make app      # compile ./musicli (nécessite ncurses)
make run      # compile et lance
```

Les listes se parcourent avec **↑/↓** et se valident avec **Entrée** ; les listes
longues défilent.

## 🎯 Menu principal

| Entrée | Étapes | Affichage |
|--------|--------|-----------|
| **Accord sur le manche** | type d'accord (37), fondamentale | notes de l'accord, manche 12 cases (notes de l'accord en rouge, fondamentale en inverse), 3 diagrammes en accordage standard |
| **Gamme sur le manche** | gamme (24), fondamentale | notes de la gamme, carte du manche par noms de notes puis par degrés (R = fondamentale) |
| **Diagrammes d'accords** | accordage (6), fondamentale, type d'accord (25 suffixes : `m`, `7`, `maj7`, `m7b5`…) | 3 positions avec la tablature `E:x A:3 …` |
| **Manche complet** | — | toutes les notes des 12 premières cases |
| **Quitter** | | |

Les fondamentales proposées sont les 12 notes en dièses (C, C#, D…).

### Diagrammes

```
  Am  — position 1  —  E:x A:o D:2 G:2 B:1 e:o
     E  A  D  G  B  e
     x  o           o
    ══════════════════
     │  │  │  │  C  │  1
    ─┼──┼──┼──┼──┼──┼─
     │  │  E  A  │  │  2
    ─┼──┼──┼──┼──┼──┼─
```

- note en **rouge** = fondamentale, en **vert** = autre note de l'accord
- `x` = corde étouffée (atténuée), `o` = corde à vide
- `5fr` à gauche du sillet = première case dessinée quand la position est haute

## 🏗️ Architecture

```
main.cpp
   └── Application.hpp        menus et enchaînement des vues
         ├── Terminal.hpp      effacement, titres, messages, sélection (NcursesMenu.hpp)
         ├── TextRenderer.hpp  dessin ANSI des données calculées
         └── cœur              ScaleStructure, ChordStructure, *Database,
                               Tuning, Fretboard, ChordDiagrams
```

Le cœur ne fait aucun affichage. Une vue de l'application suit toujours le même schéma :

```cpp
Fretboard fb;
auto notes = cs.getChordNotes(root);                       // calcul
TextRenderer::noteList(std::cout, "ACCORD: " + name, notes);
TextRenderer::neck(std::cout, fb, fb.map(root, notes, cs.getDegreeLabels()));  // rendu
```

### TextRenderer.hpp

| Fonction | Dessine |
|----------|---------|
| `noteList(os, titre, notes)` | en-tête `=== TITRE ===` et liste de notes |
| `neck(os, fb, grid)` | manche complet coloré |
| `scaleMap(os, fb, grid, titre, degrés)` | notes de l'ensemble seules, par nom ou par degré |
| `chordDiagram(os, diagramme, titre)` | diagramme vertical d'accord |
| `diagramLegend(os)` | légende des couleurs et symboles |

Toutes les fonctions écrivent dans un `std::ostream`, ce qui permet de tester le rendu
sans terminal.

### Terminal.hpp

`clear()`, `showTitle()`, `showSection()`, `printSuccess/Error/Info/Warning()`,
`waitEnter()`, `selectFromListWithArrows()`, `selectScale()`, `selectChord()`,
`selectRootNote()`, `printTable()`.

## ➕ Ajouter une vue

1. Calculer les données avec le cœur (`Fretboard`, `DiagramsDatabase`…)
2. Ajouter, si besoin, une fonction de dessin dans `TextRenderer.hpp`
3. Ajouter une méthode à `Application` et une entrée dans `run()`

---

**Dernière mise à jour** : 2026-10-29
