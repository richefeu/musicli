#pragma once

/**
 * @file ChordDiagrams.hpp
 * @brief Calcul et affichage de diagrammes d'accords pour guitare.
 *
 * Architecture :
 *   Tuning           — accordage des 6 cordes (usines : standard, dropD, openG, dadgad, …)
 *   ChordPos         — une position de l'accord sur le manche (cases + corde de la fondamentale)
 *   ChordDiagram     — diagramme complet (position + accordage + info harmonique + affichage)
 *   ChordFinder      — recherche de positions par backtracking sur le manche
 *   DiagramsDatabase — registre des types d'accords (suffixe → formule) + lookup
 */

#include "ChordStructure.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// ─── Tuning ───────────────────────────────────────────────────────────────────

struct Tuning {
    std::string              name;
    std::vector<std::string> strings; // [0]=corde grave, [5]=corde aiguë

    // Convertit un nom de note ("C", "F#", "Bb", "e" …) en classe de hauteur 0–11.
    // Normalise la première lettre en majuscule pour accepter "e", "d", etc.
    static int noteToPC(const std::string &note) {
        static const std::unordered_map<std::string, int> m = {
            {"C",0},{"B#",0},{"C#",1},{"Db",1},{"D",2},{"D#",3},{"Eb",3},
            {"E",4},{"Fb",4},{"F",5},{"E#",5},{"F#",6},{"Gb",6},{"G",7},
            {"G#",8},{"Ab",8},{"A",9},{"A#",10},{"Bb",10},{"B",11},{"Cb",11}
        };
        std::string n = note;
        if (!n.empty())
            n[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(n[0])));
        auto it = m.find(n);
        return (it != m.end()) ? it->second : -1;
    }

    int openPC(int s)          const { return noteToPC(strings[s]); }
    int noteAt(int s, int fret) const { return (openPC(s) + fret) % 12; }

    static Tuning standard() { return {"Standard (EADGBe)", {"E","A","D","G","B","e"}}; }
    static Tuning dropD()    { return {"Drop D (DADGBe)",   {"D","A","D","G","B","e"}}; }
    static Tuning openG()    { return {"Open G (DGDGBd)",   {"D","G","D","G","B","d"}}; }
    static Tuning openD()    { return {"Open D (DADFAd)",   {"D","A","D","F","A","d"}}; }
    static Tuning openE()    { return {"Open E (EBE G#Be)", {"E","B","E","G","B","e"}}; } // G# = Ab
    static Tuning dadgad()   { return {"DADGAD",             {"D","A","D","G","A","d"}}; }
};

// ─── ChordPos ─────────────────────────────────────────────────────────────────

struct ChordPos {
    // frets[s] : -1 = corde étouffée, 0 = corde à vide, n≥1 = case n
    std::vector<int> frets;
    int rootString = -1; // index de la corde portant la fondamentale (corde grave = 0)
    int rootFret   = 0;

    int loFret() const {
        int m = 99;
        for (int f : frets) if (f > 0) m = std::min(m, f);
        return (m == 99) ? 0 : m;
    }
    int hiFret() const {
        int m = 0;
        for (int f : frets) if (f > 0) m = std::max(m, f);
        return m;
    }
    int span()   const { return hiFret() > 0 ? hiFret() - loFret() : 0; }
    int played() const {
        int n = 0; for (int f : frets) if (f >= 0) n++; return n;
    }

    // Étiquette lisible : "E:x A:3 D:2 G:o B:1 e:o"
    std::string label(const Tuning &t) const {
        std::string s;
        for (int i = 0; i < 6; ++i) {
            if (i) s += " ";
            s += t.strings[i] + ":";
            if      (frets[i] <  0) s += "x";
            else if (frets[i] == 0) s += "o";
            else                    s += std::to_string(frets[i]);
        }
        return s;
    }
};

// ─── ChordDiagram ─────────────────────────────────────────────────────────────
//
// Encapsule une position + l'accordage + les informations harmoniques.
// La méthode display() produit un diagramme ASCII dans lequel :
//   • les cordes sont des lignes verticales │ continues,
//   • les frettes sont des traits horizontaux ─┼─ qui les traversent,
//   • une note pressée remplace le │ par son nom (rouge = fondamentale, vert = autre),
//   • une corde étouffée est affichée en mode atténué.

class ChordDiagram {
public:
    ChordDiagram(const ChordPos        &pos,
                 const Tuning          &tuning,
                 const std::vector<int> &chordPCs,
                 int                    rootPC,
                 const std::string     &chordName = "")
        : pos_(pos), tuning_(tuning), chordPCs_(chordPCs),
          rootPC_(rootPC), chordName_(chordName) {}

    // ── Accesseurs ──────────────────────────────────────────────────────────

    const ChordPos        &pos()      const { return pos_;      }
    const Tuning          &tuning()   const { return tuning_;   }
    const std::vector<int> &chordPCs() const { return chordPCs_; }
    int                    rootPC()   const { return rootPC_;   }
    const std::string     &chordName() const { return chordName_; }

    // ── Affichage ────────────────────────────────────────────────────────────

    void display(const std::string &title = "") const {
        static const std::string RST = "\033[0m";
        static const std::string RED = "\033[1;31m";  // fondamentale
        static const std::string GRN = "\033[1;32m";  // autre note de l'accord
        static const std::string DIM = "\033[2m";     // corde étouffée

        const std::vector<std::string> &STR = tuning_.strings;
        const std::string &t = title.empty() ? chordName_ : title;
        if (!t.empty()) std::cout << "\n  " << t << "\n";

        // Case de départ : accord ouvert = case 1 si toutes les notes tiennent
        // dans les 4 premières cases ; sinon on commence à la case la plus basse.
        bool hasOpen = false;
        for (int f : pos_.frets) if (f == 0) hasOpen = true;
        int hiF = pos_.hiFret();
        int startFret = (hasOpen && hiF <= 4) ? 1
                      : (pos_.loFret() <= 1)  ? 1
                      :  pos_.loFret();

        // Chaque corde : 3 chars. Préfixe : 4 chars (pour le label "XXfr").
        const std::string PFX = "    ";

        // Noms des cordes (haut)
        std::cout << PFX;
        for (int s = 0; s < 6; ++s) printStrName_(STR[s]);
        std::cout << "\n";

        // Marqueurs x (étouffée, atténué) / o (à vide) / espace
        std::cout << PFX;
        for (int s = 0; s < 6; ++s) {
            if      (pos_.frets[s] <  0) std::cout << DIM << " x " << RST;
            else if (pos_.frets[s] == 0) std::cout << " o ";
            else                          std::cout << "   ";
        }
        std::cout << "\n";

        // Sillet : label de position ("XXfr") ou espaces si on est en case 1
        if (startFret > 1) std::cout << std::setw(2) << startFret << "fr";
        else                std::cout << PFX;
        std::cout << hline_("═", 18) << "\n";

        // Slots de cases (4 rangées)
        for (int row = 0; row < 4; ++row) {
            int fret = startFret + row;

            // Cordes + notes
            std::cout << PFX;
            for (int s = 0; s < 6; ++s) {
                if (pos_.frets[s] < 0) {
                    std::cout << DIM << " │ " << RST;
                } else if (pos_.frets[s] == fret) {
                    int pc = tuning_.noteAt(s, fret);
                    const std::string &col = (pc == rootPC_) ? RED : GRN;
                    printNote_(pc, col);
                } else {
                    std::cout << " │ ";
                }
            }
            std::cout << " " << fret << "\n";

            // Frette (┼ atténué sur cordes étouffées)
            std::cout << PFX;
            for (int s = 0; s < 6; ++s) {
                if (pos_.frets[s] < 0) std::cout << DIM << "─┼─" << RST;
                else                    std::cout << "─┼─";
            }
            std::cout << "\n";
        }

        // Noms des cordes (bas)
        std::cout << PFX;
        for (int s = 0; s < 6; ++s) printStrName_(STR[s]);
        std::cout << "\n";
    }

private:
    ChordPos         pos_;
    Tuning           tuning_;
    std::vector<int> chordPCs_;
    int              rootPC_;
    std::string      chordName_;

    // ── Helpers d'affichage ─────────────────────────────────────────────────

    static std::string hline_(const std::string &unit, int n) {
        std::string r;
        r.reserve(unit.size() * n);
        for (int i = 0; i < n; ++i) r += unit;
        return r;
    }

    static const std::string &pcName_(int pc) {
        static const std::string names[12] = {
            "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
        };
        return names[pc % 12];
    }

    // Imprime un nom de note coloré dans une cellule de 3 chars.
    static void printNote_(int pc, const std::string &color) {
        static const std::string RST = "\033[0m";
        const std::string &n = pcName_(pc);
        std::cout << color
                  << (n.size() == 1 ? " " + n + " " : n + " ")
                  << RST;
    }

    // Imprime le nom d'une corde centré dans 3 chars.
    static void printStrName_(const std::string &n) {
        std::cout << (n.size() == 1 ? " " + n + " " : n + " ");
    }
};

// ─── ChordFinder ──────────────────────────────────────────────────────────────
//
// Cherche par backtracking toutes les positions jouables sur le manche pour un
// ensemble de classes de hauteur (chordPCs) avec une fondamentale (rootPC).
// Contraintes : écart entre les cases pressées ≤ SPAN, fondamentale présente,
// pas de corde étouffée entre deux cordes jouées.

class ChordFinder {
public:
    explicit ChordFinder(const Tuning &t, int maxFret = 12)
        : T_(t), maxFret_(maxFret) {}

    std::vector<ChordPos> find(const std::vector<int> &chordPCs,
                                int rootPC,
                                int maxResults = 3) const {
        std::vector<ChordPos> all;

        for (int win = 0; win <= maxFret_ - SPAN_; ++win) {
            std::vector<std::vector<int>> cands(6);
            for (int s = 0; s < 6; ++s) {
                cands[s].push_back(-1); // étouffer
                // Corde à vide
                if (contains_(chordPCs, T_.noteAt(s, 0)))
                    cands[s].push_back(0);
                // Cases dans la fenêtre
                int lo = std::max(1, win), hi = std::min(maxFret_, win + SPAN_);
                for (int f = lo; f <= hi; ++f)
                    if (contains_(chordPCs, T_.noteAt(s, f)))
                        cands[s].push_back(f);
            }
            std::vector<int> cur(6, -1);
            solve_(cands, chordPCs, rootPC, cur, 0, all);
        }

        // Dédoublonnage
        std::sort(all.begin(), all.end(),
                  [](const ChordPos &a, const ChordPos &b){ return a.frets < b.frets; });
        all.erase(std::unique(all.begin(), all.end(),
                              [](const ChordPos &a, const ChordPos &b){
                                  return a.frets == b.frets; }),
                  all.end());

        // Tri par score (meilleure position en premier)
        std::sort(all.begin(), all.end(), [&](const ChordPos &a, const ChordPos &b){
            return score_(a, rootPC) < score_(b, rootPC);
        });

        if (static_cast<int>(all.size()) > maxResults)
            all.resize(maxResults);
        return all;
    }

private:
    const Tuning &T_;
    int           maxFret_;
    static constexpr int SPAN_ = 3; // écart maximum entre la case basse et la case haute

    static bool contains_(const std::vector<int> &v, int x) {
        return std::find(v.begin(), v.end(), x) != v.end();
    }

    void validate_(const std::vector<int> &frets,
                   const std::vector<int> &chordPCs, int rootPC,
                   std::vector<ChordPos>  &out) const {
        // Toutes les notes de l'accord doivent être couvertes
        std::vector<bool> cov(chordPCs.size(), false);
        for (int s = 0; s < 6; ++s)
            if (frets[s] >= 0)
                for (size_t j = 0; j < chordPCs.size(); ++j)
                    if (T_.noteAt(s, frets[s]) == chordPCs[j]) cov[j] = true;
        for (bool c : cov) if (!c) return;

        // La fondamentale doit être présente
        int rootStr = -1;
        for (int s = 0; s < 6; ++s)
            if (frets[s] >= 0 && T_.noteAt(s, frets[s]) == rootPC)
                { rootStr = s; break; }
        if (rootStr < 0) return;

        // Pas de corde étouffée entre deux cordes jouées
        int lo = 6, hi = -1;
        for (int s = 0; s < 6; ++s)
            if (frets[s] >= 0) { lo = std::min(lo, s); hi = std::max(hi, s); }
        for (int s = lo; s <= hi; ++s)
            if (frets[s] < 0) return;

        ChordPos p;
        p.frets      = frets;
        p.rootString = rootStr;
        p.rootFret   = frets[rootStr];
        out.push_back(p);
    }

    void solve_(const std::vector<std::vector<int>> &cands,
                const std::vector<int> &chordPCs, int rootPC,
                std::vector<int>       &cur, int s,
                std::vector<ChordPos>  &out) const {
        if (s == 6) { validate_(cur, chordPCs, rootPC, out); return; }

        for (int f : cands[s]) {
            // Élagage : l'écart de cases ne dépasse pas SPAN_
            int lo = 99, hi = 0;
            for (int i = 0; i < s; ++i)
                if (cur[i] > 0) { lo = std::min(lo, cur[i]); hi = std::max(hi, cur[i]); }
            if (f > 0) { lo = std::min(lo, f); hi = std::max(hi, f); }
            if (hi > 0 && lo < 99 && hi - lo > SPAN_) continue;

            cur[s] = f;
            solve_(cands, chordPCs, rootPC, cur, s + 1, out);
            cur[s] = -1;
        }
    }

    int score_(const ChordPos &p, int rootPC) const {
        int sc = 0;
        // Préférer la fondamentale sur la corde grave jouée
        int lowest = -1;
        for (int i = 0; i < 6; ++i) if (p.frets[i] >= 0) { lowest = i; break; }
        if (lowest >= 0 && T_.noteAt(lowest, p.frets[lowest]) != rootPC) sc += 8;
        // Préférer les positions basses sur le manche
        sc += p.loFret() * 2;
        // Bonus pour les cordes à vide (ouvertes)
        for (int f : p.frets) if (f == 0) sc -= 1;
        // Préférer davantage de cordes jouées
        sc -= p.played();
        // Préférer un faible écart entre cases
        sc += p.span();
        return sc;
    }
};

// ─── DiagramsDatabase ─────────────────────────────────────────────────────────
//
// Registre des types d'accords (suffixe → formule en degrés).
// Permet de rechercher des diagrammes par nom d'accord ("Am7", "G", "Bbdim7"…)
// ou directement par formule + fondamentale.
//
// Les noms d'accords sont analysés en :
//   root    — note fondamentale, ex. "A", "Bb", "F#"
//   suffix  — type d'accord, ex. "", "m", "7", "maj7", "dim7"
//
// Exemple :
//   DiagramsDatabase db;
//   auto diags = db.find("Am", Tuning::standard());
//   for (auto& d : diags) d.display("La mineur");

class DiagramsDatabase {
public:
    DiagramsDatabase() { initDefaults_(); }

    // Ajoute ou remplace un type d'accord.
    void add(const std::string &suffix, const std::string &formula) {
        typeFormulas_[suffix] = formula;
    }

    // Retourne la liste des suffixes connus.
    std::vector<std::string> knownTypes() const {
        std::vector<std::string> v;
        v.reserve(typeFormulas_.size());
        for (const auto &kv : typeFormulas_) v.push_back(kv.first);
        std::sort(v.begin(), v.end());
        return v;
    }

    // Analyse un nom d'accord ("Am7") → root ("A") + formula ("1,3-,5,7-").
    // Retourne false si le type est inconnu.
    bool parseChordName(const std::string &chordName,
                        std::string &rootOut,
                        std::string &formulaOut) const {
        if (chordName.empty()) return false;

        // Extraction de la fondamentale (1 lettre + optional # ou b)
        std::string root, suffix;
        if (chordName.size() >= 2 &&
            (chordName[1] == '#' || chordName[1] == 'b')) {
            root   = chordName.substr(0, 2);
            suffix = chordName.substr(2);
        } else {
            root   = chordName.substr(0, 1);
            suffix = chordName.substr(1);
        }

        auto it = typeFormulas_.find(suffix);
        if (it == typeFormulas_.end()) return false;

        rootOut    = root;
        formulaOut = it->second;
        return true;
    }

    // Recherche des diagrammes par nom d'accord + accordage.
    std::vector<ChordDiagram> find(const std::string &chordName,
                                    const Tuning      &tuning,
                                    int                maxResults = 3) const {
        std::string root, formula;
        if (!parseChordName(chordName, root, formula)) return {};
        return findByFormula(formula, root, tuning, chordName, maxResults);
    }

    // Recherche des diagrammes par formule + fondamentale + accordage.
    std::vector<ChordDiagram> findByFormula(const std::string &formula,
                                             const std::string &rootNote,
                                             const Tuning      &tuning,
                                             const std::string &chordName = "",
                                             int                maxResults = 3) const {
        // Calcul des classes de hauteur via ChordStructure
        std::vector<int> chordPCs = formulaToPCs_(formula, rootNote);
        if (chordPCs.empty()) return {};

        int rootPC = Tuning::noteToPC(rootNote);
        if (rootPC < 0) return {};

        // Recherche des positions
        ChordFinder finder(tuning);
        auto positions = finder.find(chordPCs, rootPC, maxResults);

        // Enveloppe dans des ChordDiagram
        std::vector<ChordDiagram> result;
        result.reserve(positions.size());
        for (auto &p : positions)
            result.emplace_back(p, tuning, chordPCs, rootPC, chordName);
        return result;
    }

private:
    std::unordered_map<std::string, std::string> typeFormulas_;

    // Calcule les classes de hauteur d'un accord à partir de sa formule + fondamentale.
    static std::vector<int> formulaToPCs_(const std::string &formula,
                                           const std::string &rootNote) {
        try {
            ChordStructure cs(formula);
            auto noteNames = cs.getChordNotes(rootNote);
            std::vector<int> pcs;
            pcs.reserve(noteNames.size());
            for (const auto &n : noteNames) {
                int pc = Tuning::noteToPC(n);
                if (pc >= 0) pcs.push_back(pc);
            }
            std::sort(pcs.begin(), pcs.end());
            pcs.erase(std::unique(pcs.begin(), pcs.end()), pcs.end());
            return pcs;
        } catch (...) {
            return {};
        }
    }

    void initDefaults_() {
        // ── Triades ──────────────────────────────────────────────────────────
        add("",      "1,3,5");      // Majeur
        add("m",     "1,3-,5");     // Mineur
        add("-",     "1,3-,5");     // Mineur (alt)
        add("aug",   "1,3,5+");     // Augmenté
        add("+",     "1,3,5+");     // Augmenté (alt)
        add("dim",   "1,3-,5-");    // Diminué
        add("5",     "1,5");        // Power chord
        add("sus2",  "1,2,5");      // Sus2
        add("sus4",  "1,4,5");      // Sus4

        // ── Accords de 7ème ──────────────────────────────────────────────────
        add("7",     "1,3,5,7-");   // Dominante 7
        add("m7",    "1,3-,5,7-");  // Mineur 7
        add("-7",    "1,3-,5,7-");  // Mineur 7 (alt)
        add("maj7",  "1,3,5,7");    // Majeur 7
        add("M7",    "1,3,5,7");    // Majeur 7 (alt)
        add("mM7",   "1,3-,5,7");   // Mineur / majeur 7
        add("dim7",  "1,3-,5-,6");  // Diminué 7  (bb7 = 6ème degré)
        add("m7b5",  "1,3-,5-,7-"); // Semi-diminué (min7 quinte bémol)
        add("aug7",  "1,3,5+,7-");  // Augmenté 7
        add("7sus4", "1,4,5,7-");   // 7sus4

        // ── Accords de 6ème ──────────────────────────────────────────────────
        add("6",     "1,3,5,6");    // Majeur 6
        add("m6",    "1,3-,5,6");   // Mineur 6

        // ── Extensions (9, 11, 13) ────────────────────────────────────────────
        // Les degrés 2 / 4 / 6 représentent ici la 9e / 11e / 13e (octave agnostique).
        add("add9",  "1,3,5,2");        // Add 9
        add("madd9", "1,3-,5,2");       // Mineur add 9
        add("9",     "1,3,5,7-,2");     // Dominante 9
        add("m9",    "1,3-,5,7-,2");    // Mineur 9
        add("maj9",  "1,3,5,7,2");      // Majeur 9
        add("11",    "1,3,5,7-,2,4");   // Dominante 11
        add("m11",   "1,3-,5,7-,2,4");  // Mineur 11
        add("13",    "1,3,5,7-,2,4,6"); // Dominante 13
    }
};
