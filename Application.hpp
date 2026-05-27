#pragma once

/**
 * @file Application.hpp
 * @brief Classe principale de l'application MaMusique
 * 
 * Cette classe gère l'interface utilisateur et l'orchestration des différentes
 * fonctionnalités de l'application : affichage des accords, des gammes, et des
 * diagrammes d'accords avancés.
 * 
 * L'application utilise une architecture modulaire avec séparation des responsabilités :
 * - Terminal : Gestion de l'interface utilisateur
 * - GuitarNeck : Affichage du manche de guitare
 * - ChordDiagramDatabase : Base de données des diagrammes d'accords
 * - ChordsDatabase/ScalesDatabase : Formules musicales
 * 
 * @author Mistral Vibe
 * @date 2024
 */

#include "Terminal.hpp"
#include "ScaleStructure.hpp"
#include "ChordStructure.hpp"
#include "ScalesDatabase.hpp"
#include "ChordsDatabase.hpp"
#include "GuitarNeck.hpp"
#include "ChordDiagrams.hpp"
#include <string>
#include <vector>
#include <limits>

/**
 * @class Application
 * @brief Classe principale gérant le flux de l'application
 * 
 * Cette classe est le point d'entrée principal de l'application.
 * Elle coordonne les différentes composantes pour fournir une
 * expérience utilisateur cohérente.
 */
class Application {
private:
    //Terminal terminal; ///< Instance du terminal pour l'interface utilisateur
    
public:
    /**
     * @brief Point d'entrée principal de l'application
     * 
     * Cette méthode lance la boucle principale de l'application
     * et gère la navigation entre les différentes fonctionnalités.
     * 
     * Le menu principal propose :
     * - Affichage des accords sur le manche
     * - Affichage des gammes sur le manche  
     * - Affichage complet du manche
     * - Quitter l'application
     */
    void run() {
        while (true) {
            Terminal::clear();
            Terminal::showTitle("MaMusique - Menu Principal");
            
            std::vector<std::string> menuItems = {
                "Afficher un accord sur le manche",
                "Afficher une gamme sur le manche",
                "Afficher toutes les notes du manche",
                "Quitter"
            };
            
            int choice = Terminal::selectFromList("Menu Principal", menuItems);
            
            switch (choice) {
                case 0:
                    showChordOnGuitar();
                    break;
                case 1:
                    showScaleOnGuitar();
                    break;
                case 2:
                    showCompleteNeck();
                    break;
                case 3:
                    return;
                default:
                    break;
            }
        }
    }
    
private:
    /**
     * @brief Affiche toutes les notes sur le manche de guitare
     * 
     * Cette méthode affiche un manche de guitare complet avec toutes les notes
     * chromatiques, utile pour visualiser la disposition des notes sur le manche.
     */
    void showCompleteNeck() {
        Terminal::clear();
        Terminal::showTitle("Manche complet");
        
        GuitarNeck guitar;
        guitar.showAllNotes();
        
        // Utiliser une pause plus longue pour ncurses
        std::cout << "\nAppuyez sur Entrée pour continuer...";
        std::cout.flush();
        std::cin.get();
    }
    
    /**
     * @brief Affiche un accord sur le manche de guitare avec diagramme
     * 
     * Cette méthode est la fonctionnalité principale pour l'affichage des accords.
     * Elle gère tout le processus :
     * 1. Sélection de l'accord et de la fondamentale via l'interface utilisateur
     * 2. Construction du nom complet de l'accord
     * 3. Affichage du manche avec les notes de l'accord
     * 4. Recherche et affichage du diagramme d'accord standard
     * 5. Gestion des erreurs et des cas où aucun diagramme n'est trouvé
     * 
     * @note Cette méthode utilise la nouvelle fonctionnalité de diagrammes d'accords
     *       avec gestion avancée de la fondamentale et du doigté.
     */
    void showChordOnGuitar() {
        try {
            std::string chord_type = Terminal::selectChord();
            if (chord_type.empty()) return;

            std::string root = Terminal::selectRootNote();
            if (root.empty()) return;

            // Construire le nom complet de l'accord (ex: "Caugmented")
            std::string chord_name = root + chord_type;

            Terminal::clear();
            Terminal::showTitle("Résultat - Accord sur manche");

            GuitarNeck guitar;
            ChordDiagramDatabase diagramDb;
            
            // Utiliser la nouvelle méthode avec ChordStructure pour une meilleure gestion des noms de notes
            try {
                std::string formula = ChordsDatabase::getFormula(chord_type);
                ChordStructure chordStructure(formula);
                
                // Afficher le manche avec l'accord
                guitar.showChord(chord_name, chordStructure, root);
                
                /**
                 * Affichage du diagramme d'accord standard
                 * 
                 * Note: La base de données utilise des noms simplifiés pour les accords:
                 * - "C" pour C major au lieu de "Cmajor"
                 * - "Cm" pour C minor au lieu de "Cminor"
                 * - "C7" pour C dominant 7th
                 * 
                 * Cette adaptation permet de trouver les diagrammes standard.
                 */
                std::string diagramChordName = chord_name;
                if (chord_type == "major") {
                    diagramChordName = root; // Utiliser juste la racine pour les accords majeurs
                }
                // TODO: Ajouter d'autres adaptations pour d'autres types d'accords
                
                ChordDiagram diagram = diagramDb.getBestDiagram(diagramChordName, root);
                if (diagram.name != "Unknown") {
                    guitar.displayChordDiagram(chord_name, chordStructure, root, diagram);
                } else {
                    // Générer un diagramme basique si aucun diagramme standard n'est trouvé
                    std::cout << "\n[Info] Aucun diagramme standard trouvé pour cet accord.\n";
                    std::cout << "Affichage d'un diagramme basique...\n";
                    // Pour l'instant, ne pas afficher de diagramme basique
                    // guitar.displayChordDiagram(chord_name, chordStructure, root);
                }
            } catch (const std::exception& e) {
                // En cas d'erreur, utiliser l'ancienne méthode
                guitar.showChord(chord_name);
            }

            // Utiliser une pause plus longue pour ncurses
            std::cout << "\nAppuyez sur Entrée pour continuer...";
            std::cout.flush();
            std::cin.get();
        } catch (const std::exception& e) {
            Terminal::printError(e.what());
            std::cout << "\nAppuyez sur Entrée pour continuer...";
            std::cout.flush();
            std::cin.get();
        }
    }
    
    /**
     * @brief Affiche une gamme sur le manche de guitare
     * 
     * Cette méthode permet de visualiser une gamme musicale sur le manche.
     * Elle gère :
     * 1. Sélection de la gamme et de la fondamentale via l'interface utilisateur
     * 2. Récupération de la formule de la gamme depuis la base de données
     * 3. Calcul des notes de la gamme
     * 4. Affichage du manche avec les notes de la gamme mises en évidence
     * 
     * Les gammes disponibles incluent :
     * - Gammes majeures et mineures naturelles
     * - Gammes pentatoniques
     * - Gammes blues
     * - Gammes modes (dorien, phrygien, etc.)
     * 
     * @note Les notes sont affichées avec leur degré dans la gamme pour une
     *       meilleure compréhension de la structure musicale.
     */
    void showScaleOnGuitar() {
        try {
            std::string scale_name = Terminal::selectScale();
            if (scale_name.empty()) return;

            std::string root = Terminal::selectRootNote();
            if (root.empty()) return;

            Terminal::clear();
            Terminal::showTitle("Résultat - Gamme sur manche");

            const auto& entry = ScalesDatabase::getScale(scale_name);
            ScaleStructure scale(entry.formula);
            auto notes = scale.getNotes(root);
            GuitarNeck guitar;
            guitar.showScale(entry.name, root, notes);

            // Utiliser une pause plus longue pour ncurses
            std::cout << "\nAppuyez sur Entrée pour continuer...";
            std::cout.flush();
            std::cin.get();
        } catch (const std::exception& e) {
            Terminal::printError(e.what());
            std::cout << "\nAppuyez sur Entrée pour continuer...";
            std::cout.flush();
            std::cin.get();
        }
    }
};
