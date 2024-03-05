#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <locale.h>

static uint32_t grid_size_x = GRID_SIZE_X_MIN, grid_size_y = GRID_SIZE_Y_MIN;
static Cell grid[GRID_SIZE_MAX];

static uint32_t grid_buffer_size_x = GRID_BUFFER_SIZE_X_MIN, grid_buffer_size_y = GRID_BUFFER_SIZE_Y_MIN;
static wchar_t grid_buffer[GRID_BUFFER_SIZE_MAX];

static GameMode game_mode = MODE_CLASSIC;

static uint32_t threshold = THRESHOLD_MIN;

static uint32_t player_count = PLAYER_COUNT_MIN;
static Cell players[PLAYER_COUNT_MAX] = { CELL_X, CELL_O, CELL_Y, CELL_Z, CELL_A, CELL_B, CELL_C, CELL_D, CELL_E, CELL_F };

static uint32_t grid_turn_buffer_size_x = GRID_TURN_BUFFER_SIZE_X_MIN, grid_turn_buffer_size_y = GRID_TURN_BUFFER_SIZE_Y_MIN;
static wchar_t grid_turn_buffer[GRID_TURN_BUFFER_SIZE_MAX];

static Vector2UInt32 moves[MOVE_COUNT_MAX];

static bool light_theme = false;

int main(void)
{
#if !defined(_WIN32) && !defined(_WIN64)
    /*
        Just as any other developer might do in this scenario, we explicitly mark the game
        as Windows-exclusive because we prefer not to invest effort in making it portable.

         - Regards.
    */

    return 1;
#endif

    game_welcome();

main_menu:
    while (true)
    {
        game_loading_screen();
        if (game_main_menu())
            break;

game_process:
        while (true)
        {
            uint32_t cell_count = grid_initialize();
            uint32_t grid_buffer_size = grid_buffer_initialize();
            uint32_t grid_turn_buffer_size = grid_turn_buffer_initialize();
            uint32_t turn = grid_turn_switch_first();
            uint32_t move_count = 0;
            Vector2UInt32 selected = { 0 };

            switch (game_mode)
            {
                case MODE_CLASSIC:
                    grid_turn_buffer_assign_symbols();
                    break;
                case MODE_RANDOM_SYMBOLS:
                    grid_turn_buffer_assign_question_marks();
                    break;
                default:
                    return -1;
            }

game_match_iteration:
            while (true)
            {
                uint32_t control = 0;
                Cell player = grid_get_player(turn);

                CONSOLE_CLEAR();
                grid_print_match_info(move_count);
                grid_print();
                grid_print_player();

                do
                {
                    switch (control = grid_scan_position(&selected))
                    {
                        case 't':
                            if (game_match_end(CELL_TIE))
                                goto game_process;
                        case 'q':
                            goto main_menu;
                        case 'z':
                            if (grid_undo(&move_count, &turn, player_count))
                                goto game_match_iteration;
                    }
                }
                while (control);

                do
                {
                    CONSOLE_CLEAR();
                    grid_select_preview(selected);
                    grid_print_match_info(move_count);
                    grid_print();
                    grid_print_player();
                    grid_deselect_preview(selected);
                }
                while (!grid_scan_position_confirm(&selected));

                grid_select(player, selected, &move_count);
                turn = grid_turn_switch_next(turn, player_count);

                if (grid_test_cells(selected))
                    GOTO_TERNARY(game_match_end(player), game_process, main_menu);

                if (move_count == cell_count)
                    GOTO_TERNARY(game_match_end(CELL_TIE), game_process, main_menu);
            }
        }
    }

    game_terminate();

    return 0;
}

int32_t console_scan_input(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char type;
    int32_t variable_count = 0, c = getchar();

    if (c == EOF)
        return 0;

    while (true)
    {
        while (c == ' ' || c == '\t')
            c = getchar();

        if (c == '\n')
            return variable_count;

        ++variable_count;

        if (!(type = *format++))
            break;

        switch (type)
        {
            uint32_t length;
            uint32_t ui;
            char chr;

            case 'u':
                if ((ui = c - '0') > 9u)
                {
                    printf(INVALID_NUMBER_FORMAT);
                    goto error;
                }

                for (length = 1; (c = getchar()) - '0' <= 9u; length += (ui != 0))
                    ui = ui * 10 + c - '0';

                if (c != ' ' && c != '\t' && c != '\n')
                {
                    printf(INVALID_NUMBER_FORMAT);
                    goto error;
                }

                if (length > 6)
                {
                    printf(ARGUMENT_TOO_LONG);
                    goto error;
                }

                *va_arg(args, uint32_t*) = ui;
                break;

            case 'c':
                chr = c;
                if ((c = getchar()) != ' ' && c != '\t' && c != '\n')
                {
                    printf(ARGUMENT_TOO_LONG);
                    goto error;
                }

                *va_arg(args, char*) = chr;
                break;

            case 'C':
                chr = c;
                if ((ui = c - '0') > 9u)
                {
                    if ((c = getchar()) == ' ' || c == '\t' || c == '\n')
                    {
                        *va_arg(args, char*) = chr;
                        *va_arg(args, uint32_t*) = 1;
                        break;
                    }

                    printf(ARGUMENT_TOO_LONG);
                    goto error;
                }

                for (length = 1; (c = getchar()) - '0' <= 9u; length += (ui != 0))
                    ui = ui * 10 + c - '0';

                if (c != ' ' && c != '\t' && c != '\n')
                {
                    printf(INVALID_NUMBER_FORMAT);
                    goto error;
                }

                if (length > 6)
                {
                    printf(ARGUMENT_TOO_LONG);
                    goto error;
                }

                *va_arg(args, uint32_t*) = ui;
                *va_arg(args, uint32_t*) = 2;
                break;

            case ' ':
                while (c != '\n' && c != 26)
                    c = getchar();
                break;

            default:
                printf(UNKNOWN_CONVERSION_SPECIFIER);
                goto error;
        }
    }

    if (c != '\n')
    {
        printf(TOO_MANY_ARGUMENTS);
error:
        while (c != '\n' && c != 26)
            c = getchar();

        va_end(args);
        return -variable_count;
    }

    va_end(args);
    return variable_count;
}

void game_welcome(void)
{
    srand(time(NULL));
    setlocale(LC_CTYPE, "en_US.UTF-8");

    TRANSITION_START();
    for (uint32_t bar = 0; bar <= 23 + 21 + 25; ++bar)
    {
        TRANSITION_ITERATE();
        wprintf(L"\n"
            L"    ╔═══════════════════════════════════╗" L"\n"
            L"    ║ ┌───────────────────────────────┐ ║" L"\n"
            L"    ║ │                               │ ║" L"\n"
            L"    ║ │    %.*s%.*s%.*s    │ ║"            L"\n"
            L"    ║ │     %.*s%.*s%.*s     │ ║"          L"\n"
            L"    ║ │   %.*s%.*s%.*s   │ ║"              L"\n"
            L"    ║ │                               │ ║" L"\n"
            L"    ║ └───────────────────────────────┘ ║" L"\n"
            L"    ╚═══════════════════════════════════╝" L"\n\n",
            (bar >= 0) ? (bar > 23) ? 0 : (23 - bar) / 2 : 11, L"           ",
            (bar >= 0) ? (bar > 23) ? 23 : bar : 0, L"Welcome, fellow player!",
            (bar >= 0) ? (bar > 23) ? 0 : (24 - bar) / 2 : 12, L"            ",
            (bar >= 23) ? (bar > 23 + 21) ? 0 : (23 + 21 - bar) / 2 : 10, L"          ",
            (bar >= 23) ? (bar > 23 + 21) ? 21 : bar - 23 : 0, L"We hope you enjoy it.",
            (bar >= 23) ? (bar > 23 + 21) ? 0 : (24 + 21 - bar) / 2 : 11, L"           ",
            (bar >= 23 + 21) ? (bar > 23 + 21 + 25) ? 0 : (23 + 21 + 25 - bar) / 2 : 12, L"            ",
            (bar >= 23 + 21) ? (bar > 23 + 21 + 25) ? 25 : bar - 23 - 21 : 0, L"Press enter to proceed...",
            (bar >= 23 + 21) ? (bar > 23 + 21 + 25) ? 0 : (24 + 21 + 25 - bar) / 2 : 13, L"             ");
        _sleep(40);
    }
    TRANSITION_END();

    console_scan_input(" ");
}

void game_loading_screen(void)
{
    CONSOLE_CLEAR();

    TRANSITION_START();
    for (uint32_t bar = 0; bar <= 43 + 5 * 5; ++bar)
    {
        TRANSITION_ITERATE();
        wprintf(L"\n"
            L"    ╔═════════════════════════════════════════════════╗" L"\n"
            L"    ║ ┌─────────────────────────────────────────────┐ ║" L"\n"
            L"    ║ │ %-43.*s │ ║" L"\n"
            L"    ║ │ %-43.*s │ ║" L"\n"
            L"    ║ │ %-43.*s │ ║" L"\n"
            L"    ║ │ %-43.*s │ ║" L"\n"
            L"    ║ │ %-43.*s │ ║" L"\n"
            L"    ║ └─────────────────────────────────────────────┘ ║" L"\n"
            L"    ╚═════════════════════════════════════════════════╝" L"\n",
            (bar >= 1 * 5) ? (bar > 43 + 1 * 5) ? 43 : bar - 1 * 5 : 0, L"███     ███   ███       ███   ███       ███",
            (bar >= 2 * 5) ? (bar > 43 + 2 * 5) ? 43 : bar - 2 * 5 : 0, L" █   █  █      █    █   █      █   ███  █  ",
            (bar >= 3 * 5) ? (bar > 43 + 3 * 5) ? 43 : bar - 3 * 5 : 0, L" █      █      █   █ █  █      █   █ █  ███",
            (bar >= 4 * 5) ? (bar > 43 + 4 * 5) ? 43 : bar - 4 * 5 : 0, L" █   █  █      █   ███  █      █   █ █  █  ",
            (bar >= 5 * 5) ? (bar > 43 + 5 * 5) ? 43 : bar - 5 * 5 : 0, L" █   █  ███    █   █ █  ███    █   ███  ███");
        _sleep(20);
    }
    _sleep(200);
    TRANSITION_END();
}

bool game_main_menu(void)
{
info:
    printf("\n"
        "#   Menu options:" "\n"
        "#    `P` - Start the game." "\n"
        "#    `G` - Choose the game mode." "\n"
        "#    `D` - Choose the size of the grid." "\n"
        "#    `W` - Choose the win condition." "\n"
        "#    `N` - Choose the number of players." "\n"
        "#    `S` - Choose the player symbols." "\n"
        "#    `M` - Switch the theme." "\n"
        "#    `C` - Clear the screen." "\n"
        "#    `Q` - Quit the game." "\n");

prompt:
    printf("\n" ">   Enter one of the menu options from above: ");

    while (true)
    {
        char op;
        int32_t code = console_scan_input("c", &op);

        if (code <= 0)
        {
            if (code == 0)
                printf(TOO_FEW_ARGUMENTS);
            continue;
        }

        switch (op | 32)
        {
            case 'p':
                return false;
            case 'g':
                game_scan_mode();
                goto prompt;
            case 'd':
                game_scan_size();
                goto prompt;
            case 'w':
                game_scan_win_condition();
                goto prompt;
            case 'n':
                game_scan_player_count();
                goto prompt;
            case 's':
                game_scan_player_symbols();
                goto prompt;
            case 'm':
                game_switch_theme();
                goto prompt;
            case 'c':
                CONSOLE_CLEAR();
                wprintf(L"\n"
                    L"    ╔═════════════════════════════════════════════════╗" L"\n"
                    L"    ║ ┌─────────────────────────────────────────────┐ ║" L"\n"
                    L"    ║ │ ███     ███   ███       ███   ███       ███ │ ║" L"\n"
                    L"    ║ │  █   █  █      █    █   █      █   ███  █   │ ║" L"\n"
                    L"    ║ │  █      █      █   █ █  █      █   █ █  ███ │ ║" L"\n"
                    L"    ║ │  █   █  █      █   ███  █      █   █ █  █   │ ║" L"\n"
                    L"    ║ │  █   █  ███    █   █ █  ███    █   ███  ███ │ ║" L"\n"
                    L"    ║ └─────────────────────────────────────────────┘ ║" L"\n"
                    L"    ╚═════════════════════════════════════════════════╝" L"\n");
                goto info;
            case 'q':
                return true;
            default:
                printf(INVALID_OPTION);
                continue;
        }
    }
}

bool game_match_end(Cell winner)
{
    static       wchar_t win_message[] = L"Player ` ` won!";
    static const wchar_t tie_message[] = L"Everybody tied!";

    win_message[8] = winner;
    const wchar_t *message = winner == CELL_TIE ? tie_message : win_message;

    CONSOLE_CLEAR();

    TRANSITION_START();
    for (uint32_t bar = 0; bar <= 15; ++bar)
    {
        TRANSITION_ITERATE();
        wprintf(L"\n"
            L"    ╔═════════════════════════╗" L"\n"
            L"    ║ ┌─────────────────────┐ ║" L"\n"
            L"    ║ │                     │ ║" L"\n"
            L"    ║ │   %.*s%.*s%.*s   │ ║" L"\n"
            L"    ║ │                     │ ║" L"\n"
            L"    ║ └─────────────────────┘ ║" L"\n"
            L"    ╚═════════════════════════╝" L"\n",
            (15 - bar) / 2, L"       ",
            bar, message,
            (16 - bar) / 2, L"        ");
        _sleep(40);
    }
    TRANSITION_END();

    grid_print();

    printf("\n" ">   Enter blank to switch back to the main menu, `R` to play again: ");

    while (true)
    {
        char op;
        int32_t code = console_scan_input("c", &op);

        if (code <= 0)
        {
            if (code == 0)
                return false;

            continue;
        }

        if ((op | 32) != 'r')
        {
            printf(INVALID_OPTION);
            continue;
        }

        return true;
    }
}

void game_scan_mode(void)
{
    printf("\n"
        "#   Current mode: %u" "\n"
        "#    `0` - Classic" "\n"
        "#    `1` - Random Symbols" "\n"
        "~    `2` - Players Versus Bots" "\n",
        game_mode);

prompt:
    printf("\n" ">   Enter one of the game modes from above: ");

    while (true)
    {
        uint32_t mode;
        int32_t code = console_scan_input("u", &mode);

        if (code <= 0)
        {
            if (code == 0)
                return;

            continue;
        }

        if (MODE_COUNT <= mode)
        {
            printf(VALUE_OUT_OF_RANGE);
            continue;
        }

        if (mode == MODE_PLAYERS_VERSUS_BOTS)
        {
            printf(FEATURE_WORK_IN_PROGRESS);
            goto prompt;
        }

        game_mode = mode;
        return;
    }
}

void game_scan_size(void)
{
    printf("\n"
        "#   Current dimensions: %ux%u" "\n"
        ">   Enter the new dimension(s) of the grid from 3 to up 10 as either `n` or `n m`: ",
        grid_size_x, grid_size_y);

    while (true)
    {
        uint32_t size_x, size_y;
        int32_t code = console_scan_input("uu", &size_x, &size_y);

        if (code <= 0)
        {
            if (code == 0)
                return;

            continue;
        }

        if (code == 1)
            size_y = size_x;

        if (size_x < GRID_SIZE_X_MIN || GRID_SIZE_X_MAX < size_x ||
            size_y < GRID_SIZE_Y_MIN || GRID_SIZE_Y_MAX < size_y)
        {
            printf(VALUE_OUT_OF_RANGE);
            continue;
        }

        grid_buffer_size_x = (grid_size_x = size_x) * 4 + 1;
        grid_buffer_size_y = (grid_size_y = size_y) * 2 + 1;
        uint32_t grid_size_min = min(size_x, size_y);
        threshold = min(threshold, grid_size_min);
        uint32_t allowable_player_count = (grid_size_x * grid_size_y - 1) / (threshold - 1);
        allowable_player_count = min(allowable_player_count - 1, 10);
        player_count = min(player_count, allowable_player_count);
        return;
    }
}

void game_scan_win_condition(void)
{
    uint32_t grid_size_min = min(grid_size_x, grid_size_y);

    printf("\n"
        "#   Current winning condition: %u" "\n"
        ">   Enter the new threshold from 3 to up the current grid size (%u) as `n`: ",
        threshold, grid_size_min);

    while (true)
    {
        uint32_t t;
        int32_t code = console_scan_input("u", &t);

        if (code <= 0)
        {
            if (code == 0)
                return;

            continue;
        }

        if (t < THRESHOLD_MIN || grid_size_min < t)
        {
            printf(VALUE_OUT_OF_RANGE);
            continue;
        }

        threshold = t;
        uint32_t allowable_player_count = (grid_size_x * grid_size_y - 1) / (threshold - 1);
        allowable_player_count = min(allowable_player_count - 1, 10);
        player_count = min(player_count, allowable_player_count);
        return;
    }
}

void game_scan_player_count(void)
{
    uint32_t allowable_player_count = (grid_size_x * grid_size_y - 1) / (threshold - 1);
    allowable_player_count = min(allowable_player_count - 1, 10);

    printf("\n"
        "#   Current number of players: %u" "\n"
        ">   Enter the new player count from 2 to up the allowable number (%u) as `n`: ",
        player_count, allowable_player_count);

    while (true)
    {
        uint32_t count;
        int32_t code = console_scan_input("u", &count);

        if (code <= 0)
        {
            if (code == 0)
                return;

            continue;
        }

        if (count < PLAYER_COUNT_MIN || allowable_player_count < count)
        {
            printf(VALUE_OUT_OF_RANGE);
            continue;
        }

        player_count = count;
        return;
    }
}

void game_scan_player_symbols(void)
{
    grid_turn_buffer_initialize();

    while (true)
    {
        grid_turn_buffer_assign_symbols();
        printf("\n" "#   Current player symbols:");
        grid_print_player();

        printf(">   Enter the current symbol and its replacement in order as `c n`: ");

        while (true)
        {
            char old, new;
            int32_t code = console_scan_input("cc", &old, &new);

            if (code <= 0)
            {
                if (code == 0)
                    return;

                continue;
            }

            old &= ~32;
            new &= ~32;

            if ((uint32_t)old - 'A' > 26u || (uint32_t)new - 'A' > 26u)
            {
                printf(INVALID_SYMBOL);
                continue;
            }

            int32_t old_index = grid_get_index_of_symbol(old);
            int32_t new_index = grid_get_index_of_symbol(new);

            if (old_index == -1)
            {
                printf(SYMBOL_NOT_FOUND);
                continue;
            }

            if (new_index == -1)
            {
                players[old_index] = new;
            }
            else
            {
                Cell temp = players[old_index];
                players[old_index] = new;
                players[new_index] = temp;
            }

            break;
        }
    }
}

void game_switch_theme(void)
{
    printf((light_theme = !light_theme) ?
        "\033[?5h" "\n" "#   Theme set to light mode." "\n" :
        "\033[?5l" "\n" "#   Theme set to dark mode." "\n");
}

void game_terminate(void)
{
    CONSOLE_CLEAR();

    TRANSITION_START();
    for (uint32_t bar = 0; bar <= 19; ++bar)
    {
        TRANSITION_ITERATE();
        wprintf(L"\n"
            L"    ╔═════════════════════════════╗" L"\n"
            L"    ║ ┌─────────────────────────┐ ║" L"\n"
            L"    ║ │                         │ ║" L"\n"
            L"    ║ │   %.*s%.*s%.*s   │ ║" L"\n"
            L"    ║ │                         │ ║" L"\n"
            L"    ║ └─────────────────────────┘ ║" L"\n"
            L"    ╚═════════════════════════════╝" L"\n\n",
            (19 - bar) / 2, L"         ",
            bar, L"Thanks for playing!",
            (20 - bar) / 2, L"          ");
        _sleep(40);
    }
    TRANSITION_END();
}

uint32_t grid_initialize(void)
{
    uint32_t cell_count = grid_size_x * grid_size_y;
    memset(grid, 0, cell_count * sizeof *grid);
    return cell_count;
}

uint32_t grid_buffer_initialize(void)
{
    for (uint32_t y = 0; y < grid_buffer_size_y; ++y)
        for (uint32_t x = 0; x < grid_buffer_size_x; ++x)
            grid_buffer[y * grid_buffer_size_x + x] = grid_get_buffer_character(x, y);

    return grid_buffer_size_x * grid_buffer_size_y * (uint32_t)(sizeof *grid_buffer);
}

uint32_t grid_turn_buffer_initialize(void)
{
    grid_turn_buffer_size_x = player_count * 4 + 1;

    for (uint32_t y = 0; y < grid_turn_buffer_size_y; ++y)
        for (uint32_t x = 0; x < grid_turn_buffer_size_x; ++x)
            grid_turn_buffer[y * grid_turn_buffer_size_x + x] = grid_get_turn_buffer_character(x, y);

    return grid_turn_buffer_size_x * grid_turn_buffer_size_y * (uint32_t)(sizeof *grid_turn_buffer);
}

void grid_turn_buffer_assign_symbols(void)
{
    for (uint32_t i = 0; i < player_count; ++i)
        grid_turn_buffer[grid_turn_buffer_size_x + i * 4 + 2] = players[i];
}

void grid_turn_buffer_assign_question_marks(void)
{
    for (uint32_t i = 0; i < player_count; ++i)
        grid_turn_buffer[grid_turn_buffer_size_x + i * 4 + 2] = CELL_ANY;
}

wchar_t grid_get_buffer_character(uint32_t x, uint32_t y)
{
    if (x & 3)
    {
        if (y & 1)
            return L' ';

        if ((y == 0 || y == grid_buffer_size_y - 1) && (x & 3) == 2)
            return '0' + x / 4;

        return L'═';
    }

    if (y & 1)
        return x == 0 || x == grid_buffer_size_x - 1 ? '0' + y / 2 : L'║';

    if (y == 0)
        return x ? x == grid_buffer_size_x - 1 ? L'╗' : L'╦' : L'╔';

    if (y == grid_buffer_size_y - 1)
        return x ? x == grid_buffer_size_x - 1 ? L'╝' : L'╩' : L'╚';

    return x ? x == grid_buffer_size_x - 1 ? L'╣' : L'╬' : L'╠';
}

wchar_t grid_get_turn_buffer_character(uint32_t x, uint32_t y)
{
    if (x & 3)
        return y & 1 ? L' ' : L'═';

    if (y & 1)
        return L'║';

    if (y == 0)
        return x ? x == grid_turn_buffer_size_x - 1 ? L'╗' : L'╦' : L'╔';

    return x ? x == grid_turn_buffer_size_x - 1 ? L'╝' : L'╩' : L'╚';
}

int32_t grid_get_index_of_symbol(Cell symbol)
{
    for (uint32_t i = 0; i < PLAYER_COUNT_MAX; ++i)
        if (players[i] == symbol)
            return i;
    return -1;
}

Cell grid_get_player(uint32_t turn)
{
    return game_mode == MODE_RANDOM_SYMBOLS ? players[RANDOM_INT(player_count)] : players[turn];
}

void grid_print(void)
{
    const wchar_t *b = grid_buffer;

    puts("");

    for (uint32_t x = grid_buffer_size_x, y = grid_buffer_size_y; y > 0; b += x, --y)
        wprintf(L"    %.*s\n", x, b);
}

void grid_print_match_info(uint32_t move_count)
{
    printf("\n"
        "#   Win condition: %u" "\n"
        "#   Move count: %u" "\n\n"
        "#    - `Z` to undo a move." "\n"
        "#    - `T` to declare a draw game." "\n"
        "#    - `Q` to quit back to the main menu." "\n",
        threshold, move_count);
}

void grid_print_player(void)
{
    const wchar_t *b = grid_turn_buffer;

    puts("");

    for (uint32_t x = grid_turn_buffer_size_x, y = grid_turn_buffer_size_y; y > 0; b += x, --y)
        wprintf(L"    %.*s\n", x, b);

    puts("");
}

uint32_t grid_scan_position(Vector2UInt32 *selected)
{
    printf(">   Enter the coordinates of the cell as `x y`: ");

    while (true)
    {
        uint32_t x = 0, type, y;
        int32_t code = console_scan_input("Cu", &x, &type, &y);

        if (code <= 0)
        {
            if (code == 0)
                printf(TOO_FEW_ARGUMENTS);
            continue;
        }

        if (code == 1)
        {
            if (type == 2)
            {
                printf(TOO_FEW_ARGUMENTS);
                continue;
            }

            switch (x |= 32)
                case 'z':
                case 't':
                case 'q':
                    return x;

            printf(INVALID_OPTION);
            continue;
        }

        if (type == 1)
        {
            printf(TOO_MANY_ARGUMENTS);
            continue;
        }

        if (grid_size_x <= x || grid_size_y <= y)
        {
            printf(COORDINATES_OUT_OF_GRID);
            continue;
        }

        if (grid[y * grid_size_x + x])
        {
            printf(CELL_NOT_EMPTY);
            continue;
        }

        selected->x = x;
        selected->y = y;
        return 0;
    }
}

bool grid_scan_position_confirm(Vector2UInt32 *selected)
{
    printf(">   Enter blank to confirm, re-enter the coordinates as `x y` to reselect: ");

    while (true)
    {
        uint32_t x, y;
        int32_t code = console_scan_input("uu", &x, &y);

        if (code <= 1)
        {
            if (code == 0)
                return true;

            if (code == 1)
                printf(TOO_FEW_ARGUMENTS);
            continue;
        }

        if (grid_size_x <= x || grid_size_y <= y)
        {
            printf(COORDINATES_OUT_OF_GRID);
            continue;
        }

        if (grid[y * grid_size_x + x])
        {
            printf(CELL_NOT_EMPTY);
            continue;
        }

        selected->x = x;
        selected->y = y;
        return false;
    }
}

void grid_select_preview(Vector2UInt32 selected)
{
    grid_buffer[(selected.y * 2 + 1) * (grid_size_x * 4 + 1) + selected.x * 4 + 2] = L'#';
}

void grid_deselect_preview(Vector2UInt32 selected)
{
    grid_buffer[(selected.y * 2 + 1) * (grid_size_x * 4 + 1) + selected.x * 4 + 2] = L' ';
}

void grid_select(Cell player, Vector2UInt32 selected, uint32_t *move_count)
{
    grid[selected.y * grid_size_x + selected.x] = player;
    grid_buffer[(selected.y * 2 + 1) * (grid_size_x * 4 + 1) + selected.x * 4 + 2] = player;

    moves[*move_count] = selected;

    ++*move_count;
}

void grid_deselect(uint32_t *move_count)
{
    Vector2UInt32 move = moves[--*move_count];
    grid[move.y * grid_size_x + move.x] = 0;
    grid_buffer[(move.y * 2 + 1) * (grid_size_x * 4 + 1) + move.x * 4 + 2] = CELL_NIL;
}

bool grid_undo(uint32_t *move_count, uint32_t *turn, uint32_t player_count)
{
    if (!*move_count)
    {
        printf(NO_MOVES_LEFT);
        return false;
    }

    grid_deselect(move_count);
    *turn = grid_turn_switch_previous(*turn, player_count);
    return true;
}

uint32_t grid_turn_switch_first(void)
{
    grid_turn_buffer[grid_turn_buffer_size_x] = L'»';
    grid_turn_buffer[grid_turn_buffer_size_x + 4] = L'«';

    return 0;
}

uint32_t grid_turn_switch_next(uint32_t turn, uint32_t player_count)
{
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4] = L'║';
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4 + 4] = L'║';

    turn = (turn + 1) % player_count;

    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4] = L'»';
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4 + 4] = L'«';

    return turn;
}

uint32_t grid_turn_switch_previous(uint32_t turn, uint32_t player_count)
{
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4] = L'║';
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4 + 4] = L'║';

    turn = (player_count + turn - 1) % player_count;

    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4] = L'»';
    grid_turn_buffer[grid_turn_buffer_size_x + turn * 4 + 4] = L'«';

    return turn;
}

bool grid_test_cells(Vector2UInt32 selected)
{
    int32_t grid_size = grid_size_x * grid_size_y, stack = selected.y * grid_size_x;
    uint32_t count;
    Cell cell = grid[stack + selected.x];

    count = 1;
    for (int32_t i = selected.x; --i >= 0          && grid[stack + i] == cell; ++count);
    for (int32_t i = selected.x; ++i < grid_size_x && grid[stack + i] == cell; ++count);
    if (count >= threshold)
        return true;

    count = 1;
    for (int32_t j = stack; (j -= grid_size_x) >= 0        && grid[j + selected.x] == cell; ++count);
    for (int32_t j = stack; (j += grid_size_x) < grid_size && grid[j + selected.x] == cell; ++count);
    if (count >= threshold)
        return true;

    count = 1;
    for (int32_t j = stack, i = selected.x; (j -= grid_size_x) >= 0        && --i >= 0          && grid[j + i] == cell; ++count);
    for (int32_t j = stack, i = selected.x; (j += grid_size_x) < grid_size && ++i < grid_size_x && grid[j + i] == cell; ++count);
    if (count >= threshold)
        return true;

    count = 1;
    for (int32_t j = stack, i = selected.x; (j -= grid_size_x) >= 0        && ++i < grid_size_x && grid[j + i] == cell; ++count);
    for (int32_t j = stack, i = selected.x; (j += grid_size_x) < grid_size && --i >= 0          && grid[j + i] == cell; ++count);
    if (count >= threshold)
        return true;

    return false;
}
