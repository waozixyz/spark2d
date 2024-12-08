#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkTabBar* tabbar;

// Define the callback function before using it
static void on_tab_changed(int tab_index) {
    printf("Switched to tab %d\n", tab_index);
}
// Example usage - only specify what you need
void load(void) {
    // Minimal builder with just required fields
    SparkTabBarBuilder builder = {
        .position = 0,  // Only required field
    };
    
    tabbar = spark_ui_tabbar_build(&builder);
    if (!tabbar) {
        fprintf(stderr, "Failed to create tab bar\n");
        return;
    }

    // Simple tab configs - only specify text
    SparkTabConfig tabs[] = {
        { .text = "Home" },
        { .text = "Profile" },
        { .text = "Settings" },
        { .text = "Help" }
    };

    SparkTabGroup main_group = {
        .tabs = tabs,
        .tab_count = 4
    };
    
    spark_ui_tabbar_add_group(tabbar, &main_group);

    // Optional: only set callback if needed
    spark_ui_tabbar_set_callback(tabbar, on_tab_changed);
}


void cleanup(void) {
    if (tabbar) {
        spark_ui_tabbar_free(tabbar);
        tabbar = NULL;
    }
    spark_graphics_cleanup();
}

int main(void) {
    if (!spark_init("Tab Bar Example", 800, 600)) {
        fprintf(stderr, "Failed to initialize Spark2D\n");
        return 1;
    }
    spark_set_load(load);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}