#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkDropdown* dropdown;

// Define the callback function before using it
static void on_item_selected(int selected_index, const char* selected_item, void* user_data) {
    (void)user_data; // Unused in this example
    printf("Selected item %d: %s\n", selected_index, selected_item);
}

void load(void) {
    // Create a dropdown with position, size
    dropdown = spark_ui_dropdown_new(50, 50, 200, 30);
    
    if (!dropdown) {
        fprintf(stderr, "Failed to create dropdown\n");
        return;
    }

    // Add some items to the dropdown
    spark_ui_dropdown_add_item(dropdown, "Option 1");
    spark_ui_dropdown_add_item(dropdown, "Option 2");
    spark_ui_dropdown_add_item(dropdown, "Option 3");
    spark_ui_dropdown_add_item(dropdown, "Option 4");
    
    // Optional: set a callback for when an item is selected
    spark_ui_dropdown_set_callback(dropdown, on_item_selected, NULL);
    

}


void draw(void) {
    // Clear background
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();

    // Draw some example text to show current selection
    char buffer[256];
    int selected_index = spark_ui_dropdown_get_selected(dropdown);
    const char* selected_text = spark_ui_dropdown_get_selected_text(dropdown);
    
    snprintf(buffer, sizeof(buffer), "Selected: %s (index: %d)", 
             selected_text ? selected_text : "None", selected_index);
    
    spark_graphics_set_color(1.0f, 1.0f, 1.0f);
    spark_graphics_print(buffer, 50, 100);

    spark_graphics_present();
}

void cleanup(void) {
    if (dropdown) {
        spark_ui_dropdown_free(dropdown);
        dropdown = NULL;
    }
    spark_graphics_cleanup();
}

int main(void) {
    if (!spark_init("Dropdown Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }

    spark_set_load(load);
    spark_run();
    
    cleanup();
    spark_quit();
    return 0;
}
