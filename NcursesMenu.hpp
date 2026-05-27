// NcursesMenu.hpp - Menu interactif avec ncurses
#pragma once

#include <vector>
#include <string>
#include <ncurses.h>

class NcursesMenu {
private:
    std::vector<std::string> items;
    int selected;
    std::string title;
    
public:
    NcursesMenu(const std::string& title) : selected(0), title(title) {}
    
    void addItem(const std::string& item) {
        items.push_back(item);
    }
    
    int run() {
        // Initialiser ncurses avec support UTF-8
        setlocale(LC_ALL, ""); // Pour les caractères accentués
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0); // Masquer le curseur
        
        // Couleurs si disponibles
        if (has_colors()) {
            start_color();
            init_pair(1, COLOR_BLACK, COLOR_WHITE); // Sélection inversée
            init_pair(2, COLOR_CYAN, COLOR_BLACK);  // Titre
            init_pair(3, COLOR_GREEN, COLOR_BLACK); // Instructions
        }
        
        int ch;
        bool running = true;
        
        while (running) {
            clear();
            
            // Afficher le titre
            if (has_colors()) {
                attron(COLOR_PAIR(2) | A_BOLD);
            } else {
                attron(A_BOLD);
            }
            mvprintw(2, (COLS - title.length()) / 2, "%s", title.c_str());
            if (has_colors()) {
                attroff(COLOR_PAIR(2) | A_BOLD);
            } else {
                attroff(A_BOLD);
            }
            
            // Afficher les instructions
            if (has_colors()) {
                attron(COLOR_PAIR(3));
            }
            mvprintw(4, 2, "📝 Utilisez les flèches haut/bas pour naviguer, Entrée pour sélectionner");
            mvprintw(5, 2, "   (ou entrez un numéro 1-%d)", static_cast<int>(items.size()));
            if (has_colors()) {
                attroff(COLOR_PAIR(3));
            }
            
            // Afficher les éléments
            for (size_t i = 0; i < items.size(); ++i) {
                int y = 7 + static_cast<int>(i);
                if (i == static_cast<size_t>(selected)) {
                    if (has_colors()) {
                        attron(COLOR_PAIR(1) | A_BOLD);
                    }
                    mvprintw(y, 2, "  > %zu. %s", i + 1, items[i].c_str());
                    if (has_colors()) {
                        attroff(COLOR_PAIR(1) | A_BOLD);
                    }
                } else {
                    mvprintw(y, 2, "    %zu. %s", i + 1, items[i].c_str());
                }
            }
            
            // Afficher l'invite
            mvprintw(LINES - 3, 2, "> ");
            refresh();
            
            // Lire l'entrée utilisateur
            ch = getch();
            
            switch (ch) {
                case KEY_UP:
                    selected = (selected - 1 + items.size()) % items.size();
                    break;
                case KEY_DOWN:
                    selected = (selected + 1) % items.size();
                    break;
                case '\n':
                case KEY_ENTER:
                    running = false;
                    break;
                case 'q':
                case 'Q':
                case 27: // Échap
                    selected = -1;
                    running = false;
                    break;
                default:
                    if (ch >= '1' && ch <= '9') {
                        int num = ch - '0';
                        if (num >= 1 && num <= static_cast<int>(items.size())) {
                            selected = num - 1;
                            running = false;
                        }
                    }
                    break;
            }
        }
        
        // Nettoyer ncurses
        endwin();
        
        return selected;
    }
};