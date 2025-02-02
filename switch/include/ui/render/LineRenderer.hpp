#pragma once
#include <pu/ui/ui_Types.hpp>

namespace pksm::ui::render {

    class LineRenderer {
        public:
            static void RenderLine(const pu::ui::Color& clr, const pu::i32 x1, const pu::i32 y1, const pu::i32 x2, const pu::i32 y2, const pu::i32 thickness = 1);
    };

} 