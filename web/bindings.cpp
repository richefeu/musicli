// bindings.cpp — expose le cœur MusiCLI à JavaScript (Emscripten / embind).
//
// Toutes les fonctions renvoient des objets JS simples (tableaux, objets) et
// lèvent une exception JS en cas d'entrée invalide.
//
//   const M = await createMusiCLI();
//   M.scaleNotes("dorian", "D")          → { notes: [...], degrees: [...] }
//   M.fretboard(0, 12, "D", notes, degs) → [[{pc, midi, name, degree, inSet, isRoot}, …], …]
//   M.chordDiagrams(0, "minor7", "A", 3) → [{ frets, startFret, names, isRoot, midi }, …]

#include "ChordDiagrams.hpp"
#include "ChordsDatabase.hpp"
#include "Fretboard.hpp"
#include "ScaleStructure.hpp"
#include "ScalesDatabase.hpp"
#include <emscripten/bind.h>
#include <emscripten/val.h>

using emscripten::val;

namespace {

template <typename T> val toArray(const std::vector<T> &v) {
    val a = val::array();
    for (size_t i = 0; i < v.size(); ++i) a.set(i, val(v[i]));
    return a;
}

std::vector<std::string> toStrings(const val &a) {
    return emscripten::vecFromJSArray<std::string>(a);
}

// Convertit une exception C++ en Error JS portant son message.
template <typename F> val guarded(F f) {
    try {
        return f();
    } catch (const std::exception &e) {
        val::global("Error").new_(std::string(e.what())).throw_();
    }
    return val::undefined();
}

const Tuning &tuningAt(int index) {
    static const std::vector<Tuning> presets = Tuning::presets();
    if (index < 0 || index >= static_cast<int>(presets.size()))
        throw std::invalid_argument("Accordage inconnu : " + std::to_string(index));
    return presets[index];
}

val notesAndDegrees(const std::vector<std::string> &notes,
                    const std::vector<std::string> &degrees) {
    val r = val::object();
    r.set("notes", toArray(notes));
    r.set("degrees", toArray(degrees));
    return r;
}

// ── Catalogues ─────────────────────────────────────────────────────────────

val tunings() {
    val a = val::array();
    int i = 0;
    for (const Tuning &t : Tuning::presets()) {
        val o = val::object();
        o.set("name", t.name);
        o.set("strings", toArray(t.strings));
        o.set("midi", toArray(t.midi));
        a.set(i++, o);
    }
    return a;
}

template <typename Entry>
val catalog(const std::vector<std::string> &keys, const Entry &(*get)(const std::string &)) {
    val a = val::array();
    int i = 0;
    for (const auto &key : keys) {
        const Entry &e = get(key);
        val o = val::object();
        o.set("key", key);
        o.set("name", e.name);
        o.set("formula", e.formula);
        o.set("description", e.description);
        o.set("aliases", toArray(e.aliases));
        a.set(i++, o);
    }
    return a;
}

val scales() { return catalog<ScaleEntry>(ScalesDatabase::listScales(), &ScalesDatabase::getScale); }
val chords() { return catalog<ChordEntry>(ChordsDatabase::listChords(), &ChordsDatabase::getChord); }

// ── Calcul ─────────────────────────────────────────────────────────────────

val scaleNotes(const std::string &key, const std::string &root) {
    return guarded([&] {
        ScaleStructure s(ScalesDatabase::getFormula(key));
        return notesAndDegrees(s.getNotes(root), s.getDegreeLabels());
    });
}

val chordNotes(const std::string &key, const std::string &root) {
    return guarded([&] {
        ChordStructure c(ChordsDatabase::getFormula(key));
        return notesAndDegrees(c.getChordNotes(root), c.getDegreeLabels());
    });
}

val fretboard(int tuning, int maxFret, const std::string &root,
              const val &notes, const val &degrees) {
    FretGrid grid;
    guarded([&] {
        grid = Fretboard(tuningAt(tuning), maxFret)
                   .map(root, toStrings(notes), toStrings(degrees));
        return val::undefined();
    });
    val rows = val::array();
    for (size_t s = 0; s < grid.size(); ++s) {
        val row = val::array();
        for (size_t f = 0; f < grid[s].size(); ++f) {
            const FretNote &n = grid[s][f];
            val o = val::object();
            o.set("pc", n.pc);
            o.set("midi", n.midi);
            o.set("name", n.name);
            o.set("degree", n.degree);
            o.set("inSet", n.inSet);
            o.set("isRoot", n.isRoot);
            row.set(f, o);
        }
        rows.set(s, row);
    }
    return rows;
}

// key : clé de ChordsDatabase ("major", "minor7" …)
val chordDiagrams(int tuning, const std::string &key,
                  const std::string &root, int maxResults) {
    std::vector<ChordDiagram> diags;
    guarded([&] {
        diags = DiagramsDatabase().findByFormula(ChordsDatabase::getFormula(key), root,
                                                 tuningAt(tuning), "", maxResults);
        return val::undefined();
    });
    val a = val::array();
    for (size_t i = 0; i < diags.size(); ++i) {
        const ChordDiagram &d = diags[i];
        std::vector<std::string> names;
        std::vector<bool> roots;
        for (int s = 0; s < d.tuning().stringCount(); ++s) {
            names.push_back(d.noteName(s));
            roots.push_back(d.isRoot(s));
        }
        val o = val::object();
        o.set("frets", toArray(d.pos().frets));
        o.set("startFret", d.startFret());
        o.set("names", toArray(names));
        o.set("isRoot", toArray(roots));
        o.set("midi", toArray(d.midiNotes()));
        a.set(i, o);
    }
    return a;
}

} // namespace

EMSCRIPTEN_BINDINGS(musicli) {
    emscripten::function("tunings", &tunings);
    emscripten::function("scales", &scales);
    emscripten::function("chords", &chords);
    emscripten::function("scaleNotes", &scaleNotes);
    emscripten::function("chordNotes", &chordNotes);
    emscripten::function("fretboard", &fretboard);
    emscripten::function("chordDiagrams", &chordDiagrams);
}
