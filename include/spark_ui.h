// spark_ui.h
#ifndef SPARK_UI_H
#define SPARK_UI_H

typedef struct SparkButton SparkButton;
typedef void (*SparkButtonCallback)(void);

// Create a button
SparkButton* spark_ui_button_new(float x, float y, float width, float height, const char* text);

// Set the button's callback
void spark_ui_button_set_callback(SparkButton* button, SparkButtonCallback callback);

// Update the button (handles input)
void spark_ui_button_update(SparkButton* button);

// Draw the button
void spark_ui_button_draw(SparkButton* button);

// Destroy the button
void spark_ui_button_free(SparkButton* button);

#endif
