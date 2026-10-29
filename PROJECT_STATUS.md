# 📊 MusiCLI — État du projet

**Dernière mise à jour** : 2026-10-29

## 🎯 Objectif

Un outil de théorie musicale pour guitaristes : gammes, accords et positions sur le
manche, avec une orthographe des notes rigoureuse, utilisable dans le navigateur
(avec le son) ou dans le terminal.

## ✅ Réalisé

### Cœur musical
- Gammes et accords décrits par des formules de degrés
- Orthographe théorique stricte : une lettre par degré (C# majeur : E#, B# ;
  D# majeur : F##, C## ; Cdim7 : Bbb)
- Validation des formules (degrés 1–7, altérations cumulables, notes en double)
- 24 gammes et 37 accords, formules vérifiées
- 6 accordages, hauteurs MIDI pour le son
- Notes sur le manche (Fretboard) et recherche de positions d'accords (ChordFinder)
- Aucun affichage dans le cœur : il renvoie des données

### Interface web (`make open`)
- Moteur compilé en WebAssembly, page locale sans serveur ni dépendance externe
- Manche SVG coloré par fonction, notes ou degrés, de 12 à 24 cases
- Positions d'accords cliquables et reportées sur le manche
- Son de corde pincée (Karplus-Strong), juste à moins d'un cent
- Raccourcis clavier, thèmes clair et sombre, réglages mémorisés

### Application terminal (`make run`)
- Accord sur le manche, gamme sur le manche (noms et degrés), diagrammes par
  accordage, manche complet

### Qualité
| Suite | Résultat |
|-------|----------|
| `test_scale` | 18 / 18 |
| `test_chord` | 17 / 17 |
| `test_fretboard` | 32 / 32 |
| `test_wasm` | 9 / 9 |

Compilation sans avertissement (`-Wall -Wextra`).

## 🔄 Historique

| Date | Étape |
|------|-------|
| 2026-05 | Gammes, accords, bases de données, application terminal, diagrammes d'accords |
| 2026-10 | Séparation calcul / affichage, module WebAssembly, interface web avec son, corrections musicales et orthographe stricte |

## 📋 Prochaines étapes

Détail dans [TODO.md](TODO.md). En priorité :

1. Accords d'une tonalité et progressions d'accords
2. Doigté et barré dans la recherche de positions
3. Classe `Note` partagée
4. Clavier de piano et reconnaissance d'accord

## ⚠️ Limites connues

- Les positions d'accords ne tiennent pas compte du doigté (possibles positions à
  plus de 4 doigts) et exigent toutes les notes de l'accord.
- L'application terminal propose ses propres types d'accords pour les diagrammes
  (suffixes de `DiagramsDatabase`), distincts du catalogue `ChordsDatabase`.

## 📞 Documentation

- [README.md](README.md) : présentation et utilisation
- [FILES.md](FILES.md) : structure et rôle des fichiers
- [CLI.md](CLI.md) : application terminal
- [CHORD_DIAGRAMS.md](CHORD_DIAGRAMS.md) : recherche de positions d'accords
- [TODO.md](TODO.md) : feuille de route
