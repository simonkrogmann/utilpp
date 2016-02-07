#include "Renderer.h"

#include <iostream>

#include <glbinding/gl/gl.h>
#include <utilgpu/gl/viewport.h>

using namespace gl;

namespace util
{
Renderer::Renderer() : m_fileWatcher{}, m_frameTimeDisplay{}
{
    m_fileFBO = Framebuffer::Simple(1920, 1080);
    m_frameTimeDisplay.init();
}

Renderer::~Renderer()
{
}

void Renderer::render(const util::viewport::Viewport& viewport)
{
    if (m_fileWatcher.check())
    {
        reload();
    }
    draw(viewport);
    if (m_measureFrameTime)
    {
        m_frameTimeDisplay.render(viewport);
    }
}

void Renderer::toggleFrameTimeDisplay()
{
    m_measureFrameTime = !m_measureFrameTime;
}

void Renderer::renderToFile(const util::viewport::Viewport& resolution)
{
    m_fileFBO->resize(resolution.width, resolution.height);
    renderOffscreen(m_fileFBO.get(), resolution);
    m_fileFBO->save("final.png");
    saveFramebuffers();
}

void Renderer::renderOffscreen(const Framebuffer* fbo,
                               const util::viewport::Viewport& resolution)
{
    const auto keeper = util::viewport::use(resolution);
    const auto keeper2 = fbo->use(GL_DRAW_FRAMEBUFFER);

    render(resolution);
}

void Renderer::addDependentPath(const util::File& path)
{
    m_fileWatcher.addFile(path);
}
}
