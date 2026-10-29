#pragma once

#include "ChordDiagrams.hpp"
#include "ChordStructure.hpp"
#include "ChordsDatabase.hpp"
#include "Fretboard.hpp"
#include "ScaleStructure.hpp"
#include "ScalesDatabase.hpp"
#include "Terminal.hpp"
#include "TextRenderer.hpp"
#include <iostream>
#include <string>
#include <vector>

class Application {
public:
    void run() {
        while (true) {
            Terminal::clear();
            Terminal::showTitle("MusiCLI — Menu Principal");

            std::vector<std::string> menuItems = {
                "Accord sur le manche",
                "Gamme sur le manche",
                "Diagrammes d'accords",
                "Manche complet",
                "Quitter"
            };

            int choice = Terminal::selectFromList("Menu Principal", menuItems);

            switch (choice) {
                case 0: showChordOnGuitar();   break;
                case 1: showScaleOnGuitar();   break;
                case 2: showChordDiagrams();   break;
                case 3: showCompleteNeck();    break;
                case 4: return;
                default: break;
            }
        }
    }

private:
    // ── Helpers de sélection ────────────────────────────────────────────────

    static Tuning chooseTuning_() {
        std::vector<std::pair<std::string, std::string>> items = {
            {"Standard  EADGBe", "Le plus courant"},
            {"Drop D    DADGBe", "Corde grave en Ré"},
            {"Open G    DGDGBd", "Sol ouvert — blues, slide"},
            {"Open D    DADF#Ad", "Ré ouvert"},
            {"Open E    EBEG#Be", "Mi ouvert"},
            {"DADGAD",           "Accordage celtique"},
        };
        int sel = Terminal::selectFromListWithArrows("♪ Choisissez un accordage", items);
        switch (sel) {
            case 1:  return Tuning::dropD();
            case 2:  return Tuning::openG();
            case 3:  return Tuning::openD();
            case 4:  return Tuning::openE();
            case 5:  return Tuning::dadgad();
            default: return Tuning::standard();
        }
    }

    // Retourne le suffixe DiagramsDatabase choisi, ou "" si annulé.
    static std::string chooseChordSuffix_() {
        using P = std::pair<std::string, std::string>;
        static const std::vector<P> TYPES = {
            {"",      "Majeur"},
            {"m",     "Mineur"},
            {"7",     "Dominante 7"},
            {"maj7",  "Majeur 7"},
            {"m7",    "Mineur 7"},
            {"sus2",  "Sus2"},
            {"sus4",  "Sus4"},
            {"7sus4", "7 Sus4"},
            {"aug",   "Augmenté"},
            {"dim",   "Diminué"},
            {"dim7",  "Diminué 7"},
            {"m7b5",  "Demi-diminué (m7b5)"},
            {"9",     "Dominante 9"},
            {"m9",    "Mineur 9"},
            {"maj9",  "Majeur 9"},
            {"6",     "Majeur 6"},
            {"m6",    "Mineur 6"},
            {"add9",  "Add9"},
            {"madd9", "Mineur Add9"},
            {"mM7",   "Mineur/Majeur 7"},
            {"aug7",  "Augmenté 7"},
            {"5",     "Power chord"},
            {"11",    "Dominante 11"},
            {"m11",   "Mineur 11"},
            {"13",    "Dominante 13"},
        };

        std::vector<std::pair<std::string, std::string>> items;
        for (const auto &[suf, label] : TYPES)
            items.emplace_back(label + (suf.empty() ? "" : "  (" + suf + ")"), "");

        int sel = Terminal::selectFromListWithArrows("♪ Type d'accord", items);
        if (sel < 0 || sel >= static_cast<int>(TYPES.size())) return "";
        return TYPES[sel].first;
    }

    // ── Vues principales ────────────────────────────────────────────────────

    void showCompleteNeck() {
        Terminal::clear();
        Terminal::showTitle("Manche complet");
        Fretboard fb;
        std::cout << "\n=== COMPLETE FRETBOARD ===\n";
        TextRenderer::neck(std::cout, fb, fb.mapAll());
        Terminal::waitEnter();
    }

    void showChordOnGuitar() {
        try {
            std::string chord_type = Terminal::selectChord();
            if (chord_type.empty()) return;

            std::string root = Terminal::selectRootNote();
            if (root.empty()) return;

            Terminal::clear();
            std::string formula = ChordsDatabase::getFormula(chord_type);
            ChordStructure cs(formula);
            std::string chordLabel = root + " " + ChordsDatabase::getChord(chord_type).name;
            Terminal::showTitle("Accord : " + chordLabel);

            // Affichage sur le manche
            Fretboard fb;
            auto notes = cs.getChordNotes(root);
            TextRenderer::noteList(std::cout, "ACCORD: " + root + chord_type, notes);
            TextRenderer::neck(std::cout, fb, fb.map(root, notes, cs.getDegreeLabels()));

            // Diagrammes d'accords (accordage standard)
            DiagramsDatabase diagDb;
            Tuning t = Tuning::standard();
            auto diags = diagDb.findByFormula(formula, root, t, "", 3);
            if (diags.empty()) {
                Terminal::printInfo("Aucun diagramme trouvé pour cet accord.");
            } else {
                std::cout << "\n  Diagrammes — accordage standard :\n";
                for (size_t i = 0; i < diags.size(); ++i) {
                    std::string label = "Position " + std::to_string(i + 1)
                                      + "  —  " + diags[i].pos().label(t);
                    TextRenderer::chordDiagram(std::cout, diags[i], label);
                }
                TextRenderer::diagramLegend(std::cout);
            }
        } catch (const std::exception &e) {
            Terminal::printError(e.what());
        }
        Terminal::waitEnter();
    }

    void showScaleOnGuitar() {
        try {
            std::string scale_name = Terminal::selectScale();
            if (scale_name.empty()) return;

            std::string root = Terminal::selectRootNote();
            if (root.empty()) return;

            Terminal::clear();
            Terminal::showTitle("Résultat — Gamme sur manche");

            const auto &entry = ScalesDatabase::getScale(scale_name);
            ScaleStructure scale(entry.formula);
            auto notes = scale.getNotes(root);
            Fretboard fb;
            FretGrid grid = fb.map(root, notes, scale.getDegreeLabels());
            TextRenderer::noteList(std::cout, "SCALE: " + entry.name + " (" + root + ")", notes);
            TextRenderer::scaleMap(std::cout, fb, grid, entry.name, false);
            TextRenderer::scaleMap(std::cout, fb, grid, entry.name, true);
        } catch (const std::exception &e) {
            Terminal::printError(e.what());
        }
        Terminal::waitEnter();
    }

    void showChordDiagrams() {
        Terminal::clear();
        Terminal::showSection("Étape 1/3 — Accordage");

        // 1. Accordage
        Tuning tuning = chooseTuning_();

        Terminal::clear();
        Terminal::showSection("Étape 2/3 — Note fondamentale");

        // 2. Fondamentale
        std::string root = Terminal::selectRootNote();
        if (root.empty()) return;

        Terminal::clear();
        Terminal::showSection("Étape 3/3 — Type d'accord");

        // 3. Type d'accord
        std::string suffix = chooseChordSuffix_();
        // suffix="" est valide (accord majeur) ; on accepte toujours

        // 4. Calcul et affichage
        Terminal::clear();
        std::string chordName = root + suffix;
        Terminal::showTitle("Diagrammes — " + chordName
                            + "  (" + tuning.name + ")");

        DiagramsDatabase db;
        auto diags = db.find(chordName, tuning, 3);

        if (diags.empty()) {
            Terminal::printWarning("Aucun diagramme trouvé pour " + chordName
                                   + " en accordage " + tuning.name + ".");
        } else {
            for (size_t i = 0; i < diags.size(); ++i) {
                std::string label = chordName
                                  + "  — position " + std::to_string(i + 1)
                                  + "  —  " + diags[i].pos().label(tuning);
                TextRenderer::chordDiagram(std::cout, diags[i], label);
            }
            TextRenderer::diagramLegend(std::cout);
        }
        Terminal::waitEnter();
    }
};
