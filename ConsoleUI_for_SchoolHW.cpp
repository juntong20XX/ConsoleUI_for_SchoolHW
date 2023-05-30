// ConsoleUI_for_SchoolHW.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ConsoleUI_for_SchoolHW.hpp"

int main() {
    auto &ui = ConsoleUI::UI::get_instance();
    ui.set_group("HW")
            .add("Hello")
            .add("world")
            .add("!");
//    auto choose = ui.show_group_and_choose("HW");
//    ConsoleUI::UI::clear();
//    std::cout << choose << std::endl;
    ui.show_group("HW");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
