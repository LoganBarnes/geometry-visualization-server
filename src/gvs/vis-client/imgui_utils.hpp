// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace imgui {

struct Disable {
    class Guard {
    public:
        explicit Guard(bool disable);
        ~Guard();

    private:
        bool disable_;
    };

    static void disable_push();
    static void disable_pop();
};

} // namespace imgui
