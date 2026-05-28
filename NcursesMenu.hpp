// NcursesMenu.hpp - Menu interactif avec ncurses, scrollable et robuste
#pragma once

#include <vector>
#include <string>
#include <ncurses.h>
#include <algorithm>

class NcursesMenu {
private:
    std::vector<std::string> items;
    int selected;
    int topItem;  // Premier item visible
    std::string title;

    // Retourne le nombre max d'items visibles (réserve espace pour header+footer)
    int getMaxVisible() const {
        return std::max(1, LINES - 10);
    }

    // Ajuste topItem pour s'assurer que selected est visible
    void ensureVisible() {
        int maxVis = getMaxVisible();
        if (selected < topItem) {
            topItem = selected;
        } else if (selected >= topItem + maxVis) {
            topItem = selected - maxVis + 1;
        }
    }

public:
    NcursesMenu(const std::string& title) : selected(0), topItem(0), title(title) {}

    void addItem(const std::string& item) {
        items.push_back(item);
    }

    int run() {
        // Initialiser ncurses avec support UTF-8
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);  // Masquer le curseur

        // Couleurs si disponibles
        if (has_colors()) {
            start_color();
            init_pair(1, COLOR_BLACK, COLOR_CYAN);   // Sélection (noir sur cyan)
            init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Titre
            init_pair(3, COLOR_GREEN, COLOR_BLACK);  // Instructions
            init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Compteur et scroll
        }

        int ch;
        bool running = true;

        while (running) {
            clear();
            ensureVisible();

            int maxVis = getMaxVisible();
            int itemsAbove = topItem;
            int itemsBelow = static_cast<int>(items.size()) - (topItem + maxVis);

            // === HEADER ===
            // Afficher le titre centré avec compteur à droite
            std::string counter = "(" + std::to_string(selected + 1) + "/" +
                                  std::to_string(items.size()) + ")";
            if (has_colors()) {
                attron(COLOR_PAIR(2) | A_BOLD);
            } else {
                attron(A_BOLD);
            }

            int titleCol = (COLS - title.length()) / 2;
            mvprintw(1, titleCol, "%s", title.c_str());
            mvprintw(1, COLS - static_cast<int>(counter.length()) - 2, "%s", counter.c_str());

            if (has_colors()) {
                attroff(COLOR_PAIR(2) | A_BOLD);
            } else {
                attroff(A_BOLD);
            }

            // Ligne de séparation
            if (has_colors()) attron(COLOR_PAIR(4) | A_DIM);
            for (int x = 0; x < COLS; ++x) mvaddch(2, x, ACS_HLINE);
            if (has_colors()) attroff(COLOR_PAIR(4) | A_DIM);

            // Afficher les instructions
            if (has_colors()) attron(COLOR_PAIR(3) | A_DIM);
            mvprintw(3, 2, "↑↓ PgUp/PgDn  Entrée=sélect  Esc=annuler  1-9=accès direct");
            if (has_colors()) attroff(COLOR_PAIR(3) | A_DIM);

            // === ITEMS (scrollable) ===
            for (int i = 0; i < maxVis && topItem + i < static_cast<int>(items.size()); ++i) {
                int itemIdx = topItem + i;
                int y = 5 + i;

                if (itemIdx == selected) {
                    if (has_colors()) {
                        attron(COLOR_PAIR(1) | A_BOLD);
                    }
                    mvprintw(y, 2, "  > %d. %s", itemIdx + 1, items[itemIdx].c_str());
                    if (has_colors()) {
                        attroff(COLOR_PAIR(1) | A_BOLD);
                    }
                } else {
                    mvprintw(y, 2, "    %d. %s", itemIdx + 1, items[itemIdx].c_str());
                }
            }

            // === FOOTER ===
            int footerY = LINES - 2;

            // Indicateurs de scroll
            if (has_colors()) attron(COLOR_PAIR(4));

            if (itemsAbove > 0) {
                mvprintw(footerY, 2, "↑ (%d au-dessus)", itemsAbove);
            }
            if (itemsBelow > 0) {
                mvprintw(footerY, COLS - 20, "(%d en-dessous) ↓", itemsBelow);
            }

            if (has_colors()) attroff(COLOR_PAIR(4));

            // Ligne de séparation footer
            if (has_colors()) attron(COLOR_PAIR(4) | A_DIM);
            for (int x = 0; x < COLS; ++x) mvaddch(footerY - 1, x, ACS_HLINE);
            if (has_colors()) attroff(COLOR_PAIR(4) | A_DIM);

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

                case KEY_PPAGE:  // Page Up
                    selected = std::max(0, selected - maxVis);
                    break;

                case KEY_NPAGE:  // Page Down
                    selected = std::min(static_cast<int>(items.size()) - 1, selected + maxVis);
                    break;

                case KEY_HOME:
                    selected = 0;
                    break;

                case KEY_END:
                    selected = items.size() - 1;
                    break;

                case '\n':
                case KEY_ENTER:
                    running = false;
                    break;

                case 'q':
                case 'Q':
                case 27:  // Échap
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

        // Vider le buffer stdin avant de nettoyer ncurses
        flushinp();

        // Nettoyer ncurses
        endwin();

        return selected;
    }
};
