#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkTabBar* tabbar;

// Define the callback function before using it
static void on_tab_changed(int tab_index) {
    printf("Switched to tab %d\n", tab_index);
}

void load(void) {
    // Set responsive mode with aspect ratio maintenance
    spark_window_set_mode(SPARK_WINDOW_MODE_RESPONSIVE);
    spark_window_set_scale_mode(SPARK_SCALE_MAINTAIN_ASPECT);

    // Create UI elements in logical coordinates
    tabbar = spark_ui_tabbar_new(0, 550, 800, 50);
    if (!tabbar) {
        fprintf(stderr, "Failed to create tab bar\n");
        return;
    }

    // Changed from spark_ui_tabbar_add_tab to spark_ui_tabbar_add_text_tab
    spark_ui_tabbar_add_text_tab(tabbar, "Home");
    spark_ui_tabbar_add_text_tab(tabbar, "Profile");
    spark_ui_tabbar_add_text_tab(tabbar, "Settings");
    spark_ui_tabbar_add_text_tab(tabbar, "Help");

    spark_ui_tabbar_set_callback(tabbar, on_tab_changed);
}

void update(float dt) {
    (void)dt;
    if (tabbar) {
        spark_ui_tabbar_update(tabbar);
    }
}

void draw(void) {
    spark_graphics_set_color(0.2f, 0.2f, 0.2f);
    spark_graphics_clear();
    if (tabbar) {
        spark_ui_tabbar_draw(tabbar);
    }
    spark_graphics_present();
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
    spark_set_update(update);
    spark_set_draw(draw);
    spark_run();
    cleanup();
    spark_quit();
    return 0;
}