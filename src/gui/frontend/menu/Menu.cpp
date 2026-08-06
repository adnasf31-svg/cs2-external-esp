#include "Menu.hpp"

#include "core/engine/cache/Cache.hpp"
#include "gui/renderer/Renderer.hpp" // Circular dependency
#include "gui/renderer/window/Window.hpp" // Circular dependency
#include "assets/fonts/Icons.h";
#include <cmath>


bool Menu::Init() {
	return GetInstance().InitImpl();
}

void Menu::Render() {
	return GetInstance().RenderImpl();
}

void Menu::RenderStartupHelp() {
	return GetInstance().RenderStartupHelpImpl();
}

ImVec2 Menu::GetPos() {
	return GetInstance().pos;
}

ImVec2 Menu::GetSize() {
	return GetInstance().size;
}

bool Menu::InitImpl() {
	SetupStyles();

	LOGF(INFO, "Successfully initialized menu...");
	return true;
}

void Menu::RenderImpl() {
	if (!isSetup)
		return;

	static auto io = ImGui::GetIO();
	static auto screen = io.DisplaySize;
	static auto color_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None;

#ifdef _DEBUG
	static auto title = "Luvi Client | Dev";
#else
	static auto title = "Luvi Client";
#endif

	if (cfg::settings::rainbow) {
		const auto now = std::chrono::steady_clock::now().time_since_epoch();
		const float seconds = std::chrono::duration<float>(now).count();
		const float hue = std::fmod(seconds * 0.15f, 1.0f);
		float r = 0.0f, g = 0.0f, b = 0.0f;

		ImGui::ColorConvertHSVtoRGB(hue, 0.75f, 0.95f, r, g, b);
		theme.accent = ImVec4(r, g, b, 1.0f);

		ImGui::ColorConvertHSVtoRGB(std::fmod(hue + 0.08f, 1.0f), 0.45f, 0.45f, r, g, b);
		theme.accentSoft = ImVec4(r, g, b, 1.0f);

		ImGui::ColorConvertHSVtoRGB(std::fmod(hue + 0.03f, 1.0f), 0.22f, 0.20f, r, g, b);
		theme.panelAlt = ImVec4(r, g, b, 1.0f);
	}

	ApplyThemeToStyle();

	const float ui_scale = std::max(0.8f, std::min(cfg::settings::menu_scale, 1.3f));
	ImGui::SetNextWindowSize(ImVec2(cfg::settings::window_width * ui_scale, cfg::settings::window_height * ui_scale), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(screen.x / 2 - (cfg::settings::window_width * ui_scale / 2), screen.y / 2 - (cfg::settings::window_height * ui_scale / 2)), ImGuiCond_FirstUseEver);

	ImGui::GetWindowPos();
	if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		this->pos = ImGui::GetWindowPos();
		this->size = ImGui::GetWindowSize();

		static int active_tab = 0;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.panelAlt);
		if (ImGui::BeginChild("##header", ImVec2(0, 72), true))
		{
			ImGui::PushFont(ImGui::GetFont());
			ImGui::TextColored(theme.accent, "Luvi Client");
			ImGui::SameLine();
			ImGui::TextDisabled("| premium overlay suite");
			ImGui::PopFont();

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, theme.accentSoft);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme.accent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme.accent);
			ImGui::Button(cfg::enabled ? "ONLINE" : "OFFLINE", ImVec2(90, 24));
			ImGui::PopStyleColor(3);
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::Spacing();

		if (ImGui::BeginChild("##main_split"))
		{
			auto size = ImGui::GetContentRegionAvail();

			ImGui::BeginChild("##tab_buttons", ImVec2(120, size.y), true);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.1f, 0.5f));
				for (const auto& tab : tabs)
				{
					bool is_active = (active_tab == tab.id);

					if (is_active)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
					}

					if (ImGui::Button((tab.icon + " " + tab.label).c_str(), ImVec2(-1, 28)))
						active_tab = tab.id;

					if (is_active) 
						ImGui::PopStyleColor(3);
				}
				ImGui::PopStyleVar(1);

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Checkbox("Enable", &cfg::enabled);
			}
			ImGui::EndChild();

			ImGui::SameLine();


			ImGui::BeginDisabled(!cfg::enabled);

			ImGui::BeginChild("##tab_content", ImVec2(0, size.y), true);
			{
				if (active_tab == Tab::PLAYER)
				{
					ImGui::Text("Visuals");
					ImGui::Separator();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Box", &cfg::esp::box);
						ImGui::BeginDisabled(!cfg::esp::box);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team box color", cfg::esp::colors::box_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy box color", cfg::esp::colors::box_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Skeleton", &cfg::esp::skeleton);
						ImGui::BeginDisabled(!cfg::esp::skeleton);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team skeleton color", cfg::esp::colors::skeleton_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy skeleton color", cfg::esp::colors::skeleton_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Head Tracker", &cfg::esp::head_tracker);
						ImGui::BeginDisabled(!cfg::esp::head_tracker);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team head tracker color", cfg::esp::colors::tracker_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy head tracker color", cfg::esp::colors::tracker_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Tracers", &cfg::esp::tracers);
						ImGui::BeginDisabled(!cfg::esp::tracers);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team tracer color", cfg::esp::colors::tracer_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy tracer color", cfg::esp::colors::tracer_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Health", &cfg::esp::health);
						if (cfg::esp::health)
							ImGui::Checkbox("Health Number", &cfg::esp::health_number);
						ImGui::Checkbox("Armor", &cfg::esp::armor);

						ImGui::Checkbox("Spotted", &cfg::esp::spotted);
						ImGui::SetItemTooltip("Esp will only be visible if the player has been spotted by you");

						ImGui::Checkbox("Show Team", &cfg::esp::team);
					}
					ImGui::EndGroup();

					//ImGui::SameLine();
					ImGui::Spacing();

					ImGui::Text("Flags");
					ImGui::Separator();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Flashed", &cfg::esp::flags::flashed);
						ImGui::BeginDisabled(!cfg::esp::flags::flashed);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team flashed color", cfg::esp::colors::flags::flashed_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy flashed color", cfg::esp::colors::flags::flashed_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Reloading", &cfg::esp::flags::reloading);
						ImGui::BeginDisabled(!cfg::esp::flags::reloading);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team reloading color", cfg::esp::colors::flags::reloading_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy reloading color", cfg::esp::colors::flags::reloading_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Defusing", &cfg::esp::flags::defusing);
						ImGui::BeginDisabled(!cfg::esp::flags::defusing);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team defusing color", cfg::esp::colors::flags::defusing_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy defusing color", cfg::esp::colors::flags::defusing_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Scoped", &cfg::esp::flags::scoped);
						ImGui::BeginDisabled(!cfg::esp::flags::scoped);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team scoped color", cfg::esp::colors::flags::scoped_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy scoped color", cfg::esp::colors::flags::scoped_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Has C4", &cfg::esp::flags::has_c4);
						ImGui::BeginDisabled(!cfg::esp::flags::has_c4);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team C4 color", cfg::esp::colors::flags::c4_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy C4 color", cfg::esp::colors::flags::c4_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Name", &cfg::esp::flags::name);
						ImGui::Checkbox("Money", &cfg::esp::flags::money);
						ImGui::Checkbox("Weapon", &cfg::esp::flags::weapon);
						ImGui::Checkbox("Ammo", &cfg::esp::flags::ammo);
						ImGui::Checkbox("Ping", &cfg::esp::flags::ping);
					}
					ImGui::EndGroup();
				}
				else if (active_tab == Tab::WORLD)
				{
					ImGui::Text("Bomb");
					ImGui::Separator();
					{
						ImGui::Checkbox("Bomb ESP", &cfg::esp::bomb);
						ImGui::SameLine();
						ImGui::ColorEdit4("Bomb color", cfg::esp::colors::bomb.data(), color_flags);
					}
					ImGui::Checkbox("Bomb Location", &cfg::world::bomb::location);
					ImGui::Checkbox("Bomb Timer", &cfg::world::bomb::timer);

					ImGui::Spacing();

					ImGui::Text("Spectator list");
					ImGui::Separator();

					ImGui::Checkbox("Enable", &cfg::world::spectators::enabled);
					if (cfg::world::spectators::enabled) {
						ImGui::Checkbox("Detailed", &cfg::world::spectators::detailed);
						ImGui::Checkbox("Only Self", &cfg::world::spectators::self_only);
						ImGui::SetItemTooltip("Only display users spectating you");
					}

					ImGui::Spacing();

					ImGui::Text("Misc");
					ImGui::Separator();

					ImGui::Checkbox("Crosshair", &cfg::world::crosshair::enabled);
					ImGui::Checkbox("Velocity Graph", &cfg::world::velocity::enabled);
				#ifdef _DEBUG // Part of the velocity graph for developers
					if (cfg::world::velocity::enabled) {
						ImGui::SliderInt("Sample rate", &cfg::world::velocity::sample_rate, 1, 100);
						ImGui::SliderFloat("Sample length", &cfg::world::velocity::sample_length, 1, 20, "%.1f");
					}
				#endif
					ImGui::Spacing();

					ImGui::Text("Radar");
					ImGui::Separator();

					ImGui::Checkbox("Radar", &cfg::world::radar::enabled);
					ImGui::BeginDisabled(!cfg::world::radar::enabled);
					{
						ImGui::SameLine();
						ImGui::SliderFloat("Range", &cfg::world::radar::range, 100.f, 8000.f, "%.0f u");
						ImGui::Checkbox("Disable Rotation", &cfg::world::radar::no_rotate);
					}
					ImGui::EndDisabled();
				}
				else if (active_tab == Tab::SETTINGS)
				{
					ImGui::Text("Aimbot");
					ImGui::Separator();
					ImGui::Checkbox("Enable Aimbot", &cfg::aimbot::enabled);
					ImGui::Checkbox("Visible Only", &cfg::aimbot::visible_only);
					ImGui::SliderFloat("FOV", &cfg::aimbot::fov, 1.f, 30.f, "%.1f");
					ImGui::SliderFloat("Smooth", &cfg::aimbot::smooth, 0.05f, 1.f, "%.2f");
					ImGui::SliderFloat("RCS X", &cfg::aimbot::rcs_scale_x, 0.f, 2.f, "%.2f");
					ImGui::SliderFloat("RCS Y", &cfg::aimbot::rcs_scale_y, 0.f, 2.f, "%.2f");
					ImGui::SliderInt("RCS Bullet", &cfg::aimbot::rcs_bullet, 1, 10);
					ImGui::Checkbox("Recoil Control", &cfg::aimbot::use_recoil_control);
					ImGui::SliderInt("Hotkey", &cfg::aimbot::hotkey, 0, 7);
					ImGui::TextDisabled("Hotkey index: 0=LBUTTON, 1=LMENU, 2=RBUTTON, 3=XBUTTON1, 4=XBUTTON2, 5=CAPITAL, 6=LSHIFT, 7=LCONTROL");
					ImGui::Spacing();
					if (ImGui::CollapsingHeader("More", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Checkbox("Rainbow Mode", &cfg::settings::rainbow);
						ImGui::SliderInt("Menu Width", &cfg::settings::window_width, 640, 1200);
						ImGui::SliderInt("Menu Height", &cfg::settings::window_height, 420, 900);
						ImGui::SliderFloat("Menu Scale", &cfg::settings::menu_scale, 0.8f, 1.3f, "%.2f");
						if (ImGui::Button("Reset size"))
						{
							cfg::settings::window_width = 760;
							cfg::settings::window_height = 520;
							cfg::settings::menu_scale = 1.0f;
						}
						ImGui::SameLine();
						ImGui::TextDisabled("Instantly resizes the menu window.");
					}

					ImGui::Spacing();
					ImGui::Text("Theme");
					ImGui::Separator();
					ImGui::ColorEdit4("Accent", &theme.accent.x, color_flags);
					ImGui::SameLine();
					ImGui::ColorEdit4("Accent Soft", &theme.accentSoft.x, color_flags);
					ImGui::ColorEdit4("Panel", &theme.panel.x, color_flags);
					ImGui::SameLine();
					ImGui::ColorEdit4("Background", &theme.background.x, color_flags);
					ImGui::ColorEdit4("Text", &theme.text.x, color_flags);
					ImGui::SameLine();
					ImGui::ColorEdit4("Muted", &theme.muted.x, color_flags);
					ImGui::Spacing();
					ImGui::TextDisabled("The theme updates instantly and keeps the menu polished and readable.");

					ImGui::Spacing();
					ImGui::Text("Misc");
					ImGui::Separator();

					if (ImGui::Checkbox("Streamproof", &cfg::settings::streamproof))
					{
						Window::SetAffinity(
							Window::hwnd,
							cfg::settings::streamproof ? WindowAffinity::Invisible : WindowAffinity::Disabled
						);
					}

					ImGui::Checkbox("Watermark", &cfg::settings::watermark);

					if (ImGui::Checkbox("VSync", &cfg::settings::vsync))
						Window::vsync = cfg::settings::vsync;

					ImGui::Checkbox("Free CPU", &cfg::settings::free_cpu);
					ImGui::SetItemTooltip("Let the CPU sleep to Free Resources\nNOTE: might cause performance issues in lower end computers!");

					ImGui::Text("Notes");
					ImGui::Separator();
					ImGui::TextWrapped(
						"If you experience bad performance/lag try the following:\n"
						"\t- Disable ESP VSync: Look up > VSync: Un-Check\n"
						"\t- Disable VSync in game: ...Advanced Video > V-Sync: Disabled\n"
						"\t- Last Resort: Disable \"Free CPU\" option, it will inpact on your overall performace, but improve latency\n"
					);

#ifdef _DEBUG
					ImGui::Text("Dev");
					ImGui::Separator();

					if (ImGui::Checkbox("Console", &cfg::dev::console))
						if (!cfg::dev::console) LogHelper::Free();

					static int key_out;
					if (ImGui::Button("Open Menu Key"))
					{
						for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++)
						{
							if (ImGui::IsKeyPressed((ImGuiKey)i))
							{
								key_out = i;
								LOGF(VERBOSE, "Changed the open menu key to {}", key_out);
								break;
							}
						}
					}

					ImGui::SliderInt("Cache Refresh Rate", &cfg::dev::cache_refresh_rate, 0, 100, "%dms");
					ImGui::Checkbox("Force Show Flags", &cfg::dev::force_show_flags);
#endif
				}
			}
			ImGui::EndChild();

			ImGui::EndDisabled();


			ImGui::EndChild();
		}

	}

	ImGui::End();
}

void Menu::ApplyThemeToStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = theme.text;
	style.Colors[ImGuiCol_TextDisabled] = theme.muted;
	style.Colors[ImGuiCol_WindowBg] = theme.background;
	style.Colors[ImGuiCol_ChildBg] = theme.panel;
	style.Colors[ImGuiCol_PopupBg] = theme.panelAlt;
	style.Colors[ImGuiCol_Border] = ImVec4(0.24f, 0.29f, 0.38f, 0.85f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	style.Colors[ImGuiCol_FrameBg] = theme.panelAlt;
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.26f, 0.34f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = theme.accentSoft;
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.07f, 0.10f, 0.70f);

	style.Colors[ImGuiCol_MenuBarBg] = theme.panel;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.04f, 0.06f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32f, 0.37f, 0.46f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.46f, 0.56f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = theme.accent;
	style.Colors[ImGuiCol_CheckMark] = theme.accent;

	style.Colors[ImGuiCol_SliderGrab] = theme.accent;
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.10f, 0.60f, 0.78f, 1.00f);

	style.Colors[ImGuiCol_Button] = theme.panelAlt;
	style.Colors[ImGuiCol_ButtonHovered] = theme.accentSoft;
	style.Colors[ImGuiCol_ButtonActive] = theme.accent;
	style.Colors[ImGuiCol_Header] = theme.accentSoft;
	style.Colors[ImGuiCol_HeaderHovered] = theme.accent;
	style.Colors[ImGuiCol_HeaderActive] = theme.accent;
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = theme.accent;
	style.Colors[ImGuiCol_SeparatorActive] = theme.accent;

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = theme.accent;
	style.Colors[ImGuiCol_ResizeGripActive] = theme.accent;

	style.Colors[ImGuiCol_Tab] = theme.panelAlt;
	style.Colors[ImGuiCol_TabHovered] = theme.accentSoft;
	style.Colors[ImGuiCol_TabActive] = theme.accent;
	style.Colors[ImGuiCol_TabUnfocused] = theme.panel;
	style.Colors[ImGuiCol_TabUnfocusedActive] = theme.panelAlt;
	style.Colors[ImGuiCol_PlotLines] = theme.accent;
	style.Colors[ImGuiCol_PlotLinesHovered] = theme.accent;
	style.Colors[ImGuiCol_PlotHistogram] = theme.accent;
	style.Colors[ImGuiCol_PlotHistogramHovered] = theme.accent;
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.30f);
	style.Colors[ImGuiCol_DragDropTarget] = theme.accent;
	style.Colors[ImGuiCol_NavHighlight] = theme.accent;
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style.FrameBorderSize = 1.0f;
	style.WindowRounding = 16.f;
	style.ChildRounding = 12.f;
	style.FrameRounding = 6.f;
	style.PopupRounding = 8.f;
	style.GrabRounding = 4.f;
	style.ItemSpacing = ImVec2(10.f, 8.f);
	style.ItemInnerSpacing = ImVec2(8.f, 6.f);
}

void Menu::SetupStyles() {
	ApplyThemeToStyle();

	auto& io = ImGui::GetIO();

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);

	ImFontConfig merge_icon_cfg{};
	merge_icon_cfg.FontDataOwnedByAtlas = false;
	merge_icon_cfg.MergeMode = true;
	merge_icon_cfg.GlyphOffset = Vec2_t(0, 3.5f);

	// the icons will use the size specified when getting added so it ignores the base size
	static const ImWchar icon_ranges[] = { 0xE100, 0xE108, 0 };
	io.Fonts->AddFontFromMemoryTTF(icons_font, icons_font_len, 20.f, &merge_icon_cfg, icon_ranges);
}

void Menu::RenderStartupHelpImpl() {
	static bool has_opened_menu = false;

	if (has_opened_menu)
		return;

	auto& io = ImGui::GetIO();
	auto screen = io.DisplaySize;
	auto d = ImGui::GetBackgroundDrawList();

	if (Renderer::IsOpen())
		has_opened_menu = true;

	auto help = "To OPEN the menu, Use Insert or Right Shift keys"
		"\n\t\t\t\tTo CLOSE, press the End key";
	auto size = ImGui::CalcTextSize(help);

	d->AddText(
		ImVec2(screen.x / 2 - size.x / 2, 80),
		IM_COL32(255, 255, 255, 255),
		help
	);
}
