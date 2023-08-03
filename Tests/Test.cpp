#include "Test.h"

#include "imgui.h"

#include <iostream>

namespace test {

TestMenu::TestMenu(Test **currentTestPointer)
    : m_CurrentTest(currentTestPointer) {
}

void TestMenu::OnImGuiRender() {
    for (const auto &test : m_Tests) {
        if (ImGui::Button(test.first.c_str())) {
            std::cout << "Switching to test " << test.first << std::endl;
            *m_CurrentTest = test.second();
        }
    }
}

}