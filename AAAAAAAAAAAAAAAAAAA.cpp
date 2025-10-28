#include <iostream>
#include <vector>
#include <memory>
#include <conio.h>  

// ----------------------------
// Base Skill Class
// ----------------------------
class askills {
public:
    std::string name;
    bool selected = false;

    askills(std::string n) : name(std::move(n)) {}
    virtual ~askills() = default;

    virtual void show() const {
        std::cout << name << (selected ? " [SELECTED]" : "") << std::endl;
    }
};

// ----------------------------
// Derived Skills
// ----------------------------
class Move : public askills {
public:
    Move() : askills("Move") {}
};

class Dash : public askills {
public:
    Dash() : askills("Dash") {}
};

class Shoot : public askills {
public:
    Shoot() : askills("Shoot") {}
};

// ----------------------------
// Globals
// ----------------------------
std::vector<std::unique_ptr<askills>> skillslist;  // all possible
std::vector<askills*> activeSkills;                // selected ones

// ----------------------------
// Function to display and select
// ----------------------------
void testDisplay() {
    std::cout << "\nSelect a skill (1-" << skillslist.size() << "):\n";
    for (size_t i = 0; i < skillslist.size(); ++i) {
        std::cout << i + 1 << ". ";
        skillslist[i]->show();
    }
    std::cout << "\nPress number key to select. Press 0 to finish.\n";

    if (_kbhit()) {
        char ch = _getch();
        int index = ch - '1'; // convert '1'..'9' to 0..8

        if (ch == '0') {
            std::cout << "\nSelection complete.\n";
            return;
        }

        if (index >= 0 && index < (int)skillslist.size()) {
            auto& skill = skillslist[index];
            if (!skill->selected) {
                skill->selected = true;
                activeSkills.push_back(skill.get());
                std::cout << "Added " << skill->name << " to active skills.\n";
            } else {
                std::cout << "⚠️ " << skill->name << " already selected.\n";
            }
        }
    }
}

// ----------------------------
// Display active skills
// ----------------------------
void showActiveSkills() {
    std::cout << "\n=== Active Skills ===\n";
    for (auto* s : activeSkills) {
        std::cout << "- " << s->name << std::endl;
    }
    std::cout << "=====================\n";
}

// ----------------------------
// Main
// ----------------------------
int main() {
    skillslist.push_back(std::make_unique<Move>());
    skillslist.push_back(std::make_unique<Dash>());
    skillslist.push_back(std::make_unique<Shoot>());

    std::cout << "Skill selection started...\n";

    while (true) {
        testDisplay();

        if (_kbhit()) {
            char ch = _getch();
            if (ch == '0') break;
        }

        showActiveSkills();
        _sleep(1000); // just to slow down the loop a bit
    }

    std::cout << "\nFinal Active Skills:\n"; 
    showActiveSkills();
    return 0;
}
