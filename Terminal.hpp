#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <cstdlib>
#include <cctype>
#include <termios.h>
#include <locale.h>
#include "NcursesMenu.hpp"
#include "ScalesDatabase.hpp"
#include "ChordsDatabase.hpp"

class Terminal {
public:
    // Effacer l'écran
    static void clear() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Afficher un titre amélioré avec encadré coloré
    static void showTitle(const std::string& title) {
        std::string line(70, '=');
        std::cout << "\n\033[1;36m+" << line << "+\033[0m\n";
        std::cout << "\033[1;36m|\033[0m  \033[1;37m" << std::left << std::setw(66) << title << "\033[0m\033[1;36m|\033[0m\n";
        std::cout << "\033[1;36m+" << line << "+\033[0m\n\n";
    }

    // Afficher une section avec séparateur coloré
    static void showSection(const std::string& section) {
        std::cout << "\n\033[2m──── " << section << " ────\033[0m\n\n";
    }

    // Afficher une ligne pointillée
    static void showDivider() {
        std::cout << std::string(70, '-') << std::endl;
    }

    // Afficher un message
    static void println(const std::string& msg) {
        std::cout << msg << std::endl;
    }

    // Afficher avec symboles colorés
    static void printSuccess(const std::string& msg) {
        std::cout << "\033[32m✓\033[0m " << msg << std::endl;
    }

    static void printError(const std::string& msg) {
        std::cout << "\033[31m✗\033[0m " << msg << std::endl;
    }

    static void printInfo(const std::string& msg) {
        std::cout << "\033[34mℹ\033[0m " << msg << std::endl;
    }

    static void printWarning(const std::string& msg) {
        std::cout << "\033[33m⚠\033[0m " << msg << std::endl;
    }

    // Pause robuste qui vide le buffer stdin complètement
    static void waitEnter() {
        std::cout << "\n  \033[2m[ Appuyez sur Entrée pour continuer ]\033[0m";
        std::cout.flush();
        while (std::getchar() != '\n') {
            // Vider le buffer complètement
        }
    }

    // Pause (alias, utilise la robuste waitEnter)
    static void pause() {
        waitEnter();
    }

    // Lire l'input utilisateur
    static std::string readInput(const std::string& prompt = "> ") {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    // Lire un entier
    static int readInteger(const std::string& prompt = "> ") {
        std::string input = readInput(prompt);
        try {
            return std::stoi(input);
        } catch (...) {
            return -1;
        }
    }

    // Lire une touche spéciale (flèches, entrée, etc.)
    static std::string readSpecialKey() {
        struct termios oldt, newt;
        std::string key;

        // Sauvegarder les paramètres actuels du terminal
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        // Configurer le terminal en mode raw
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            key += c;

            // Si c'est un caractère d'échappement (27), lire la séquence complète
            if (c == 27) {
                // Lire les caractères suivants (timeout court)
                struct timeval tv = {0, 100000}; // 100ms timeout
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(STDIN_FILENO, &fds);

                if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
                    char seq[3];
                    int bytes = read(STDIN_FILENO, seq, sizeof(seq));
                    if (bytes > 0) {
                        key.append(seq, bytes);
                    }
                }
            }
        }

        // Restaurer les paramètres du terminal
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return key;
    }

    // Afficher un menu et retourner le choix
    static int showMenu(const std::string& title, const std::vector<std::string>& options) {
        clear();
        showTitle(title);

        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
        }

        std::cout << std::endl;
        int choice = readInteger("Entrez un numéro (1-" + std::to_string(options.size()) + ") : ");

        if (choice < 1 || choice > static_cast<int>(options.size())) {
            printError("Option invalide");
            pause();
            return -1;
        }

        return choice - 1;
    }

    // Afficher une liste avec descriptions et sélection numérique
    static int selectFromList(const std::string& title, const std::vector<std::pair<std::string, std::string>>& items) {
        clear();
        showTitle(title);

        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << std::left << std::setw(25) << items[i].first;
            if (!items[i].second.empty()) {
                std::cout << " | " << items[i].second;
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        int choice = readInteger("Entrez un numéro (1-" + std::to_string(items.size()) + ") : ");

        if (choice < 1 || choice > static_cast<int>(items.size())) {
            printError("Option invalide");
            pause();
            return -1;
        }

        return choice - 1;
    }

    // Afficher une liste avec navigation par flèches haut/bas (utilisant ncurses)
    static int selectFromListWithArrows(const std::string& title, const std::vector<std::pair<std::string, std::string>>& items) {
        // Initialiser setlocale une seule fois pour l'UTF-8
        static bool localeInitialized = false;
        if (!localeInitialized) {
            setlocale(LC_ALL, "");
            localeInitialized = true;
        }

        // Créer un menu Ncurses avec les options
        NcursesMenu menu(title);

        for (const auto& item : items) {
            std::string display = item.first;
            if (!item.second.empty()) {
                display += " | " + item.second;
            }
            menu.addItem(display);
        }

        // Exécuter le menu et retourner la sélection
        return menu.run();
    }

    // Sélectionner un élément à partir d'une liste de chaînes
    static int selectFromList(const std::string& title, const std::vector<std::string>& items) {
        std::vector<std::pair<std::string, std::string>> pairs;
        for (const auto& item : items) {
            pairs.emplace_back(item, "");
        }
        return selectFromListWithArrows(title, pairs);
    }

    // Fonction utilitaire pour sélectionner une gamme
    static std::string selectScale() {
        auto scales = ScalesDatabase::listScales();
        std::vector<std::pair<std::string, std::string>> scaleItems;
        std::vector<std::string> scaleKeys;

        for (const auto& scale_name : scales) {
            try {
                const auto& entry = ScalesDatabase::getScale(scale_name);
                scaleItems.emplace_back(entry.name + " (" + entry.formula + ")", entry.description);
                scaleKeys.push_back(scale_name);
            } catch (...) {
                // Ignorer les erreurs
            }
        }

        int selected = selectFromListWithArrows("♩ Choisissez une gamme", scaleItems);
        if (selected >= 0 && selected < static_cast<int>(scaleKeys.size())) {
            return scaleKeys[selected];
        }
        return "";
    }

    // Fonction utilitaire pour sélectionner un accord
    static std::string selectChord() {
        auto chords = ChordsDatabase::listChords();
        std::vector<std::pair<std::string, std::string>> chordItems;
        std::vector<std::string> chordKeys;

        for (const auto& chord_name : chords) {
            try {
                const auto& entry = ChordsDatabase::getChord(chord_name);
                chordItems.emplace_back(entry.name + " (" + entry.formula + ")", entry.description);
                chordKeys.push_back(chord_name);
            } catch (...) {
                // Ignorer les erreurs
            }
        }

        int selected = selectFromListWithArrows("♩ Choisissez un accord", chordItems);
        if (selected >= 0 && selected < static_cast<int>(chordKeys.size())) {
            return chordKeys[selected];
        }
        return "";
    }

    // Fonction utilitaire pour sélectionner une note racine
    static std::string selectRootNote() {
        std::vector<std::string> notes = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };

        int selected = selectFromList("♩ Note fondamentale", notes);
        if (selected >= 0 && selected < static_cast<int>(notes.size())) {
            return notes[selected];
        }
        return "";
    }

    // Version simplifiée pour les menus sans descriptions
    static int selectFromListWithArrows(const std::string& title, const std::vector<std::string>& items) {
        std::vector<std::pair<std::string, std::string>> pairs;
        for (const auto& item : items) {
            pairs.emplace_back(item, "");
        }
        return selectFromListWithArrows(title, pairs);
    }

    // Afficher une table
    static void printTable(const std::vector<std::string>& headers,
                          const std::vector<std::vector<std::string>>& rows) {
        // Calculer les largeurs
        std::vector<size_t> widths(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); ++i) {
            widths[i] = headers[i].length();
        }
        for (const auto& row : rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                widths[i] = std::max(widths[i], row[i].length());
            }
        }

        // Afficher le header
        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << std::left << std::setw(widths[i] + 2) << headers[i];
        }
        std::cout << std::endl;

        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << std::string(widths[i] + 2, '-');
        }
        std::cout << std::endl;

        // Afficher les lignes
        for (const auto& row : rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << std::left << std::setw(widths[i] + 2) << row[i];
            }
            std::cout << std::endl;
        }
    }
};
