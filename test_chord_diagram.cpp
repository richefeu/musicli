// test_chord_diagram.cpp
// Test du module ChordDiagrams.hpp : DiagramsDatabase + TextRenderer::chordDiagram()

#include "ChordDiagrams.hpp"
#include "TextRenderer.hpp"
#include <iostream>
#include <string>
#include <vector>

static std::string hline(const std::string &unit, int n) {
    std::string r;
    for (int i = 0; i < n; ++i) r += unit;
    return r;
}

static void section(const std::string &title) {
    std::cout << "\n" << hline("═", 60) << "\n";
    std::cout << title << "\n";
    std::cout << hline("═", 60) << "\n";
}

// Recherche par nom via DiagramsDatabase et affiche les diagrammes.
static void testByName(DiagramsDatabase &db, const Tuning &t,
                       const std::string &chordName, int maxResults = 3) {
    auto diags = db.find(chordName, t, maxResults);
    if (diags.empty()) {
        std::cout << "\n  [" << chordName << " : aucune position trouvée]\n";
        return;
    }
    for (size_t i = 0; i < diags.size(); ++i) {
        std::string label = chordName + "  — position " + std::to_string(i + 1)
                          + " — " + diags[i].pos().label(t);
        TextRenderer::chordDiagram(std::cout, diags[i], label);
    }
}

// Recherche par formule directe et affiche les diagrammes.
static void testByFormula(DiagramsDatabase &db, const Tuning &t,
                          const std::string &formula, const std::string &root,
                          const std::string &label, int maxResults = 3) {
    auto diags = db.findByFormula(formula, root, t, label, maxResults);
    if (diags.empty()) {
        std::cout << "\n  [" << label << " : aucune position trouvée]\n";
        return;
    }
    for (size_t i = 0; i < diags.size(); ++i) {
        std::string title = label + "  — position " + std::to_string(i + 1)
                          + " — " + diags[i].pos().label(t);
        TextRenderer::chordDiagram(std::cout, diags[i], title);
    }
}

int main() {
    DiagramsDatabase db;
    Tuning std_t = Tuning::standard();
    Tuning dropD = Tuning::dropD();
    Tuning openG = Tuning::openG();

    // ── Types d'accords connus ────────────────────────────────────────────────
    section("TYPES D'ACCORDS ENREGISTRÉS");
    auto types = db.knownTypes();
    std::cout << "Suffixes connus (" << types.size() << ") : ";
    for (size_t i = 0; i < types.size(); ++i) {
        if (i) std::cout << ", ";
        std::cout << (types[i].empty() ? "(majeur)" : types[i]);
    }
    std::cout << "\n";

    // ── Accordage standard : accords ouverts courants ─────────────────────────
    section("ACCORDAGE STANDARD — ACCORDS OUVERTS");
    for (const auto &name : {"C", "G", "D", "A", "E", "Am", "Em", "Dm"})
        testByName(db, std_t, name);

    // ── Accordage standard : accords de 7ème ─────────────────────────────────
    section("ACCORDAGE STANDARD — ACCORDS DE 7ÈME");
    for (const auto &name : {"G7", "C7", "Cmaj7", "Am7", "E7", "B7"})
        testByName(db, std_t, name);

    // ── Accordage standard : accords avec bémol/dièse ────────────────────────
    section("ACCORDAGE STANDARD — TONALITÉS ALTÉRÉES");
    for (const auto &name : {"Bb", "F", "Bbm", "F#m", "Eb", "Ab"})
        testByName(db, std_t, name, 2);

    // ── Accords diminués et augmentés ─────────────────────────────────────────
    section("ACCORDAGE STANDARD — DIMINUÉS / AUGMENTÉS");
    for (const auto &name : {"Adim", "Edim7", "Caug", "Gaug"})
        testByName(db, std_t, name);

    // ── Accords sus ───────────────────────────────────────────────────────────
    section("ACCORDAGE STANDARD — SUS2 / SUS4");
    for (const auto &name : {"Dsus2", "Asus2", "Gsus4", "Asus4"})
        testByName(db, std_t, name);

    // ── Recherche par formule directe ─────────────────────────────────────────
    section("RECHERCHE PAR FORMULE DIRECTE");
    testByFormula(db, std_t, "1,3-,5-,7-", "B", "Bm7b5 (semi-diminué)");
    testByFormula(db, std_t, "1,3,5,7-,2", "G", "G9 (dominante 9)");
    testByFormula(db, std_t, "1,4,5,7-",   "A", "A7sus4");

    // ── Drop D ────────────────────────────────────────────────────────────────
    section("ACCORDAGE DROP D");
    {
        for (const auto &name : {"D", "Dm", "D7"})
            testByName(db, dropD, name);
    }

    // ── Open G ───────────────────────────────────────────────────────────────
    section("ACCORDAGE OPEN G");
    {
        for (const auto &name : {"G", "C", "D", "Gm"})
            testByName(db, openG, name);
    }

    // ── Légende ───────────────────────────────────────────────────────────────
    section("LÉGENDE");
    std::cout << "  \033[1;31mXX\033[0m = fondamentale   "
              << "\033[1;32mXX\033[0m = autre note de l'accord\n";
    std::cout << "  \033[2mx\033[0m  = corde étouffée   o = corde à vide\n";
    std::cout << hline("═", 60) << "\n\n";

    return 0;
}
