////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/Context.hpp"

#include <vector>

namespace curlew {
    class call_logger {
        using pred_type = std::function<bool(avocet::opengl::decorator_data)>;
        std::vector<std::string>* m_Calls{};
        pred_type m_Pred{};
    public:
        template<class Pred>
            requires std::is_invocable_r_v<bool, Pred, avocet::opengl::decorator_data>
        call_logger(std::vector<std::string>& calls, Pred pred)
            : m_Calls{&calls}
            , m_Pred{std::move(pred)}
        {
        }

        void operator()(const avocet::opengl::context&, const avocet::opengl::decorator_data& data) {
            if(m_Pred(data))
                m_Calls->push_back(std::string{data.fn_name});
        }
    };

}