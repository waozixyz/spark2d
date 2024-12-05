#ifndef SPARK_IMGUI_H
#define SPARK_IMGUI_H

#include <SDL3/SDL.h>
#include "cimgui.h"

#ifdef __cplusplus
extern "C" {
#endif

// Backend function declarations
CIMGUI_API bool ImGui_ImplSDL3_InitForSDLRenderer(SDL_Window* window, SDL_Renderer* renderer);
CIMGUI_API bool ImGui_ImplSDLRenderer3_Init(SDL_Renderer* renderer);
CIMGUI_API void ImGui_ImplSDL3_Shutdown(void);
CIMGUI_API bool ImGui_ImplSDL3_ProcessEvent(const SDL_Event* event);
CIMGUI_API void ImGui_ImplSDLRenderer3_NewFrame(void);
CIMGUI_API void ImGui_ImplSDL3_NewFrame(void);
CIMGUI_API void ImGui_ImplSDLRenderer3_RenderDrawData(struct ImDrawData* draw_data, SDL_Renderer* renderer);
CIMGUI_API void ImGui_ImplSDLRenderer3_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // SPARK_IMGUI_H