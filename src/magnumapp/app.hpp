/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#pragma once

#include <Magnum/GL/DefaultFrameBuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

using namespace Magnum;

class GrohoApp : public Platform::Application {
public:
    explicit GrohoApp(const Arguments& arguments);

private:
    void drawEvent() override;
};

GrohoApp::GrohoApp(const Arguments& arguments)
    : Platform::Application{ arguments }
{
}

void GrohoApp::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    swapBuffers();
}