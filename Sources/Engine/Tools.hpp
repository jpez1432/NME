
#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <glm.hpp>

#include "../Config.hpp"

namespace Tools {

    int FindView(double XPos, double YPos);

    void Frame(int Index, bool All = true);
    void Select(int Index, bool Start = false);

    void DuplicateSelection(void);
    void DeleteSelection(int Option);
    void DeleteAll(void);

    void SelectAll(void);
    void SelectNone(void);
    void SelectInvert(void);

    void HideSelection(void);
    void HideNone(void);

    void Rotate(glm::vec3 Rotation, glm::bvec3 Include, int About = 0, bool Global = true);
    void Scale(glm::vec3 Scale, glm::bvec3 Include, int About = 0, bool Into = false);
    void Move(glm::vec3 Move, glm::bvec3 Include, int Offset);
}

#endif
