#pragma once

#include <string>
#include <vector>

class Minesweeper {
public:
    struct Cell {
        size_t x = 0;
        size_t y = 0;
    };

    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    using RenderedField = std::vector<std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count);
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);
    ~Minesweeper();

    void NewGame(size_t width, size_t height, size_t mines_count);
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void OpenCell(const Cell& cell);
    void MarkCell(const Cell& cell);

    GameStatus GetGameStatus() const;
    time_t GetGameTime() const;

    RenderedField RenderField() const;

private:
    struct FieldCell {
        bool is_open = false;
        bool is_bomb = false;
        bool is_flag = false;
        int bombs_around = 0;
    };
    struct RelativeCell {
        int64_t x = 0;
        int64_t y = 0;
    };
    void PlaceMines(const std::vector<Cell>& cell_with_mines);
    bool IsInField(const Cell& cell);
    void DeleteField();
    FieldCell** field_;
    GameStatus current_game_status_;
    size_t field_width_, field_height_;
    const std::vector<RelativeCell> around_ = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
    time_t game_start_time_, all_game_time_;
    size_t free_cells_count_;
};
