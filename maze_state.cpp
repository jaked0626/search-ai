#include <iostream> 
#include <random>
#include <sstream>
#include "maze_state.h"
#include <ctime>

/****************************** GAME ENVIRONMENT ******************************/

struct Coord
{
    int x_;
    int y_;
    // init 
    Coord(const int y = 0, const int x = 0) : y_(y), x_(x) {}
};

constexpr const int H = { 3 };
constexpr const int W = { 4 };
constexpr const int FINAL_TURN = { 4 };

class Maze
{
    private:
        int points_[H][W];
        int turn_ = { 0 };
        static constexpr const int dx[4] = { 1, -1, 0, 0 };
        static constexpr const int dy[4] = { 0, 0, 1, -1 };

    public:
        Coord character_ = { Coord() };

        int game_score_ = { 0 };

        int seed_ = { 0 };

        // add default constructor
        Maze() = default;

        /// @brief manual constructor
        /// @param seed 
        Maze(const int seed)
        {
            this->seed_ = seed;
            auto rand_int = std::mt19937(this->seed_);
            this->character_.y_ = absolute_value(rand_int() % H);
            this->character_.x_ = absolute_value(rand_int() % W);

            for (int y = 0; y < H; y++) 
            {
                for (int x = 0; x < W; x++)
                {
                    if (x == this->character_.x_ && y == this->character_.y_) 
                    {
                        this->points_[y][x] = 0;
                        continue;
                    }
                    points_[y][x] = absolute_value(rand_int() % 10); // score per coordinate is 0~9
                }
            } 
        }

        /// @brief checks if the game is done 
        /// @return true if game is done (reached the final turn)
        bool is_done() const
        {
            return this->turn_ == FINAL_TURN;
        }

        /// @brief returns a vector object of legal actions
        /// @return actions
        std::vector<int> legal_actions() const
        {
            std::vector<int> actions;
            for (int action = 0; action < 4; action++)
            {
                int ty = { this->character_.y_ + dy[action] };
                int tx = { this->character_.x_ + dx[action] };
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                {
                    actions.push_back(action);
                }
            }
            return actions;
        }

        /// @brief advances the game by one play 
        /// @param action 
        void advance(const int action) 
        {
            std::vector<int> actions = legal_actions();
            if (std::find(actions.begin(), actions.end(), action) == actions.end()) 
            {
                std::cout << "You are trying to play an illegal move. Please try again." << std::endl;
                return;
            }
             
            this->character_.x_ += dx[action];
            this->character_.y_ += dy[action];
            this->game_score_ += this->points_[this->character_.y_][this->character_.x_];
            this->points_[this->character_.y_][this->character_.x_] = 0;
            this->turn_++;

            return;
        }

        /// @brief returns the points possible from a specific action
        /// @param action 
        /// @return points
        int get_point(int action) {
            int tx = { this->character_.x_ + this->dx[action] };
            int ty = { this->character_.y_ + this->dy[action] };

            return this->points_[ty][tx];
        }

        /// @brief represents the game state as a string 
        /// @return string representing the state of the game 
        std::string to_string() const 
        {
            std::stringstream ss;
            ss << "turn: " << this->turn_ << "\n";
            ss << "score: " << this->game_score_ << "\n";
            for (int h = 0; h < H; h++)
            {
                ss << "|";
                for (int w = 0; w < W; w++)
                {
                    if (w == this->character_.x_ && h == this->character_.y_)
                    {
                        ss << "@|";
                    }
                    else if (this->points_[h][w] > 0)
                    {
                        ss << points_[h][w] << "|";
                    }
                    else {
                        ss << ".|";
                    }
                }
                ss << "\n";
            }
            std::cout << ss.str();
            return ss.str();
        }
};

/****************************** GAMEPLAY ******************************/

/// @brief we define an ai that chooses actions randomly per turn
/// @param m 
/// @return a legal action of int type 
int random_action (Maze m)
{   auto rand_int = std::mt19937(m.seed_++);
    std::vector legal_actions = m.legal_actions();
    return legal_actions[absolute_value(rand_int() % legal_actions.size())];
}


/// @brief returns a user input action for each turn of the game.
/// @param m
/// @return a legal action of int type
int user_action (Maze m)
{
    std::cout << "Pick one of the following: 0 (right), 1 (left), 2 (down), 3 (up): " << std::endl;
    int action { };
    std::cin >> action;

    return action;
}


/// @brief player_ai function that uses the greedy method to solve the game
/// @param m 
/// @return greedy_action
int greedy_action (Maze m) 
{
    std::vector legal_actions = { m.legal_actions() }; // { 0, 1, 2, 3 }
    int max_points = { 0 };
    int greedy_action = { 0 };
    for (const int action: legal_actions)
    {
        int temp_points = { m.get_point(action) };
        if (temp_points > max_points)
        {
            max_points = temp_points;
            greedy_action = action;
        }
    }

    return greedy_action;
}


/// TODO: create a player_ai function that uses the beamsearch method to solve the game 

/// @brief player_ai function that uses the beamsearch method to solve the game 
/// @param m 
/// @return a legal action of int type
int beamsearch_action (Maze m) 
{
    return 0;
}


/// @brief simulates game given a player function 
/// @param m
/// @param player_ai
void play_game(Maze m, int (*player_ai)(Maze)) 
{
    m.to_string();
    while (!m.is_done())
    {
        int play = player_ai(m);
        m.advance(play);
        m.to_string();
    }
}

/******************************* UTILS *******************************/

/// @brief defining own absolute value function to avoid conflicting std::abs definitions
/// @param x integer 
/// @return the absolute value of x
int absolute_value(int x) 
{
    if (x >= 0)
    {
        return x;
    } 
    else 
    {
        return -x;
    }
}

/******************************* MAIN *******************************/

int main() {
    auto rand_int = std::mt19937(std::time(0));

    Maze m { 2024 };


    std::cout << "------- GAME 1 (RANDOM) ---------\n"; 
    play_game(m, *random_action);

    std::cout << "------- GAME 2 (GREEDY) ---------\n"; 
    play_game(m, *greedy_action);

    return 0;
}