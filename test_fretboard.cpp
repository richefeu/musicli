// test_fretboard.cpp
// Tests du cœur sans affichage : Tuning, Fretboard, ChordDiagram.

#include "ChordDiagrams.hpp"
#include "Fretboard.hpp"
#include "ScaleStructure.hpp"
#include <iostream>
#include <string>
#include <vector>

static int passed = 0, failed = 0;

static void check(bool ok, const std::string &what) {
    std::cout << (ok ? "\033[32m✓\033[0m " : "\033[31m✗\033[0m ") << what << "\n";
    (ok ? passed : failed)++;
}

int main() {
    // ── Tuning ────────────────────────────────────────────────────────────
    check(Tuning::noteToPC("C") == 0 && Tuning::noteToPC("e") == 4, "noteToPC : notes naturelles");
    check(Tuning::noteToPC("E#") == 5 && Tuning::noteToPC("Cb") == 11, "noteToPC : E# et Cb");
    check(Tuning::noteToPC("Bbb") == 9 && Tuning::noteToPC("F##") == 7, "noteToPC : doubles altérations");
    check(Tuning::noteToPC("H") == -1 && Tuning::noteToPC("Cx") == -1, "noteToPC : notes invalides");

    Tuning std_t = Tuning::standard();
    check(std_t.midiAt(0, 0) == 40 && std_t.midiAt(5, 0) == 64, "MIDI : E2 grave, E4 aigu");
    check(std_t.midiAt(1, 3) == 48, "MIDI : corde de La case 3 = C3");
    for (const Tuning &t : {Tuning::standard(), Tuning::dropD(), Tuning::openG(),
                            Tuning::openD(), Tuning::openE(), Tuning::dadgad()}) {
        bool ok = true;
        for (int s = 0; s < t.stringCount(); ++s)
            if (t.midi[s] % 12 != t.openPC(s)) ok = false;
        check(ok, "Cohérence noms/MIDI : " + t.name);
    }

    // ── Fretboard ─────────────────────────────────────────────────────────
    Fretboard fb;
    ScaleStructure fsharp("1,2,3,4,5,6,7");
    FretGrid g = fb.map("F#", fsharp.getNotes("F#"), fsharp.getDegreeLabels());
    check(g.size() == 6 && g[0].size() == 13, "Grille 6 cordes × 13 cases");
    check(g[0][2].isRoot && g[0][2].name == "F#" && g[0][2].degree == "1",
          "F# majeur : Mi grave case 2 = fondamentale");
    check(g[0][1].inSet && g[0][1].degree == "7", "F# majeur : 7e degré présent en case 1");
    check(g[0][0].inSet == false && g[0][0].name == "E", "F# majeur : E hors gamme");

    ScaleStructure am("1,2,3-,4,5,6-,7-");
    g = fb.map("A", am.getNotes("A"), am.getDegreeLabels());
    check(g[0][8].name == "C" && g[0][8].degree == "3-", "La mineur : C = degré 3-");
    check(g[2][0].midi == 50 && !g[2][0].isRoot && g[2][0].degree == "4", "La mineur : D à vide = degré 4");

    FretGrid all = fb.mapAll();
    bool allIn = true;
    for (auto &row : all) for (auto &n : row) if (!n.inSet || n.isRoot) allIn = false;
    check(allIn, "mapAll : toutes les notes, aucune fondamentale");

    check(Fretboard::inlayDots(12) == 2 && Fretboard::inlayDots(7) == 1 && Fretboard::inlayDots(6) == 0,
          "Repères de touche");

    // ── ChordDiagram ──────────────────────────────────────────────────────
    DiagramsDatabase db;
    auto bb = db.find("Bb", std_t, 1);
    check(!bb.empty(), "Bb : au moins une position");
    if (!bb.empty()) {
        bool spelled = true;
        for (int s = 0; s < 6; ++s) {
            std::string n = bb[0].noteName(s);
            if (n == "A#") spelled = false;
        }
        check(spelled, "Bb : orthographié Bb, pas A#");
        check(bb[0].midiNotes().size() == static_cast<size_t>(bb[0].pos().played()),
              "Bb : une hauteur MIDI par corde jouée");
    }

    auto c = db.find("C", std_t, 1);
    check(!c.empty() && c[0].startFret() == 1, "C ouvert : diagramme dès la case 1");

    std::cout << "\nResults: " << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
