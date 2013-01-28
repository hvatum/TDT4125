#include "evaluate.h"
#include "ex1.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>

#include <climits>

using namespace std;
using namespace std::placeholders;

#define NO_ANSWER_YET INT_MAX

class Coin_counter {
    public:
        Coin_counter(uint32_t n, vector<uint32_t>* c) : coins(c), num_coins(n) {};
        uint32_t num_coins;
        vector<uint32_t>* coins;
};

Coin_counter* optimal(const vector<Coin_counter*>& dp, uint32_t value){
    // TODO: Implement this function
    return new Coin_counter(1,new vector<uint32_t>(dp[0]->coins->size(), 0));
}

vector<uint32_t> calculateMinimumCoins(const vector<uint32_t>& coins, uint32_t value){
    vector<uint32_t> ret(coins.size(), 0);

    //Your code here
    printf("Total value is %d, and coins avaliable are:\n", value);
    // Create DP-table with one entry for each value from 0 up until value
    Coin_counter no_answer(NO_ANSWER_YET, NULL);
    vector<Coin_counter*> dp(value + 1, &no_answer);

    // Set all basis-elements to 1, since we only need one coin if value = coin-value
    vector<uint32_t >* zero = new vector<uint32_t>(coins.size(), 0);
    dp[0] = new Coin_counter(0, zero);
    for (int i = 0; i < coins.size(); i++)
    {
        printf("\t%d\n", coins[i]);
        if (coins[i] >= value) break;
        vector<uint32_t>* single_coin_answer = new vector<uint32_t>(coins.size(), 0);
        (*single_coin_answer)[i] = 1;
        dp[coins[i]] = new Coin_counter(1, single_coin_answer);

    }

    for (int current = 0; current <= value; current++)
    {
        if (dp[current] != &no_answer) continue;
        else dp[current] = optimal(dp, current);
    }

    ret = *(dp[value]->coins);

    return ret;
}

int main(){

    //number of coins to use
    uint32_t nCoins = 50;
    //max value of coins
    uint32_t maxCoin = 1000;
    //max possible total value
    uint32_t maxValue = 1000;

    //generator used to create random numbers, seeded with current time
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    //create function that generates random coins
    uniform_int_distribution<int> coinDistribution(2, maxCoin);
    auto randomCoin = bind(coinDistribution, generator);

    //create function that generates random values
    uniform_int_distribution<int> valueDistribution(maxValue/2, maxValue);
    auto randomValue = bind(valueDistribution, generator);

    //make nCoins large vector of coins, all has value 1
    vector<uint32_t> coins(nCoins, 1);
    //fill in all except the first place with random coins
    generate(coins.begin() + 1, coins.end(), randomCoin);
    //sort the coins
    sort(coins.begin(), coins.end());

    //generate a random value
    uint32_t value = randomValue();

    //evaluate the function
    Evaluation evaluation = evaluateEx1( bind(calculateMinimumCoins, coins, value), bind( evaluateCertificateEx1, _1, coins, value ) );
    printf("Function achieves accuracy %.5f and time %.5f\n", evaluation.accuracy, evaluation.seconds);

}
