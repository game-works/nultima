#pragma once

#include <string>
#include <vector>

namespace Nultima
{

class Graphics;
class Keyboard;
class Model;

class Context
{
public:
    Context();
    ~Context();

    static Context* get();
    void            release();

    void            init(int argc, char** argv);
    void            deinit();

    std::string     getWorldFile();
    std::string     getStateFile();
    Graphics*       getGraphics ()                  { return m_graphics; }
    Keyboard*       getKeyboard ()                  { return m_keyboard; }
    const Model*    getModel    (int type);

private:
    int                 m_argc;
    char**              m_argv;
    Graphics*           m_graphics;
    Keyboard*           m_keyboard;
    std::vector<Model*> m_models;
};

}; // namespace