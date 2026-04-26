#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
using namespace std;

class Dice{
private:
    static inline mt19937 rng{random_device{}()};
    int dice1, dice2;
    uniform_int_distribution<int> dist;
    void roll(){
        dice1=dist(rng);
        dice2=dist(rng);
    }
    int getDice1(){ return dice1; }
    int getDice2(){ return dice2; }
public:
    Dice():dist(1, 6){
        roll();
    }
    int getDiceSum(){
        roll();
        return getDice1()+getDice2();
    }
};

class Horse{
private:
    int pos, goal, no;
public:
    Horse(int _pos, int _goal, int _no):pos(_pos), goal(_goal), no(_no){}
    void move(){ pos++; }
    bool win(){ return pos>=goal; }
    int getNo(){ return no; }
    void reset(){ pos=0; }
};

class GamePlay{
private:
    Dice dice;
    vector<Horse> horses;
public:
    GamePlay(){
        for(int i=2; i<13; i++){
            horses.push_back(Horse(0, 13-2*abs(7-i), i));
        }
    }
    void resetAll(){
        for(auto &a:horses)
            a.reset();
    }
    int run(){
        resetAll();
        while(1){
            int numToMove=dice.getDiceSum();
            horses[numToMove-2].move();
            if(horses[numToMove-2].win()){
                return horses[numToMove-2].getNo();
            }
        }
    }
};

class Player{

};

class Statistic{
public:
    void Test(){
        cout<<"Nhap so luot choi: ";
        int n;  cin>>n;
        vector<int> winner(13);
        GamePlay play;
        for(int i=0; i<n; i++)
            winner[play.run()]++;
        cout<<endl<<"Ti le thang: ";
        for(int i=2; i<13; i++)
            cout<<endl<<"No."<<i<<": "<<fixed<<setprecision(2)<<winner[i]*100.0/n<<" %";
    }
};

int main(){
    Statistic test;
    test.Test();
    return 0;
}