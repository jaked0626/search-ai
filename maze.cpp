#include <iostream> 
#include <random>
#include <sstream>
#include <ctime>
#include <vector>
#include "utils.h"

/****************************** GAME ENVIRONMENT ******************************/

struct Coord
{
    int x_;
    int y_;
    // init 
    Coord(const int y = 0, const int x = 0) : y_{ y }, x_{ x } 
    {
        // no other constructor initialization logic 
    }
};

class Maze
{
    private:
        const int m_height{};
        const int m_width = {};
        const int m_final_turn = {};
        // use vector for points to allow dynamic construction
        std::vector<std::vector<int>> m_points;
        int m_turn = { 0 };
        static constexpr const int m_dx[4] = { 1, -1, 0, 0 };
        static constexpr const int m_dy[4] = { 0, 0, 1, -1 };

    public:
        Coord m_character{ Coord() };
        int m_game_score{ 0 };
        int m_seed{ 0 };
        
        /// @brief default constructor 
        Maze() = default;

        /// @brief manual constructor
        /// @param seed
        Maze(const int seed, const int height=30, const int width=40, const int final_turn=4) 
            : m_seed{ seed }
            , m_height{ height }
            , m_width{ width }
            , m_final_turn{ final_turn }
        {
            // randomized starting place of our character 
            auto rand_int = std::mt19937(m_seed);
            m_character.y_ = abs_val(rand_int() % m_height);
            m_character.x_ = abs_val(rand_int() % m_width);

            // initialize grid of points with scores per coordinate of 0~9
            m_points.resize(m_height, std::vector<int>(m_width, 0));
            for (int y = 0; y < m_height; y++) 
            {
                for (int x = 0; x < m_width; x++)
                {
                    if (x == m_character.x_ && y == m_character.y_) 
                    {
                        m_points[y][x] = 0;
                        continue;
                    }
                    m_points[y][x] = abs_val(rand_int() % 10); 
                }
            }
            // std::cout << m_height << " " << m_width << " " << m_final_turn;
            print_points();
        }

        /// @brief checks if the game is done 
        /// @return true if game is done (reached the final turn)
        bool is_done() const
        {
            return m_turn == m_final_turn;
        }

        /// @brief returns a vector object of legal actions
        /// @return actions
        std::vector<int> legal_actions() const
        {
            std::vector<int> actions;
            for (int action = 0; action < 4; action++)
            {
                int ty = { m_character.y_ + m_dy[action] };
                int tx = { m_character.x_ + m_dx[action] };
                if (ty >= 0 && ty < m_height && tx >= 0 && tx < m_width)
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
             
            m_character.x_ += m_dx[action];
            m_character.y_ += m_dy[action];
            m_game_score += m_points[m_character.y_][m_character.x_];
            m_points[m_character.y_][m_character.x_] = 0;
            m_turn++;

            return;
        }

        /// @brief returns the points possible from a specific action
        /// @param action 
        /// @return points
        int get_point(int action) 
        {
            int tx = { m_character.x_ + m_dx[action] };
            int ty = { m_character.y_ + m_dy[action] };

            return m_points[ty][tx];
        }

        /// @brief gets the expected game score from playing a certain action
        /// @return 
        int get_score(int action) 
        {
            int point = { get_point(action) };

            return m_game_score + get_point(action);
        }

        /// @brief represents the game state as a string 
        /// @return string representing the state of the game 
        std::string to_string() const 
        {
            std::stringstream ss;
            ss << "turn: " << m_turn << "\n";
            ss << "score: " << m_game_score << "\n";
            for (int h = 0; h < m_height; h++)
            {
                ss << "|";
                for (int w = 0; w < m_width; w++)
                {
                    if (w == m_character.x_ && h == m_character.y_)
                    {
                        ss << "@|";
                    }
                    else if (m_points[h][w] > 0)
                    {
                        ss << m_points[h][w] << "|";
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
    
        void print_points() 
        {
            std::cout << "Printing Point Board\n";
            for (int i = 0; i < m_points.size(); i++) 
            {
                for (int j = 0; j < m_points[i].size(); j++) 
                {
                    std::cout << m_points[i][j] << " ";
                }
                std::cout << "\n";
            }
        }
};

/****************************** GAMEPLAY ******************************/

struct ActionArgs
{
    Maze m;
    int beam_width;
    int beam_length;
    ActionArgs(Maze maze = Maze{ }, const int w = 1, const int l = 1) 
        : m{ maze }
        , beam_width{ w }
        , beam_length{ l } 
    {
        // no other constructor details 
    }
};

/// @brief we define an ai that chooses actions randomly per turn
/// @param m 
/// @return a legal action of int type 
int random_action (Maze m)
{   auto rand_int = std::mt19937(m.m_seed++);
    std::vector legal_actions = m.legal_actions();
    return legal_actions[abs_val(rand_int() % legal_actions.size())];
}


/// @brief returns a user input action for each turn of the game.
/// @param m
/// @return a legal action of int type
int user_action (Maze m)
{
    std::cout << "Pick one of the following: 0 (right), 1 (left), 2 (down), 3 (up): " << std::endl;
    int action = { };
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

// bool operator<(CallLog a, CallLog b)
// {
//     return a.dur < b.dur;
// } need this too? return m1.m_game_score < m2.m_game_score; 

/// TODO: create a player_ai function that uses the beamsearch method to solve the game 

/// @brief player_ai function that uses the beamsearch method to solve the game 
/// @param m 
/// @return a legal action of int type
int beamsearch_action (ActionArgs args) 
{
    // current_beam = priorityqueue for maze 
    // Maze best_state
    // Maze: int first_action_map 
    // current_beam.push(args.m)
    // for t < args.beam_depth 
    // {
        // next_beam = priorityqueue for maze 
        // for i < beam_width 
        // {
            // if current_beam is empty, break
            // current_state = current_beam.pop(); 
            // for action in legalactions: 
            //      next_state = current_state
            //      next_state.advance(action);
            //      if t == 0: firstactionmap[next_state] = action; # must find another way to track first action back to maze board state. Perhaps add as a member? 
            //      next_beam.push(next_state);
            // 
        // }
        // current_beam = next_beam;
        // best_state = current_beam.top()
        // if best_state is done, break; 
    // }

    // return best_state.first_action
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



/******************************* MAIN *******************************/

int main() {
    auto rand_int = std::mt19937(std::time(0));

    Maze m { 2024 };


    // std::cout << "------- GAME 1 (RANDOM) ---------\n"; 
    // play_game(m, *random_action);

    // std::cout << "------- GAME 2 (GREEDY) ---------\n"; 
    // play_game(m, *greedy_action);

    return 0;
}