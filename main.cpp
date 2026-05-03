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
    GamePlay(int maxStep, int dif){
        for(int i=2; i<13; i++){
            horses.push_back(Horse(0, maxStep-dif*abs(7-i), i));
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

class Bookmaker{
private:
    double loiNhuan, tienVon;
public:
    Bookmaker(double _loiNhuan, double _tienVon):loiNhuan(_loiNhuan), tienVon(_tienVon){}
    vector<double> getTiLeCuoc(vector<double> tiLeThang, int error){
        vector<double> TiLeCuoc(13, 0.0);
        for(int i=2; i<13; i++){
            if(tiLeThang[i]>0){
                double tiLeAo=tiLeThang[i]*(1+loiNhuan);
                if(tiLeAo>1)    tiLeAo=1;
                TiLeCuoc[i]=1.0/tiLeAo;
            }
        }
        if(error)
            TiLeCuoc[3]=(1.0/tiLeThang[3])*2.0;
        return TiLeCuoc;
    }
    void thuTien(double tienCuoc){ tienVon+=tienCuoc; }
    void traTien(double tienCuoc, double tiLeCuoc){ tienVon-=tienCuoc*tiLeCuoc; }
    double getTienVon(){ return tienVon; }
};

class Player{
private:
    double tienVon;
public:
    Player(double _tienVon):tienVon(_tienVon){}
    int findBestBet(vector<double> TiLeThang, vector<double> TiLeCuoc){
        int bestHorse=-1;
        double maxKiVong=0;
        for(int i=2; i<13; i++){
            double kiVong=TiLeThang[i]*TiLeCuoc[i]-1;
            if(kiVong>maxKiVong){
                maxKiVong=kiVong;
                bestHorse=i;
            }
        }
        return bestHorse;
    }
    double datCuoc(double tiLeVon){
        double betAmount=tienVon*tiLeVon;
        tienVon-=betAmount;
        return betAmount;
    }
    void nhanTien(double money){
        tienVon+=money;
    }
    double getTienVon(){ return tienVon; }
};

class Bet{
public:
    void run(){
        int maxStep, dif, nTest, nPlay, error;
        cout << "Nhap khoang cach lon nhat de thang (khoang cach ngua so 7 can di de thang): ";
        cin >> maxStep;
        cout << "Nhap chenh lech khoang cach 2 con ngua ke nhau: ";
        cin >> dif;
        cout << "Nhap so van test: ";
        cin >> nTest;
        cout << "Nhap so van ca cuoc thuc te: ";
        cin >> nPlay;
        cout << "Nha cai loi ( 0 / 1 ): ";
        cin >> error;
        GamePlay test(maxStep, dif);
        vector<int> winner(13, 0);
        for(int i=0; i<nTest; i++)
            winner[test.run()]++;
        vector<double> TiLeThang(13, 0.0);
        for(int i=2; i<13; i++)
            TiLeThang[i]=double(winner[i])/nTest;
        Bookmaker nhaCai(0.1, 1000000);
        Player nguoiChoi(1000);
        vector<double> TiLeCuoc=nhaCai.getTiLeCuoc(TiLeThang, error);
        GamePlay real(maxStep, dif);
        int betTarget=nguoiChoi.findBestBet(TiLeThang, TiLeCuoc);
        if(betTarget!=-1)
            cout<<"Ngua so "<<betTarget<<" co ki vong duong.";
        else{
            cout<<"Khong co keo ngon, khong bet.";
            return;
        }
        for(int i=0; i<nPlay; i++){
            if(betTarget!=-1 && nguoiChoi.getTienVon()>0){
                double tienCuoc=nguoiChoi.datCuoc(0.05);
                nhaCai.thuTien(tienCuoc);
                int winner=real.run();
                if(winner==betTarget){
                    nguoiChoi.nhanTien(tienCuoc*TiLeCuoc[betTarget]);
                    nhaCai.traTien(tienCuoc, TiLeCuoc[betTarget]);
                }
            }
        }
        cout<<fixed<<setprecision(2);
        cout<<endl<<"Ket qua sau "<<nPlay<<" cuoc dua."<<endl;
        cout<<"Tien von nguoi choi: "<<nguoiChoi.getTienVon()<<endl;
        cout<<"Tien von nha cai: "<<nhaCai.getTienVon();
    }
};

int main(){
    Bet test;
    test.run();
    return 0;
}
