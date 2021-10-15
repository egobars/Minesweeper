#include "minesweeper.h"
#include <ctime>
#include <queue>
#include <random>

void Minesweeper::DeleteField() {
    for (size_t i = 0; i < field_height_; ++i) {
        delete[] field_[i];
    }
    delete[] field_;
}

bool Minesweeper::IsInField(const Cell& cell) {
    if (cell.x < 0 || cell.x >= field_width_) {
        return false;
    }
    if (cell.y < 0 || cell.y >= field_height_) {
        return false;
    }
    return true;
}

void Minesweeper::PlaceMines(const std::vector<Cell>& cells_with_mines) {
    for (const Cell& cell : cells_with_mines) {
        field_[cell.y][cell.x].is_bomb = true;
        for (const RelativeCell& relative_cell : around_) {
            Cell new_cell = {cell.x + relative_cell.x, cell.y + relative_cell.y};
            if (IsInField(new_cell)) {
                ++field_[new_cell.y][new_cell.x].bombs_around;
            }
        }
    }
}

Minesweeper::Minesweeper(size_t width, size_t height, size_t mines_count) {
    field_ = new FieldCell*[height];
    for (size_t i = 0; i < height; ++i) {
        field_[i] = new FieldCell[width];
    }
    field_height_ = height;
    field_width_ = width;
    std::vector<Cell> all_cells;
    for (size_t i = 0; i < width * height; ++i) {
        all_cells.push_back({i % width, i / width});
    }
    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(all_cells.begin(), all_cells.end(), generator);
    std::vector<Cell> cells_with_mines;
    for (size_t i = 0; i < mines_count; ++i) {
        cells_with_mines.push_back(all_cells[i]);
    }
    PlaceMines(cells_with_mines);
    current_game_status_ = GameStatus::NOT_STARTED;
    free_cells_count_ = height * width - mines_count;
}

Minesweeper::Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    field_ = new FieldCell*[height];
    for (size_t i = 0; i < height; ++i) {
        field_[i] = new FieldCell[width];
    }
    field_height_ = height;
    field_width_ = width;
    PlaceMines(cells_with_mines);
    current_game_status_ = GameStatus::NOT_STARTED;
    free_cells_count_ = height * width - cells_with_mines.size();
}

Minesweeper::~Minesweeper() {
    DeleteField();
}

void Minesweeper::NewGame(size_t width, size_t height, size_t mines_count) {
    DeleteField();
    field_ = new FieldCell*[height];
    for (size_t i = 0; i < height; ++i) {
        field_[i] = new FieldCell[width];
    }
    field_height_ = height;
    field_width_ = width;
    std::vector<Cell> all_cells;
    for (size_t i = 0; i < width * height; ++i) {
        all_cells.push_back({i % width, i / width});
    }
    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(all_cells.begin(), all_cells.end(), generator);
    std::vector<Cell> cells_with_mines;
    for (size_t i = 0; i < mines_count; ++i) {
        cells_with_mines.push_back(all_cells[i]);
    }
    PlaceMines(cells_with_mines);
    current_game_status_ = GameStatus::NOT_STARTED;
    free_cells_count_ = height * width - mines_count;
}

void Minesweeper::NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    DeleteField();
    field_ = new FieldCell*[height];
    for (size_t i = 0; i < height; ++i) {
        field_[i] = new FieldCell[width];
    }
    field_height_ = height;
    field_width_ = width;
    PlaceMines(cells_with_mines);
    current_game_status_ = GameStatus::NOT_STARTED;
    free_cells_count_ = height * width - cells_with_mines.size();
}

void Minesweeper::OpenCell(const Cell& cell) {
    if (current_game_status_ == GameStatus::DEFEAT || current_game_status_ == GameStatus::VICTORY) {
        return;
    }
    if (field_[cell.y][cell.x].is_open) {
        return;
    }
    if (field_[cell.y][cell.x].is_flag) {
        return;
    }
    if (current_game_status_ == GameStatus::NOT_STARTED) {
        current_game_status_ = GameStatus::IN_PROGRESS;
        game_start_time_ = std::time(nullptr);
    }
    if (field_[cell.y][cell.x].is_bomb) {
        for (size_t i = 0; i < field_height_; ++i) {
            for (size_t j = 0; j < field_width_; ++j) {
                field_[i][j].is_open = true;
            }
        }
        current_game_status_ = GameStatus::DEFEAT;
        all_game_time_ = std::time(nullptr) - game_start_time_;
        return;
    }
    std::queue<Cell> bfs_queue;
    bfs_queue.push(cell);
    while (!bfs_queue.empty()) {
        Cell now_cell = bfs_queue.front();
        bfs_queue.pop();
        if (field_[now_cell.y][now_cell.x].is_open) {
            continue;
        }
        field_[now_cell.y][now_cell.x].is_open = true;
        --free_cells_count_;
        bool has_bomb_around = false;
        for (const RelativeCell& relative_cell : around_) {
            Cell new_cell = {now_cell.x + relative_cell.x, now_cell.y + relative_cell.y};
            if (IsInField(new_cell)) {
                if (field_[new_cell.y][new_cell.x].is_bomb) {
                    has_bomb_around = true;
                }
            }
        }
        if (!has_bomb_around) {
            for (const RelativeCell& relative_cell : around_) {
                Cell new_cell = {now_cell.x + relative_cell.x, now_cell.y + relative_cell.y};
                if (IsInField(new_cell)) {
                    if (!field_[new_cell.y][new_cell.x].is_flag) {
                        bfs_queue.push(new_cell);
                    }
                }
            }
        }
    }
    if (free_cells_count_ == 0) {
        current_game_status_ = GameStatus::VICTORY;
        all_game_time_ = std::time(nullptr) - game_start_time_;
    }
}

void Minesweeper::MarkCell(const Cell& cell) {
    if (current_game_status_ == GameStatus::DEFEAT || current_game_status_ == GameStatus::VICTORY) {
        return;
    }
    if (field_[cell.y][cell.x].is_open) {
        return;
    }
    field_[cell.y][cell.x].is_flag = !(field_[cell.y][cell.x].is_flag);
    if (current_game_status_ == GameStatus::NOT_STARTED) {
        current_game_status_ = GameStatus::IN_PROGRESS;
        game_start_time_ = std::time(nullptr);
    }
}

Minesweeper::GameStatus Minesweeper::GetGameStatus() const {
    return current_game_status_;
}

time_t Minesweeper::GetGameTime() const {
    if (current_game_status_ == GameStatus::NOT_STARTED) {
        return 0;
    }
    if (current_game_status_ == GameStatus::IN_PROGRESS) {
        return std::time(nullptr) - game_start_time_;
    }
    return all_game_time_;
}

Minesweeper::RenderedField Minesweeper::RenderField() const {
    RenderedField result(field_height_);
    for (size_t y = 0; y < field_height_; ++y) {
        for (size_t x = 0; x < field_width_; ++x) {
            if (!field_[y][x].is_open) {
                if (field_[y][x].is_flag) {
                    result[y] += '?';
                } else {
                    result[y] += '-';
                }
            } else if (field_[y][x].is_bomb) {
                result[y] += '*';
            } else {
                if (field_[y][x].bombs_around == 0) {
                    result[y] += '.';
                } else {
                    result[y] += static_cast<char>(field_[y][x].bombs_around) + '0';
                }
            }
        }
    }
    return result;
}
