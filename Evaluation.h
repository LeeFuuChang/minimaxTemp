#ifndef EVALUATION_H
#define EVALUATION_H


#include <vector>


#define self this->
#define int long long
#define float double


using namespace std;


struct State {
    bool pos[8][8]; // false: x (red), true: o (blue)
    bool exist[8][8];
};
struct srnd{
    int s, a, b, c;
    int dr[2];

    srnd(int s, int a, int b, int c, int d1, int d2){
        self s = s;
        self a = a;
        self b = b;
        self c = c;
        self dr[0] = d1;
        self dr[1] = d2;
    };
};


class AI{
    public:
        int side = 1;

        int boardWeightMultiplier = 4;
        int boardWeight[8][8] = {
            { 64, -32, 16,  8,  8, 16, -32, 64},
            {-32, -16, -8,  1,  1, -8, -16,-32},
            { 16,  -8,  4,  2,  2,  4,  -8, 16},
            {  8,   1,  2,  1,  1,  2,   1,  8},
            {  8,   1,  2,  1,  1,  2,   1,  8},
            { 16,  -8,  4,  2,  2,  4,  -8, 16},
            {-32, -16, -8,  1,  1, -8, -16,-32},
            { 64, -32, 16,  8,  8, 16, -32, 64}
        };

        srnd rnd[4] = {
            srnd( 0, 1, 8, 9, 1, 8),
            srnd( 7, 6,15,14,-1, 8),
            srnd(56,57,48,49, 1,-8),
            srnd(63,62,55,54,-1,-8)
        };
        int dr[8] = {-8,-7, 1, 9, 8, 7,-1,-9};
        int bk[8] = { 8, 0, 0, 0, 8, 7, 7, 7};
        int weight[7] = {10, 801.724, 382.026, 74.396, 78.922, 0, 100};

        AI(){};

        int dire(int i, int d){
            i += dr[d];
            return ( (i&64)!=0 || (i&7) == bk[d] ) ? 64 : i;
        }

        float evaluation(State &s, int available){
            int m[64] = {}, space = 0, board_weight = 0, my_tiles = 0, opp_tiles = 0;
            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    int n = i*8 + j;
                    if(s.exist[i][j]){
                        m[n] = s.pos[i][j]? self side : -self side;
                        board_weight += self boardWeight[i][j]*self side*m[n]*boardWeightMultiplier;
                        my_tiles += (m[n] == self side);
                        opp_tiles += (m[n] == -self side);
                    }else{
                        m[n] = 0;
                        space++;
                    }
                }
            }
            int tileDiff;
            if(my_tiles > opp_tiles) tileDiff = (100.0 * my_tiles)/(my_tiles + opp_tiles);
            else if(my_tiles < opp_tiles) tileDiff = -(100.0 * opp_tiles)/(my_tiles + opp_tiles);
            else tileDiff = 0;



            int corner = 0, steady = 0, uk[64] = {};
            for(int i=0; i<4; i++){
                srnd v = rnd[i];
                if (m[v.s]==0){
                    corner += m[v.a] * -3;
                    corner += m[v.b] * -3;
                    corner += m[v.c] * -6;
                    continue;
                }
                corner += m[v.s] * 15;
                steady += m[v.s];
                for(int j=0; j<2; j++){
                    if(uk[v.s + v.dr[j]]) continue;
                    bool eb = true; int tmp = 0;
                    int k;
                    for(k=1; k<=6; k++){
                        int t = m[v.s+v.dr[j]*k];
                        if(t==0) break;
                        else if(eb && t==m[v.s]) steady += t;
                        else{
                            eb = false;
                            tmp += t;
                        }
                    }
                    if (k==7 && m[v.s+v.dr[k]*7]!=0){
                        steady += tmp;
                        uk[v.s+v.dr[k]*6] = 1;
                    }
                }
            }

            int frontier = 0;
            for(int i=9; i<54; i += ((i&7)==6)?3:1 ){
                if(m[i] == 0) continue;
                for(int j=0; j<8; j++){
                    if( m[self dire(i, j)] == 0 ){
                        frontier -= m[i];
                        break;
                    }
                }
            }

            int mobility = available*self side;

            int parity = space<18 ? ( space%2==0 ? -self side : self side) : 0;

            int score = (
                tileDiff*weight[0]
            ) + (
                corner*weight[1]
            ) + (
                steady*weight[2]
            ) + (
                frontier*weight[3]
            ) + (
                mobility*weight[4]
            ) + (
                parity*weight[5]
            ) + (
                board_weight*weight[6]
            );
            return score * self side;
        };
};


#endif
