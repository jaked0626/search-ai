/******************************* UTILS *******************************/

/// @brief defining own absolute value function to avoid conflicting std::abs definitions
/// @param x integer 
/// @return the absolute value of x
int abs_val(int x) 
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