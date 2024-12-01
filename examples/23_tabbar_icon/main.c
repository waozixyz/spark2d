#include "spark2d.h"
#include "spark_ui.h"
#include "spark_graphics.h"
#include <stdio.h>

static SparkTabBar* tabbar;
static SparkImage* home_image;

static void on_tab_changed(int tab_index) {
    printf("Switched to tab %d\n", tab_index);
}

void load(void) {
    // Initialize theming system
    spark_theme_init();
    
    // Simple preset usage
    SparkTheme* theme = spark_theme_create_from_preset(SPARK_THEME_PRESET_DARK);
    spark_theme_set_current(theme);
    spark_theme_free(theme);


    // Set responsive mode with aspect ratio maintenance
    spark_window_set_mode(SPARK_WINDOW_MODE_RESPONSIVE);
    spark_window_set_scale_mode(SPARK_SCALE_NONE);

    // Create tabbar with minimal configuration
    SparkTabBarBuilder builder = {
        .position = SPARK_TAB_BOTTOM
    };
    
    tabbar = spark_ui_tabbar_build(&builder);
    if (!tabbar) {
        fprintf(stderr, "Failed to create tab bar\n");
        return;
    }

    // Load image
    home_image = spark_graphics_load_image("assets/home.svg");
    if (!home_image) {
        fprintf(stderr, "Failed to load home image\n");
        return;
    }

    // Configure mixed text/image tabs
    SparkTabConfig tabs[] = {
        { .text = "Back" },
        { .image = home_image },
        { .text = "Next" }
    };

    SparkTabGroup nav_group = {
        .tabs = tabs,
        .tab_count = 3
    };
    
    // Add tabs and set callback
    spark_ui_tabbar_add_group(tabbar, &nav_group);
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
    
    if (home_image) {
        spark_graphics_image_free(home_image);
        home_image = NULL;
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