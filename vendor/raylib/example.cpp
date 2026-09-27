#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
    InitWindow(800, 600, "Draggable Panel Example");
    SetTargetFPS(60);

    // Panel position and size stored in a Rectangle so it can change dynamically
    Rectangle infoPanelRect = { 50, 50, 300, 400 };

    // Dragging state variables
    bool isDraggingPanel = false;
    Vector2 dragOffset = { 0, 0 };

    char textBuffer[128] = "Type here...";
    bool textBoxEditing = false;
    int dropdownActive = 0;
    bool dropdownEditing = false;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        // --- PANEL DRAGGING LOGIC ---
        // Define a top title bar area for the panel (e.g., 30 pixels tall)
        Rectangle panelTitleBar = { infoPanelRect.x, infoPanelRect.y, infoPanelRect.width, 30 };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Check if user clicked inside the panel's title bar
            if (CheckCollisionPointRec(mousePos, panelTitleBar)) {
                isDraggingPanel = true;
                // Save the offset from the mouse to the panel's top-left corner
                dragOffset.x = mousePos.x - infoPanelRect.x;
                dragOffset.y = mousePos.y - infoPanelRect.y;
            }
        }

        // While holding down, update panel position based on mouse movement
        if (isDraggingPanel) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                infoPanelRect.x = mousePos.x - dragOffset.x;
                infoPanelRect.y = mousePos.y - dragOffset.y;
            }
            else {
                isDraggingPanel = false; // Release drag when mouse button is let go
            }
        }

        // --- TEXT BOX LOGIC ---
        Rectangle textBoxBounds = { 400, 50, 250, 30 };
        if (GuiTextBox(textBoxBounds, textBuffer, 128, textBoxEditing)) {
            textBoxEditing = !textBoxEditing;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, textBoxBounds)) {
                textBoxEditing = true;
                if (TextIsEqual(textBuffer, "Type here...")) {
                    textBuffer[0] = '\0';
                }
            }
            else {
                textBoxEditing = false;
                if (textBuffer[0] == '\0') {
                    TextCopy(textBuffer, "Type here...");
                }
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 1. Dropdown List (DRAW LAST)
        Rectangle dropdownBounds = { 400, 100, 250, 30 };
        const char* dropdownOptions = "Option One;Option Two;Option Three";
        if (GuiDropdownBox(dropdownBounds, dropdownOptions, &dropdownActive, dropdownEditing)) {
            dropdownEditing = !dropdownEditing;
        }
        DrawText(TextFormat("Entered Text: %s", textBuffer), 400, 160, 10, BLACK);
        DrawText(TextFormat("Selected Dropdown Index: %d", dropdownActive), 400, 180, 10, BLACK);

        // 2. Info Panel (using the dynamic infoPanelRect)
        GuiPanel(infoPanelRect, "Info Panel (Drag Here)");
        // Content positioned relative to the panel's moving coordinates
        DrawText("System Status: Online", infoPanelRect.x + 20, infoPanelRect.y + 40, 10, DARKGRAY);
        DrawText("FPS: ", infoPanelRect.x + 20, infoPanelRect.y + 60, 10, DARKGRAY);
        DrawText(TextFormat("%d", GetFPS()), infoPanelRect.x + 50, infoPanelRect.y + 60, 10, LIME);


        EndDrawing();
    }

    CloseWindow();
    return 0;
}