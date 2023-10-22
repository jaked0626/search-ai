#include <iostream> 
#include <random>
#include <sstream>
#include <ctime>
#include <vector>
#include <queue>
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
    public:
        Coord m_character { Coord() };
        int m_game_score { 0 };
        int m_seed { 0 };
        int first_action {};
        
        /// @brief default constructor and copy constructor 
        Maze() = default;

        /// @brief manual constructor
        /// @param seed
        Maze(const int seed, const int height=30, const int width=40, const int final_turn=4) 
            : m_seed { seed }
            , m_height { height }
            , m_width { width }
            , m_final_turn { final_turn }
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
            std::cout << "Printing Board\n";
            std::cout << to_string();
        }

        // https://www.learncpp.com/cpp-tutorial/overloading-the-assignment-operator/

        // Overloaded copy assignment
	    Maze& operator= (const Maze& maze)
        {
            // self-assignment check
            if (this == &maze)
            {
                return *this;
            }
            m_character = maze.m_character;
            m_game_score = maze.m_game_score;
            m_seed = maze.m_seed;
            first_action = maze.first_action;
            m_height = maze.m_height;
            m_width = maze.m_width;
            m_final_turn = maze.m_final_turn;
            m_points = maze.m_points;
            m_turn = maze.m_turn;

            return *this;
        }

        /// @brief checks if the game is done 
        /// @return true if game is done (reached the final turn)
        bool is_done() const
        {
            return m_turn == m_final_turn;
        }

        /// @brief returns a vector object of legal actions
        /// @return actions
        std::vector<int> get_legal_actions() const
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
            std::vector<int> actions = get_legal_actions();
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

    private:
        // no longer using const due to need for copy assignment in beam search implementation 
        int m_height {};
        int m_width {};
        int m_final_turn {};
        // use vector for points to allow dynamic construction
        std::vector<std::vector<int>> m_points;
        int m_turn{ 0 };
        static constexpr const int m_dx[4] = { 1, -1, 0, 0 };
        static constexpr const int m_dy[4] = { 0, 0, 1, -1 };
};

/****************************** GAMEPLAY ******************************/

struct ActionArgs
{
    Maze maze {};
    int beam_width {};
    int beam_depth {};
    ActionArgs(Maze maze = Maze{ }, const int w = 1, const int d = 1) 
        : maze{ maze }
        , beam_width{ w }
        , beam_depth{ d } 
    {
        // no other constructor details 
    }
};

/// @brief we define an ai that chooses actions randomly per turn
/// @param args ActionArgs 
/// @return a legal action of int type 
int random_action (ActionArgs args)
{   auto rand_int = std::mt19937(args.maze.m_seed++);
    std::vector legal_actions = args.maze.get_legal_actions();
    return legal_actions[abs_val(rand_int() % legal_actions.size())];
}


/// @brief returns a user input action for each turn of the game.
/// @param args
/// @return a legal action of int type
int user_action (ActionArgs args)
{
    std::cout << "Pick one of the following: 0 (right), 1 (left), 2 (down), 3 (up): " << std::endl;
    int action = { };
    std::cin >> action;

    return action;
}


/// @brief player_ai function that uses the greedy method to solve the game
/// @param args
/// @return greedy_action
int greedy_action (ActionArgs args) 
{
    std::vector legal_actions{ args.maze.get_legal_actions() }; // { 0, 1, 2, 3 }
    int max_points{ 0 };
    int greedy_action{ 0 };
    for (const int action: legal_actions)
    {
        int temp_points = { args.maze.get_point(action) };
        if (temp_points > max_points)
        {
            max_points = temp_points;
            greedy_action = action;
        }
    }

    return greedy_action;
}

bool operator<(Maze a, Maze b)
{
    return a.m_game_score < b.m_game_score;
}
/// TODO: create a player_ai function that uses the beamsearch method to solve the game 

/// @brief player_ai function that uses the beamsearch method to solve the game 
/// @param args
/// @return a legal action of int type
int beamsearch_action (ActionArgs args) 
{
    std::priority_queue<Maze, std::vector<Maze>> current_beam {};
    Maze best_state {};
    current_beam.push(args.maze);
    for (int i = 0; i < args.beam_depth; ++i) 
    {
        std::priority_queue<Maze, std::vector<Maze>> next_beam {};
        for (int j = 0; j < args.beam_width; ++j)
        {
            if (current_beam.empty())
            {
                break;
            }
            Maze current_state{ current_beam.top() };
            current_beam.pop();
            for (const int action: current_state.get_legal_actions())
            {
                Maze next_state { current_state };
                next_state.advance(action);
                if (i == 0) 
                {
                    next_state.first_action = action;
                }
                next_beam.push(next_state);
            }
        }
        current_beam = next_beam;
        best_state = current_beam.top();
        if (best_state.is_done())
        {
            break;
        }
    }
    return best_state.first_action;
}


/// @brief simulates game given a player function 
/// @param args
/// @param player_ai
void play_game(ActionArgs args, int (*player_ai)(ActionArgs)) 
{
    args.maze.to_string();
    while (!args.maze.is_done())
    {
        int play = player_ai(args);
        args.maze.advance(play);
        args.maze.to_string();
    }
}



/******************************* MAIN *******************************/

int main() {
    auto rand_int = std::mt19937(std::time(0));

    Maze maze { 2024, 30, 20, 6 };

    ActionArgs args { maze, 10, 10 };

    // std::cout << "------- GAME 1 (RANDOM) ---------\n"; 
    // play_game(args, *random_action);

    std::cout << "------- GAME 2 (GREEDY) ---------\n"; 
    play_game(args, *greedy_action);

    std::cout << "------- GAME 3 (BEAM SEARCH) ---------\n"; 
    play_game(args, *beamsearch_action);

    return 0;
}