# How to Use.  

```bash
# compile the executable
g++ -std=c++17 -o maze maze_state.cpp 

# run the main app
./main
```

To add algorithms, define a function that takes a maze object and define your search algorithm within that function.  
Then, call your function in `main()`:
```cpp
    std::cout << "------- GAME 3 ---------\n"; 
    play_game(m, *new_algo);
```