#pragma once

/**
 * @file ChordDiagrams.hpp
 * @brief Système de diagrammes d'accords avancés pour guitaristes
 * 
 * Ce fichier contient les classes pour représenter et gérer les diagrammes d'accords
 * standard utilisés par les guitaristes. Les diagrammes incluent des informations
 * sur le doigté, la position de la fondamentale, et le niveau de difficulté.
 * 
 * @author Mistral Vibe
 * @date 2024
 */

#include <vector>
#include <string>
#include <unordered_map>

class ChordDiagram {
public:
    std::string name;        // Nom du diagramme (ex: "C (position ouverte)")
    std::vector<int> frets;   // -1 = corde à vide, 0 = corde étouffée, 1-4 = frette
    int rootString;          // Corde où se trouve la fondamentale (0-5, où 0 = E basse)
    int rootFret;           // Frette où se trouve la fondamentale
    std::vector<int> fingering; // Doigté (1-4, 0 = pas de doigt)
    std::string formula;     // Formule de l'accord
    int difficulty;         // Niveau de difficulté (1-10)
    
    ChordDiagram(const std::string& name, const std::vector<int>& frets, 
                 int rootString, int rootFret, const std::vector<int>& fingering, 
                 const std::string& formula, int difficulty = 5)
        : name(name), frets(frets), rootString(rootString), rootFret(rootFret), 
          fingering(fingering), formula(formula), difficulty(difficulty) {}
};

/**
 * @class ChordDiagramDatabase
 * @brief Base de données de diagrammes d'accords standard
 * 
 * Cette classe gère une collection de diagrammes d'accords pré-définis
 * pour les accords les plus courants. Elle permet de rechercher et
 * de récupérer des diagrammes en fonction du nom de l'accord.
 */
class ChordDiagramDatabase {
private:
    std::unordered_map<std::string, std::vector<ChordDiagram>> diagrams;
    
    /**
     * @brief Charge les diagrammes standard dans la base de données
     * 
     * Cette méthode initialise la base de données avec des diagrammes
     * pour les accords majeurs, mineurs et de septième.
     */
    void loadStandardDiagrams() {
        // Accords majeurs
        addMajorChords();
        addMinorChords();
        addSeventhChords();
    }
    
    /**
     * @brief Ajoute les diagrammes pour les accords majeurs
     * 
     * Cette méthode ajoute les diagrammes standard pour les accords majeurs
     * en positions ouvertes et barrées. Chaque accord a au moins un diagramme,
     * certains ont plusieurs positions alternatives.
     * 
     * Convention pour les tableaux :
     * - frets : positions pour E(0), A(1), D(2), G(3), B(4), e(5)
     * - fingering : doigté correspondant pour chaque corde
     * - -1 = corde non jouée, 0 = corde étouffée, 1-4 = frette
     */
    void addMajorChords() {
        // C majeur - 2 positions
        diagrams["C"].push_back(ChordDiagram(
            "C (position ouverte)", {0, 1, 0, 2, 3, -1}, 4, 3, {2, 1, 3, -1, -1, -1}, "1,3,5", 3
            // E:0, A:1, D:0, G:2, B:3, e:-1 (ne pas jouer)
            // Doigté: Index(1) sur A, Majeur(2) sur D, Annulaire(3) sur G
        ));
        diagrams["C"].push_back(ChordDiagram(
            "C (barré 3ème case)", {3, 5, 5, 4, 3, 3}, 4, 3, {1, 3, 4, 2, 1, 1}, "1,3,5", 7
            // Barré à la 3ème case - plus difficile
        ));
        
        // G majeur - 1 position
        diagrams["G"].push_back(ChordDiagram(
            "G (position ouverte)", {3, 2, 0, 0, 0, 3}, 4, 3, {2, 1, -1, -1, -1, 3}, "1,3,5", 2
            // E:3, A:2, D:0, G:0, B:0, e:3
            // Doigté: Index(1) sur A, Majeur(2) sur E basse, Annulaire(3) sur e aiguë
        ));
        
        // D majeur - 1 position
        diagrams["D"].push_back(ChordDiagram(
            "D (position ouverte)", {-1, -1, 0, 2, 3, 2}, 3, 2, {-1, -1, -1, 1, 3, 2}, "1,3,5", 3
            // E:-1, A:-1, D:0, G:2, B:3, e:2
            // Doigté: Index(1) sur G, Majeur(2) sur e, Annulaire(3) sur B
        ));
        
        // A majeur - 1 position
        diagrams["A"].push_back(ChordDiagram(
            "A (position ouverte)", {-1, 0, 2, 2, 2, 0}, 4, 2, {-1, -1, 1, 2, 3, -1}, "1,3,5", 2
            // E:-1, A:0, D:2, G:2, B:2, e:0
            // Doigté: Index(1) sur D, Majeur(2) sur G, Annulaire(3) sur B
        ));
        
        // E majeur - 1 position
        diagrams["E"].push_back(ChordDiagram(
            "E (position ouverte)", {0, 2, 2, 1, 0, 0}, 5, 1, {-1, 2, 3, 1, -1, -1}, "1,3,5", 2
            // E:0, A:2, D:2, G:1, B:0, e:0
            // Doigté: Index(1) sur G, Majeur(2) sur A, Annulaire(3) sur D
        ));
    }
    
    /**
     * @brief Ajoute les diagrammes pour les accords mineurs
     * 
     * Cette méthode ajoute les diagrammes standard pour les accords mineurs.
     * Les accords mineurs ont une tierce mineure (3-) au lieu d'une tierce majeure (3).
     */
    void addMinorChords() {
        // C mineur - 1 position (barré)
        diagrams["Cm"].push_back(ChordDiagram(
            "Cm (barré 3ème case)", {3, 5, 5, 3, 4, -1}, 4, 3, {1, 3, 4, 1, 2, -1}, "1,3-,5", 7
            // Barré à la 3ème case - difficile
            // E:3, A:5, D:5, G:3, B:4, e:-1
        ));
        
        // A mineur - 1 position (ouverte)
        diagrams["Am"].push_back(ChordDiagram(
            "Am (position ouverte)", {-1, 0, 2, 2, 1, 0}, 4, 2, {-1, -1, 1, 2, 1, -1}, "1,3-,5", 2
            // E:-1, A:0, D:2, G:2, B:1, e:0
            // Doigté: Index(1) sur B et D, Majeur(2) sur G
        ));
        
        // E mineur - 1 position (ouverte)
        diagrams["Em"].push_back(ChordDiagram(
            "Em (position ouverte)", {0, 2, 2, 0, 0, 0}, 5, 1, {-1, 2, 2, -1, -1, -1}, "1,3-,5", 2
            // E:0, A:2, D:2, G:0, B:0, e:0
            // Doigté: Majeur(2) sur A, Annulaire(3) sur D
        ));
    }
    
    /**
     * @brief Ajoute les diagrammes pour les accords de septième
     * 
     * Cette méthode ajoute les diagrammes standard pour les accords de septième dominante.
     * Les accords de septième ont la formule 1,3,5,7- (septième mineure).
     * Ces accords ont un son "bluesy" et sont très utilisés en jazz et blues.
     */
    void addSeventhChords() {
        // C7 - 1 position (ouverte)
        diagrams["C7"].push_back(ChordDiagram(
            "C7 (position ouverte)", {-1, 1, 0, 2, 3, -1}, 4, 3, {2, 1, -1, 3, -1, -1}, "1,3,5,7-", 4
            // E:-1, A:1, D:0, G:2, B:3, e:-1
            // Doigté: Index(1) sur A, Annulaire(3) sur B
            // La septième mineure (Bb) est sur la corde de B à la 3ème frette
        ));
        
        // G7 - 1 position (ouverte)
        diagrams["G7"].push_back(ChordDiagram(
            "G7 (position ouverte)", {3, 2, 0, 0, 0, 1}, 4, 3, {2, 1, -1, -1, -1, 1}, "1,3,5,7-", 3
            // E:3, A:2, D:0, G:0, B:0, e:1
            // Doigté: Index(1) sur A et e, Majeur(2) sur E basse
            // La septième mineure (F) est sur la corde de e à la 1ère frette
        ));
    }
    
public:
    /**
     * @brief Constructeur de ChordDiagramDatabase
     * 
     * Initialise la base de données en chargeant tous les diagrammes standard.
     */
    ChordDiagramDatabase() {
        loadStandardDiagrams();
    }

    /**
     * @brief Récupère tous les diagrammes pour un accord donné
     * @param chordName Nom de l'accord (ex: "C", "G7", "Am")
     * @return Vecteur de diagrammes correspondants, ou vecteur vide si aucun trouvé
     */
    std::vector<ChordDiagram> getDiagramsForChord(const std::string& chordName) const {
        auto it = diagrams.find(chordName);
        if (it != diagrams.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * @brief Récupère le meilleur diagramme pour un accord donné
     * @param chordName Nom de l'accord (ex: "C", "G7", "Am")
     * @param rootNote Note fondamentale (non utilisée pour l'instant)
     * @return Le meilleur diagramme trouvé, ou un diagramme "Unknown" si aucun trouvé
     * 
     * @note Actuellement retourne le premier diagramme trouvé.
     *       Future amélioration: choix basé sur la position de la fondamentale.
     */
    ChordDiagram getBestDiagram(const std::string& chordName, const std::string& /*rootNote*/) const {
        auto diagrams = getDiagramsForChord(chordName);
        if (diagrams.empty()) {
            return ChordDiagram("Unknown", {}, 0, 0, {}, "");
        }
        
        // Pour l'instant, retourner le premier diagramme
        // Plus tard, implémenter une logique de choix basée sur la rootNote
        return diagrams[0];
    }
};
