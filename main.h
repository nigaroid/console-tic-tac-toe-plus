#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#pragma warning(disable: 4996)

#define GRID_SIZE_X_MIN 3
#define GRID_SIZE_X_MAX 10
#define GRID_SIZE_Y_MIN 3
#define GRID_SIZE_Y_MAX 10
#define GRID_SIZE_MIN (GRID_SIZE_Y_MIN * GRID_SIZE_X_MIN)
#define GRID_SIZE_MAX (GRID_SIZE_Y_MAX * GRID_SIZE_X_MAX)

#define GRID_BUFFER_SIZE_X_MIN (GRID_SIZE_X_MIN * 4 + 1)
#define GRID_BUFFER_SIZE_X_MAX (GRID_SIZE_X_MAX * 4 + 1)
#define GRID_BUFFER_SIZE_Y_MIN (GRID_SIZE_Y_MIN * 2 + 1)
#define GRID_BUFFER_SIZE_Y_MAX (GRID_SIZE_Y_MAX * 2 + 1)
#define GRID_BUFFER_SIZE_MIN (GRID_BUFFER_SIZE_Y_MIN * GRID_BUFFER_SIZE_X_MIN)
#define GRID_BUFFER_SIZE_MAX (GRID_BUFFER_SIZE_Y_MAX * GRID_BUFFER_SIZE_X_MAX)

#define THRESHOLD_MIN 3
#define THRESHOLD_MAX 10

#define PLAYER_COUNT_MIN 2
#define PLAYER_COUNT_MAX 10

#define GRID_TURN_BUFFER_SIZE_X_MIN (PLAYER_COUNT_MIN * 4 + 1)
#define GRID_TURN_BUFFER_SIZE_X_MAX (PLAYER_COUNT_MAX * 4 + 1)
#define GRID_TURN_BUFFER_SIZE_Y_MIN (1 * 2 + 1)
#define GRID_TURN_BUFFER_SIZE_Y_MAX (1 * 2 + 1)
#define GRID_TURN_BUFFER_SIZE_MIN (GRID_TURN_BUFFER_SIZE_Y_MIN * GRID_TURN_BUFFER_SIZE_X_MIN)
#define GRID_TURN_BUFFER_SIZE_MAX (GRID_TURN_BUFFER_SIZE_Y_MAX * GRID_TURN_BUFFER_SIZE_X_MAX)

#define MOVE_COUNT_MIN GRID_SIZE_MIN
#define MOVE_COUNT_MAX GRID_SIZE_MAX

#define UNKNOWN_CONVERSION_SPECIFIER "\n" "~   Unknown conversion specifier. " "\n"
#define FEATURE_WORK_IN_PROGRESS     "\n" "~   This feature is a work in progress!" "\n"
#define TOO_FEW_ARGUMENTS            "!   Too few arguments, please try again: "
#define TOO_MANY_ARGUMENTS           "!   Too many arguments, please try again: "
#define ARGUMENT_TOO_LONG            "!   Argument too long, please try again: "
#define INVALID_NUMBER_FORMAT        "!   Invalid number format, please try again: "
#define INVALID_OPTION               "!   Invalid option, please try again: "
#define INVALID_SYMBOL               "!   Invalid symbol, please try again: "
#define VALUE_OUT_OF_RANGE           "!   Value out of the provided range, please try again: "
#define COORDINATES_OUT_OF_GRID      "!   Coordinates outside the grid, please try again: "
#define CELL_NOT_EMPTY               "!   Cell not empty, please try again: "
#define SYMBOL_NOT_FOUND             "!   Symbol not found, please try again: "
#define NO_MOVES_LEFT                "\n" "~   No moves left to undo." "\n\n"

#define TRANSITION_START()   printf("\0337\033[?25l")
#define TRANSITION_ITERATE() printf("\0338")
#define TRANSITION_END()     printf("\033[?25h")
#define CONSOLE_CLEAR()      system("cls")

#define GOTO_TERNARY(condition, layer_true, layer_false) do { if (condition) goto layer_true; goto layer_false; } while (false)

#define RANDOM_INT(max) (rand() % (max))

#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef enum _Cell
{
    CELL_NIL = ' ', CELL_TIE = '@', CELL_ANY = '?',
    CELL_A = 'A', CELL_B = 'B', CELL_C = 'C', CELL_D = 'D',
    CELL_E = 'E', CELL_F = 'F', CELL_G = 'G', CELL_H = 'H',
    CELL_I = 'I', CELL_J = 'J', CELL_K = 'K', CELL_L = 'L',
    CELL_M = 'M', CELL_N = 'N', CELL_O = 'O', CELL_P = 'P',
    CELL_Q = 'Q', CELL_R = 'R', CELL_S = 'S', CELL_T = 'T',
    CELL_U = 'U', CELL_V = 'V', CELL_W = 'W', CELL_X = 'X',
    CELL_Y = 'Y', CELL_Z = 'Z',
} Cell;

typedef struct _Vector2UInt32
{
    uint32_t x;
    uint32_t y;
} Vector2UInt32;

typedef enum _GameMode
{
    MODE_CLASSIC             = 0,
    MODE_RANDOM_SYMBOLS      = 1,
    MODE_PLAYERS_VERSUS_BOTS = 2,
    MODE_COUNT
} GameMode;

int32_t console_scan_input(const char *format, ...);

void game_welcome(void);

void game_loading_screen(void);

bool game_main_menu(void);

bool game_match_end(Cell winner);

void game_scan_mode(void);

void game_scan_size(void);

void game_scan_win_condition(void);

void game_scan_player_count(void);

void game_scan_player_symbols(void);

void game_switch_theme(void);

void game_terminate(void);

uint32_t grid_initialize(void);

uint32_t grid_buffer_initialize(void);

uint32_t grid_turn_buffer_initialize(void);

void grid_turn_buffer_assign_symbols(void);

void grid_turn_buffer_assign_question_marks(void);

wchar_t grid_get_buffer_character(uint32_t x, uint32_t y);

wchar_t grid_get_turn_buffer_character(uint32_t x, uint32_t y);

int32_t grid_get_index_of_symbol(Cell symbol);

Cell grid_get_player(uint32_t turn);

void grid_print(void);

void grid_print_match_info(uint32_t move_count);

void grid_print_player(void);

uint32_t grid_scan_position(Vector2UInt32 *selected);

bool grid_scan_position_confirm(Vector2UInt32 *selected);

void grid_select_preview(Vector2UInt32 selected);

void grid_deselect_preview(Vector2UInt32 selected);

void grid_select(Cell player, Vector2UInt32 selected, uint32_t *move_count);

void grid_deselect(uint32_t *move_count);

bool grid_undo(uint32_t *move_count, uint32_t *turn, uint32_t player_count);

uint32_t grid_turn_switch_first(void);

uint32_t grid_turn_switch_next(uint32_t turn, uint32_t player_count);

uint32_t grid_turn_switch_previous(uint32_t turn, uint32_t player_count);

bool grid_test_cells(Vector2UInt32 selected);

#endif
