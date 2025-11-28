# app/utils/constants.py
import os

class AppConstants:
    WINDOW_TITLE = "KIDSVT UI - Симуляция неисправностей RAM"
    WINDOW_WIDTH = 1300
    WINDOW_HEIGHT = 800
    
    # Sizes by dafault
    DEFAULT_WORD_COUNT = 16
    BITS_PER_WORD = 16
    
    # Visualisation
    GRID_COLS = 16
    DEFAULT_CELL_SIZE = 40

    # Paths
    TEST_FILES_PATH = r"./res"
    ICON_PATH = r"icon.ico" 

    # Colors (HEX)
    COLOR_BG_DEFAULT = "#FFFFFF"
    COLOR_BG_ACTIVE = "#FFFACD"  
    COLOR_BG_SUCCESS = "#90EE90"
    
    COLOR_BG_ERROR = "#FF3333"    
    
    COLOR_TEXT_DEFAULT = "#000000"