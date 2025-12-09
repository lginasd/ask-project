#include "gui.h"

#include "conversion.h"
#include "raylib.h"
#include <algorithm>
#include <string>
#include <vector>

namespace gui {

using namespace std;
using namespace conversion;

namespace {

vector<string> BuildBitLines(const string &bits, size_t bitsPerLine = 32) {
  vector<string> lines;

  for (size_t i = 0; i < bits.size(); i += bitsPerLine) {
    string chunk = bits.substr(i, bitsPerLine);
    string grouped;
    for (size_t j = 0; j < chunk.size(); ++j) {
      if (j > 0 && j % 4 == 0) {
        grouped += ' ';
      }
      grouped += chunk[j];
    }
    lines.push_back(grouped);
  }

  return lines;
}

bool DrawButton(const Rectangle &bounds, const string &label, Color baseColor) {
  Vector2 mouse = GetMousePosition();
  bool hovered = CheckCollisionPointRec(mouse, bounds);
  bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

  Color fill = hovered ? ColorAlpha(baseColor, pressed ? 0.9f : 0.7f)
                       : ColorAlpha(baseColor, 0.5f);

  DrawRectangleRounded(bounds, 0.25f, 8, fill);
  DrawRectangleRoundedLines(bounds, 0.25f, 8,
                            pressed ? ColorAlpha(baseColor, 0.9f)
                                    : ColorAlpha(baseColor, 0.8f));

  Font font = GetFontDefault();
  const float fontSize = 20.0f;
  Vector2 textSize = MeasureTextEx(font, label.c_str(), fontSize, 1.0f);
  Vector2 textPos = {bounds.x + (bounds.width - textSize.x) / 2.0f,
                     bounds.y + (bounds.height - textSize.y) / 2.0f};
  DrawTextEx(font, label.c_str(), textPos, fontSize, 1.0f, RAYWHITE);

  return hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

void DrawInputField(const Rectangle &bounds, const string &value, bool active) {
  Color bg = {20, 28, 44, 230};
  Color border = active ? Color{83, 140, 255, 255} : Color{120, 135, 155, 200};
  DrawRectangleRounded(bounds, 0.2f, 8, bg);
  DrawRectangleRoundedLines(bounds, 0.2f, 8, border);

  Font font = GetFontDefault();
  const float fontSize = 28.0f;
  string display = value.empty() ? "Enter decimal value (e.g. 263.3)" : value;
  Color textColor = value.empty() ? Color{170, 180, 195, 255} : RAYWHITE;

  Vector2 textPos = {bounds.x + 20.0f, bounds.y + bounds.height / 2.0f - 14.0f};
  DrawTextEx(font, display.c_str(), textPos, fontSize, 1.0f, textColor);

  if (active) {
    Vector2 size = MeasureTextEx(font, value.c_str(), fontSize, 1.0f);
    float cursorX = bounds.x + 20.0f + size.x + 3.0f;
    float cursorHeight = bounds.height - 24.0f;
    if (value.empty()) {
      cursorX = bounds.x + 20.0f;
    }
    if ((static_cast<int>(GetTime() * 2) % 2) == 0) {
      DrawRectangle(cursorX, bounds.y + (bounds.height - cursorHeight) / 2.0f,
                    2, cursorHeight, RAYWHITE);
    }
  }
}

void DrawStatusBar(const string &message, Color color, float y) {
  float width = GetScreenWidth() - 120.0f;
  Rectangle bar = {60.0f, y, width, 44.0f};
  DrawRectangleRounded(bar, 0.3f, 8, color);
  DrawText(message.c_str(), static_cast<int>(bar.x + 18.0f),
           static_cast<int>(bar.y + 12.0f), 20, RAYWHITE);
}

void DrawBitStrip(const ConversionResult &result, float x, float width,
                  float y) {
  const float height = 60.0f;
  float unit = width / 128.0f;
  float signWidth = unit;
  float exponentWidth = unit * 15.0f;
  float mantissaWidth = width - signWidth - exponentWidth;

  Rectangle signRect = {x, y, signWidth, height};
  Rectangle expRect = {signRect.x + signRect.width, y, exponentWidth, height};
  Rectangle mantRect = {expRect.x + expRect.width, y, mantissaWidth, height};

  const Color signColor = {189, 59, 72, 255};
  const Color expColor = {39, 117, 182, 255};
  const Color mantColor = {56, 161, 105, 255};

  DrawRectangleRec(signRect, signColor);
  DrawRectangleRec(expRect, expColor);
  DrawRectangleRec(mantRect, mantColor);

  string signLabel = "Sign (1): " + result.signBit;
  string expLabel = "Exponent (15): " + result.exponentBits;
  string mantLabel =
      "Mantissa (112): " + result.mantissaBits.substr(0, 32) + "...";

  DrawText(signLabel.c_str(), static_cast<int>(signRect.x),
           static_cast<int>(signRect.y - 24.0f), 18, signColor);
  DrawText(expLabel.c_str(), static_cast<int>(expRect.x),
           static_cast<int>(expRect.y - 24.0f), 18, expColor);
  DrawText(mantLabel.c_str(), static_cast<int>(mantRect.x),
           static_cast<int>(mantRect.y - 24.0f), 18, mantColor);
}

void DrawResultPanels(const ConversionResult &result, float topY) {
  float width = GetScreenWidth() - 120.0f;
  float x = 60.0f;

  Rectangle summary = {x, topY, width, 110.0f};
  DrawRectangleRounded(summary, 0.2f, 8, Color{15, 21, 33, 230});
  DrawRectangleRoundedLines(summary, 0.2f, 8, Color{60, 80, 110, 255});

  string line1 = "Integer bits: " + result.integerBits;
  string line2 = "Exponent (base 2): " + std::to_string(result.exponentValue);
  DrawText(line1.c_str(), static_cast<int>(summary.x + 20.0f),
           static_cast<int>(summary.y + 24.0f), 22, RAYWHITE);
  DrawText(line2.c_str(), static_cast<int>(summary.x + 20.0f),
           static_cast<int>(summary.y + 60.0f), 22, RAYWHITE);

  DrawBitStrip(result, x, width, summary.y + summary.height + 40.0f);

  Rectangle bitsPanel = {x, summary.y + summary.height + 120.0f, width,
                         static_cast<float>(GetScreenHeight()) -
                             (summary.y + summary.height + 160.0f)};
  bitsPanel.height = std::max(140.0f, bitsPanel.height);
  DrawRectangleRounded(bitsPanel, 0.2f, 8, Color{12, 16, 26, 220});
  DrawRectangleRoundedLines(bitsPanel, 0.2f, 8, Color{70, 90, 120, 255});

  DrawText("IEEE 754 (128-bit)", static_cast<int>(bitsPanel.x + 16.0f),
           static_cast<int>(bitsPanel.y + 16.0f), 22, RAYWHITE);

  vector<string> lines = BuildBitLines(result.ieeeBits);
  float textY = bitsPanel.y + 50.0f;
  for (const string &line : lines) {
    DrawText(line.c_str(), static_cast<int>(bitsPanel.x + 16.0f),
             static_cast<int>(textY), 20, Color{200, 210, 225, 255});
    textY += 28.0f;
    if (textY > bitsPanel.y + bitsPanel.height - 30.0f) {
      break;
    }
  }
}

void DrawBackground() {
  int width = GetScreenWidth();
  int height = GetScreenHeight();
  Color top = {7, 11, 19, 255};
  Color bottom = {25, 39, 63, 255};
  DrawRectangleGradientV(0, 0, width, height, top, bottom);
  DrawRectangleGradientV(0, 0, width, height / 2, Color{0, 0, 0, 0},
                         Color{0, 0, 0, 40});
}

} // namespace

void RunGuiMode() {
  const int initialWidth = 1200;
  const int initialHeight = 720;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(initialWidth, initialHeight, "IEEE 754 Quad Converter");
  SetTargetFPS(60);

  string input;
  const size_t maxInputLength = 64;
  bool inputActive = false;
  bool requestConversion = false;
  ConversionResult lastResult;
  bool hasResult = false;
  string statusText = "Type a decimal number and press Convert.";
  Color statusColor = {40, 52, 75, 220};
  const Color buttonColor = {70, 108, 190, 255};

  while (!WindowShouldClose()) {
    Rectangle inputBox = {60.0f, 120.0f, GetScreenWidth() - 120.0f, 64.0f};
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      inputActive = CheckCollisionPointRec(mouse, inputBox);
    }

    if (inputActive) {
      int key = GetCharPressed();
      while (key > 0) {
        if (key >= 32 && key <= 126 && input.length() < maxInputLength) {
          input.push_back(static_cast<char>(key));
        }
        key = GetCharPressed();
      }

      if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
        input.pop_back();
      }

      if (IsKeyPressed(KEY_ENTER)) {
        requestConversion = true;
      }
    }

    Rectangle convertBtn = {60.0f, 200.0f, 170.0f, 48.0f};
    Rectangle clearBtn = {convertBtn.x + convertBtn.width + 12.0f, 200.0f,
                          140.0f, 48.0f};

    bool convertClicked = DrawButton(convertBtn, "Convert", buttonColor);
    bool clearClicked =
        DrawButton(clearBtn, "Clear", Color{120, 120, 120, 255});

    if (convertClicked || requestConversion) {
      lastResult = ConvertToIEEE(input);
      hasResult = lastResult.success;
      statusText = lastResult.success ? "Conversion successful."
                                      : lastResult.errorMessage;
      statusColor = lastResult.success ? Color{50, 120, 90, 230}
                                       : Color{160, 60, 70, 230};
      requestConversion = false;
    }

    if (clearClicked) {
      input.clear();
      hasResult = false;
      statusText = "Type a decimal number and press Convert.";
      statusColor = Color{40, 52, 75, 220};
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawBackground();

    DrawText("IEEE 754 Quad Converter", 60, 40, 36, RAYWHITE);
    DrawText("Built with raylib for instant feedback", 60, 84, 20,
             Color{180, 190, 210, 255});

    DrawInputField(inputBox, input, inputActive);

    DrawStatusBar(statusText, statusColor, 270.0f);

    if (hasResult) {
      DrawResultPanels(lastResult, 330.0f);
    } else {
      Rectangle placeholder = {60.0f, 330.0f, GetScreenWidth() - 120.0f,
                               GetScreenHeight() - 380.0f};
      placeholder.height = std::max(140.0f, placeholder.height);
      DrawRectangleRounded(placeholder, 0.2f, 8, Color{10, 15, 24, 200});
      DrawRectangleRoundedLines(placeholder, 0.2f, 8, Color{55, 70, 96, 255});
      DrawText("Results will appear here",
               static_cast<int>(placeholder.x + 20.0f),
               static_cast<int>(placeholder.y + 24.0f), 22,
               Color{185, 195, 210, 255});
      DrawText("Press Convert to visualize the IEEE 754 breakdown.",
               static_cast<int>(placeholder.x + 20.0f),
               static_cast<int>(placeholder.y + 60.0f), 20,
               Color{120, 135, 155, 255});
    }

    EndDrawing();
  }

  CloseWindow();
}

} // namespace gui
