#include "ScalesDatabase.hpp"
#include "ChordsDatabase.hpp"
#include "ScaleStructure.hpp"
#include "ChordStructure.hpp"
#include <iostream>
#include <iomanip>

int main() {
    try {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "GAMMES DISPONIBLES" << std::endl;
        std::cout << std::string(70, '=') << std::endl << std::endl;

        // Afficher les gammes disponibles
        auto scales = ScalesDatabase::listScales();
        std::cout << "Total: " << scales.size() << " gammes\n" << std::endl;

        std::vector<std::string> scales_to_test = {"major", "minor_natural", "minor_harmonic", "pentatonic_major", "blues_scale", "wholeTone"};

        for (const auto& scale_name : scales_to_test) {
            const auto& entry = ScalesDatabase::getScale(scale_name);
            ScaleStructure scale(entry.formula);

            std::cout << "📊 " << std::left << std::setw(30) << entry.name;
            std::cout << " | Formule: " << std::setw(20) << entry.formula;
            std::cout << " | C : ";

            auto notes = scale.getNotes("C");
            for (const auto& note : notes) {
                std::cout << note << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "ACCORDS DISPONIBLES" << std::endl;
        std::cout << std::string(70, '=') << std::endl << std::endl;

        // Afficher les accords disponibles
        auto chords = ChordsDatabase::listChords();
        std::cout << "Total: " << chords.size() << " accords\n" << std::endl;

        std::vector<std::string> chords_to_test = {
            "major", "minor", "diminished", "augmented",
            "dominant7", "major7", "minor7", "minor7flat5",
            "dominant9", "major9", "add9", "sus2", "sus4",
            "7sharp9"
        };

        for (const auto& chord_name : chords_to_test) {
            const auto& entry = ChordsDatabase::getChord(chord_name);
            ChordStructure chord(entry.formula);

            std::cout << "🎼 " << std::left << std::setw(25) << entry.name;
            std::cout << " | Formule: " << std::setw(15) << entry.formula;
            std::cout << " | C : ";

            auto notes = chord.getChordNotes("C");
            for (const auto& note : notes) {
                std::cout << note << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "EXAMPLES DE PROGRESSIONS" << std::endl;
        std::cout << std::string(70, '=') << std::endl << std::endl;

        // Exemple : progression I-IV-V-I en Do majeur
        std::cout << "Progression I-IV-V-I en Do majeur :\n" << std::endl;
        ChordStructure major(ChordsDatabase::getFormula("major"));

        std::vector<std::string> roots = {"C", "F", "G", "C"};
        std::vector<std::string> names = {"I", "IV", "V", "I"};

        for (size_t i = 0; i < roots.size(); ++i) {
            auto notes = major.getChordNotes(roots[i]);
            std::cout << "  " << std::left << std::setw(3) << names[i];
            std::cout << " (" << std::left << std::setw(1) << roots[i] << ") : ";
            for (const auto& note : notes) {
                std::cout << note << " ";
            }
            std::cout << std::endl;
        }

        // Exemple : gamme mineure harmonique en A
        std::cout << "\nGamme mineure harmonique en A :\n" << std::endl;
        ScaleStructure harmonic_minor(ScalesDatabase::getFormula("minor_harmonic"));
        auto notes = harmonic_minor.getNotes("A");
        std::cout << "  Notes : ";
        for (const auto& note : notes) {
            std::cout << note << " ";
        }
        std::cout << std::endl;

        std::cout << "\n" << std::string(70, '=') << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
}
