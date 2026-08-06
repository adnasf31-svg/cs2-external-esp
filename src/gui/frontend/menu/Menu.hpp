#pragma once
#include "assets/fonts/Icons.h"

enum Tab {
    PLAYER,
    WORLD,
    SETTINGS
};

struct TabItem
{
    Tab id;
    std::string label;
    std::string icon;
};

static const TabItem tabs[] =
{
    { Tab::PLAYER,      "Player",   Icons::PERSON },
    { Tab::WORLD,       "World",    Icons::GLOBE },
    { Tab::SETTINGS,    "Settings", Icons::SETTINGS }
};

class Menu {
public:
    struct ThemeState {
        ImVec4 accent = ImVec4(0.12f, 0.72f, 0.95f, 1.00f);
        ImVec4 accentSoft = ImVec4(0.16f, 0.24f, 0.38f, 1.00f);
        ImVec4 background = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
        ImVec4 panel = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
        ImVec4 panelAlt = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
        ImVec4 text = ImVec4(0.95f, 0.96f, 0.99f, 1.00f);
        ImVec4 muted = ImVec4(0.70f, 0.76f, 0.84f, 1.00f);
    };

    ~Menu() = default;
    Menu(const Menu&) = delete;
    Menu(Menu&&) = delete;
    Menu& operator=(const Menu&) = delete;
    Menu& operator=(Menu&&) = delete;

    static bool Init();
    static void Render();

    static void RenderStartupHelp();

    static ImVec2 GetPos();
    static ImVec2 GetSize();
private:
    Menu() {};

    static Menu& GetInstance()
    {
        static Menu i{};
        return i;
    }

    bool InitImpl();
    void RenderImpl();
    void RenderStartupHelpImpl();

    void SetupStyles();
    void ApplyThemeToStyle();
private:
    bool isSetup = true;

    ImVec2 pos;
    ImVec2 size;
    
    ImFont* font_icons;
    ThemeState theme;
};