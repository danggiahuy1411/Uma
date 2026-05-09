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
        if(error){
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dist(2, 12);
            int errorIdx = dist(gen);
            TiLeCuoc[errorIdx]=(1.0/tiLeThang[errorIdx])*2.0;
        }
        return TiLeCuoc;
    }
    void thuTien(double tienCuoc){ tienVon+=tienCuoc; }
    void traTien(double tienCuoc, double tiLeCuoc){ tienVon-=tienCuoc*tiLeCuoc; }
    double getTienVon(){ return tienVon; }
};

class Player{
protected:
    double tienVon;
public:
    Player(double _tienVon):tienVon(_tienVon){}
    virtual int findBestBet(vector<double> TiLeThang, vector<double> TiLeCuoc){
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
    virtual double datCuoc(bool)=0;
    void nhanTien(double money){
        tienVon+=money;
    }
    double getTienVon(){ return tienVon; }
};

class PercentPlayer : public Player{
public:
    PercentPlayer(double _tienVon):Player(_tienVon){}
    double datCuoc(bool){
        float tiLeVon=0.002;
        double betAmount=tienVon*tiLeVon;
        tienVon-=betAmount;
        return betAmount;
    }
};

class FiboPlayer : public Player{
private:
    double pre1=0, pre2=10;
public:
    FiboPlayer(double _tienVon):Player(_tienVon){}
    double datCuoc(bool preWin){
        if(!tienVon)    return 0;
        double tienCuoc;
        if(preWin){
            pre1=0;
            pre2=10;
            tienCuoc=min(pre2, tienVon);
            tienVon-=tienCuoc;
            return tienCuoc;
        }
        double temp=pre2;
        pre2+=pre1;
        pre1=temp;
        tienCuoc=min(pre2, tienVon);
        tienVon-=tienCuoc;
        return tienCuoc;
    }
};

class DoublePlayer : public Player{
private:
    double pre=10;
public:
    DoublePlayer(double _tienVon):Player(_tienVon){}
    double datCuoc(bool preWin){
        if(!tienVon)    return 0;
        double tienCuoc;
        if(preWin){
            pre=10;
            tienCuoc=min(tienVon, pre);
            tienVon-=tienCuoc;
            return tienCuoc;
        }
        pre*=2;
        tienCuoc=min(tienVon, pre);
        tienVon-=tienCuoc;
        return tienCuoc;
    }
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
        PercentPlayer nguoiChoiAnToan(10000);
        FiboPlayer nguoiChoiFibo(10000);
        DoublePlayer ngaDauGapDoiDo(10000);
        vector<double> TiLeCuoc=nhaCai.getTiLeCuoc(TiLeThang, error);
        GamePlay real(maxStep, dif);
        int betTarget=nguoiChoiAnToan.findBestBet(TiLeThang, TiLeCuoc);
        if(betTarget!=-1)
            cout<<"Ngua so "<<betTarget<<" co ki vong duong."<<endl;
        else
            cout<<"Khong co keo ngon, bet random."<<endl;
        bool PercentWin=1;
        bool FiboWin=1;
        bool DoubleWin=1;
        bool keoNgon=(betTarget!=-1 ? 1 : 0);
        cout<<left<<setw(10)<<"Luot"<<setw(15)<<"Bet Target"<<setw(15)<<"Percent"<<setw(15)<<"Fibo"<<setw(15)<<"Double"<<endl;
        int j;
        for(j=0; j<nPlay; j++){
            if(!keoNgon){
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dist(2, 12);
                betTarget = dist(gen);
            }
            int winner=real.run();
            if(nguoiChoiAnToan.getTienVon()>0){
                double tienCuoc=nguoiChoiAnToan.datCuoc(PercentWin);
                nhaCai.thuTien(tienCuoc);
                if(winner==betTarget){
                    nguoiChoiAnToan.nhanTien(tienCuoc*TiLeCuoc[betTarget]);
                    nhaCai.traTien(tienCuoc, TiLeCuoc[betTarget]);
                    PercentWin=1;
                }
                else{
                    PercentWin=0;
                }
            }
            if(nguoiChoiFibo.getTienVon()>0){
                double tienCuoc=nguoiChoiFibo.datCuoc(FiboWin);
                nhaCai.thuTien(tienCuoc);
                if(winner==betTarget){
                    nguoiChoiFibo.nhanTien(tienCuoc*TiLeCuoc[betTarget]);
                    nhaCai.traTien(tienCuoc, TiLeCuoc[betTarget]);
                    FiboWin=1;
                }
                else{
                    FiboWin=0;
                }
            }
            if(ngaDauGapDoiDo.getTienVon()>0){
                double tienCuoc=ngaDauGapDoiDo.datCuoc(DoubleWin);
                nhaCai.thuTien(tienCuoc);
                if(winner==betTarget){
                    ngaDauGapDoiDo.nhanTien(tienCuoc*TiLeCuoc[betTarget]);
                    nhaCai.traTien(tienCuoc, TiLeCuoc[betTarget]);
                    DoubleWin=1;
                }
                else{
                    DoubleWin=0;
                }
            }
            cout<<left<<setw(10)<<j+1<<setw(15)<<betTarget<<setw(15)<<fixed<<setprecision(1)
            <<nguoiChoiAnToan.getTienVon()<<setw(15)
            <<nguoiChoiFibo.getTienVon()<<setw(15)
            <<ngaDauGapDoiDo.getTienVon()<<endl;
            if(nhaCai.getTienVon()<=0)  break;
            if(nguoiChoiAnToan.getTienVon()<=0 && nguoiChoiFibo.getTienVon()<=0 && ngaDauGapDoiDo.getTienVon()<=0)  break;
        }
    }
};

int main(){
    Bet test;
    test.run();
    return 0;
}
