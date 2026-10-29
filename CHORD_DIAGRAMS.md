# 🎸 Diagrammes d'accords

`ChordDiagrams.hpp` calcule les positions jouables d'un accord sur le manche, pour
n'importe quel accordage. Aucune position n'est écrite à la main : elles sont
cherchées à partir des notes de l'accord. Le fichier ne fait aucun affichage ; le rendu
est assuré par `TextRenderer::chordDiagram()` (terminal) et `web/app.js` (SVG).

## Classes

| Classe | Rôle |
|--------|------|
| `Tuning` (`Tuning.hpp`) | Accordage : noms des cordes, hauteurs MIDI à vide |
| `ChordPos` | Une position : case de chaque corde (`-1` étouffée, `0` à vide, `n` case n) |
| `ChordDiagram` | Position + accordage + notes de l'accord, avec les données de rendu |
| `ChordFinder` | Recherche des positions sur le manche |
| `DiagramsDatabase` | Registre suffixe → formule (`"m7"` → `"1,3-,5,7-"`) et point d'entrée |

## Utilisation

```cpp
DiagramsDatabase db;

// Par nom : fondamentale + suffixe
auto diags = db.find("Am7", Tuning::standard(), 3);

// Par formule (c'est ce que fait l'interface web avec ChordsDatabase)
auto d2 = db.findByFormula(ChordsDatabase::getFormula("diminished7"), "C",
                           Tuning::dropD(), "Cdim7", 4);

const ChordDiagram &d = diags[0];
d.pos().frets;      // {-1, 0, 2, 0, 1, 0}
d.pos().label(d.tuning());   // "E:x A:o D:2 G:o B:1 e:o"
d.startFret();      // première case à dessiner (1 pour un accord ouvert)
d.noteName(s);      // note jouée sur la corde s, orthographiée selon l'accord (Bb, pas A#)
d.isRoot(s);        // la corde s joue-t-elle la fondamentale ?
d.midiNotes();      // hauteurs MIDI des cordes jouées, du grave à l'aigu
```

Suffixes reconnus par `find()` : `""`, `m`, `-`, `aug`, `+`, `dim`, `5`, `sus2`, `sus4`,
`7`, `m7`, `-7`, `maj7`, `M7`, `mM7`, `dim7`, `m7b5`, `aug7`, `7sus4`, `6`, `m6`,
`add9`, `madd9`, `9`, `m9`, `maj9`, `11`, `m11`, `13` (liste complète : `knownTypes()`).

## Algorithme de recherche

`ChordFinder::find(chordPCs, rootPC, maxResults)` parcourt le manche (12 cases) par
fenêtres de 4 cases :

1. **Candidats par corde** : étouffée, à vide si la note à vide appartient à l'accord,
   ou toute case de la fenêtre donnant une note de l'accord.
2. **Exploration** (backtracking) des combinaisons, en élaguant dès que l'écart entre
   la case pressée la plus basse et la plus haute dépasse 3.
3. **Validation** d'une position :
   - toutes les notes de l'accord sont présentes ;
   - la fondamentale est jouée ;
   - aucune corde étouffée entre deux cordes jouées.
4. **Dédoublonnage**, puis **tri par score** (le plus bas d'abord) :

   | Critère | Effet |
   |---------|-------|
   | la note la plus grave n'est pas la fondamentale | +8 |
   | case la plus basse | +2 par case |
   | corde à vide | −1 chacune |
   | corde jouée | −1 chacune |
   | écart entre cases | +1 par case |

   Les positions basses, ouvertes, pleines, avec la fondamentale à la basse, passent
   donc en premier.

`ChordDiagram::startFret()` choisit la fenêtre dessinée : case 1 pour un accord avec
cordes à vide tenant dans les 4 premières cases, sinon la case pressée la plus basse
(affichée `5fr`).

## Limites connues

- Pas de doigté ni de barré : une position peut demander plus de 4 doigts.
- Toutes les notes sont exigées : aucune note n'est omise sur les accords étendus
  (la quinte d'un 13 par exemple). Les 37 accords de la base ont néanmoins au moins
  une position en C (vérifié par `test_fretboard`).
- Guitare à 6 cordes, 12 premières cases.

## Tests

- `test_fretboard.cpp` : orthographe (Bb), hauteurs MIDI, fenêtre d'un accord ouvert,
  position trouvée pour chaque accord de la base
- `test_chord_diagram.cpp` : démonstration affichée pour plusieurs accordages

---

**Dernière mise à jour** : 2026-10-29
