# TODO — Feuille de route MusiCLI

## 🌐 Interface web
- [x] Moteur C++ compilé en WebAssembly, utilisable en `file://` sans serveur
- [x] Gammes et accords, fondamentales ♯/♭, accordages, 12 à 24 cases
- [x] Manche SVG coloré par fonction, notes ou degrés, clic pour entendre
- [x] Positions d'accords cliquables, reportées sur le manche
- [x] Son de corde pincée (Karplus-Strong)
- [ ] Clavier de piano (vue des gammes et accords)
- [ ] Accords d'une tonalité (harmonisation de la gamme, degrés I–VII)
- [ ] Progressions d'accords : saisie, lecture en boucle, sauvegarde
- [ ] Reconnaissance d'accord à partir de notes cliquées sur le manche
- [ ] Choix du son (attaque, durée, arpège ou gratté) et métronome
- [ ] Export d'une fiche (PDF / image) des diagrammes

## 🎵 Gammes (ScaleStructure)
- [x] Degrés et altérations, unicité des intervalles
- [x] Orthographe théorique stricte, doubles altérations comprises
- [x] Modes de la gamme majeure
- [ ] `getChordAtDegree()` : accords construits sur chaque degré

## 🎼 Accords (ChordStructure)
- [x] Même orthographe que les gammes
- [x] Validation des formules (degrés 1–7, notes en double, altérations multiples)
- [x] Positions sur le manche (ChordFinder)
- [ ] Renversements (basse sur la tierce, la quinte…)
- [ ] Accords à basse imposée et polychords (Cmaj7/G)
- [ ] Reconnaissance du type d'accord étendue (`getChordTypeName()` ne connaît que 10 types)

## 🎸 Manche et diagrammes
- [x] Grille de notes sur le manche (Fretboard), accordages prédéfinis
- [x] Notation x / o, plusieurs positions par accord, tri par jouabilité
- [ ] Doigté et barré (exclure les positions demandant plus de 4 doigts)
- [ ] Omission de notes pour les accords étendus (quinte d'un 13)
- [ ] Accordages personnalisés, guitare 7 cordes, basse

## 📊 Architecture
- [x] Cœur sans affichage, renderers séparés (terminal, web)
- [ ] Une seule source des types d'accords : l'application terminal utilise encore les
      suffixes de `DiagramsDatabase` pour ses diagrammes, le web utilise `ChordsDatabase`
- [ ] Classe `Note` partagée (lettre + altération) ; la conversion nom → hauteur est
      encore dupliquée dans ScaleStructure, ChordStructure et Tuning
- [ ] Classe `Interval`
- [ ] Classe `Tonality` (tonalité majeure / mineure, armure)
- [ ] Bases de données en JSON (import / export)

## 🗄️ Bases de données
- [x] 24 gammes et 37 accords vérifiés, formules corrigées
- [ ] Recherche par alias (`ChordsDatabase::getChord("m7")`)
- [ ] Descriptions musicales plus complètes (usage, styles)

## 💻 Application terminal
- [x] Menus aux flèches, manche, cartes de gammes, diagrammes par accordage
- [ ] Fondamentales en bémols (seuls les dièses sont proposés)
- [ ] Commandes directes sans menu (`musicli chord Am7 --tuning dropD`)

## 🧪 Tests
- [x] Enharmoniques et tonalités théoriques
- [x] Toutes les entrées des bases sur 17 fondamentales
- [x] Module WebAssembly sous Node
- [ ] Test automatisé de l'interface web dans un navigateur
- [ ] Benchmarks de ChordFinder

## 🔧 Build et outils
- [x] Makefile : application, tests, WebAssembly, ouverture de l'interface
- [x] `.gitignore`
- [ ] CMake
- [ ] clang-tidy, couverture de code (gcov)

---

## Priorités suggérées

1. **Accords d'une tonalité** et **progressions** : le plus utile au quotidien
2. **Doigté / barré** : écarter les positions injouables
3. **Classe `Note`** : supprimer la triple conversion nom → hauteur
4. **Clavier de piano** et **reconnaissance d'accord**

---

**Dernière mise à jour** : 2026-10-29
