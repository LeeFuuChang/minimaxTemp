#include <bits/stdc++.h>


#define self this->
#define int long long
#define float double


using namespace std;


const int boardWeightMultiplier = 4;
const int boardWeight[8][8] = {
    { 64, -32, 16,  8,  8, 16, -32, 64},
    {-32, -16, -8,  1,  1, -8, -16,-32},
    { 16,  -8,  4,  2,  2,  4,  -8, 16},
    {  8,   1,  2,  1,  1,  2,   1,  8},
    {  8,   1,  2,  1,  1,  2,   1,  8},
    { 16,  -8,  4,  2,  2,  4,  -8, 16},
    {-32, -16, -8,  1,  1, -8, -16,-32},
    { 64, -32, 16,  8,  8, 16, -32, 64}
};


struct State {
    bool pos[8][8]; // false: x (red), true: o (blue)
    bool exist[8][8];
};


struct Point { // 閮?暺?雿蔭 銋隞亦靘?????
    int r, c;
    Point(int r, int c){
        this->r=r;
        this->c=c;
    }
};
bool operator== (const Point &p1, const Point &p2){
	return (p1.r==p2.r and p1.c==p2.c);
};
bool operator!= (const Point &p1, const Point &p2){
	return !(p1==p2);
};
Point corners[4][6] = {
    {
        Point( 0, 0),
        Point( 0, 1),
        Point( 1, 0),
        Point( 1, 1),
        Point( 0, 1),
        Point( 1, 0)
    },{
        Point( 0, 7),
        Point( 0, 6),
        Point( 1, 7),
        Point( 1, 6),
        Point( 0,-1),
        Point( 1, 0)
    },{
        Point( 7, 0),
        Point( 7, 1),
        Point( 6, 0),
        Point( 6, 1),
        Point( 0, 1),
        Point(-1, 0)
    },{
        Point( 7, 7),
        Point( 7, 6),
        Point( 6, 7),
        Point( 6, 6),
        Point( 0,-1),
        Point(-1, 0)
    }
};
Point directions[8] = {
    Point(-1,-1),
    Point(-1, 0),
    Point(-1, 1),
    Point( 0,-1),
    Point( 0, 1),
    Point( 1,-1),
    Point( 1, 0),
    Point( 1, 1)
};


int weight[7] = {10, 801.724, 382.026, 74.396, 78.922, 0, 100};


/*
 7 + -1*6 =  1
 0 +  1*6 =  6
56 + -8*6 =  8
63 + -8*6 = 15
 0 +  8*6 = 48
 7 +  8*6 = 55
63 + -1*6 = 57
56 +  1*6 = 62


 0  1  2  3  4  5  6  7
 8  9 10 11 12 13 14 15
16 17 18 19 20 21 22 23
24 25 26 27 28 29 30 31
32 33 34 35 36 37 38 39
40 41 42 43 44 45 46 47
48 49 50 51 52 53 54 55
56 57 58 59 60 61 62 63
*/



float evaluation(const State &cs, int available){
    int 
    cornerScore=0,
    stabilityScore=0,
    innerScore=0,
    mobilityScore=0,
    parityScore=0,
    tileScore=0,
    boardScore=0;

	int space=8*8, myTiles=0, opTiles=0, m[8][8]={};

	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			m[i][j] = 0;
			if( cs.exist[i][j] ){
				space--;
				m[i][j] = cs.pos[i][j]? 1 : -1;
				boardScore += boardWeight[i][j]*boardWeightMultiplier*m[i][j];
				myTiles += (m[i][j] == 1);
				opTiles += (m[i][j] ==-1);
			}
		}
	}
	if(myTiles > opTiles){
		tileScore =  (100 * myTiles) / (myTiles + opTiles);
	}else if(myTiles < opTiles){
		tileScore = -(100 * opTiles) / (myTiles + opTiles);
	}else{
		tileScore = 0;
	}


	int uk[10]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(int i=0; i<4; i++){
		Point &s = corners[i][0];
		Point &a = corners[i][1];
		Point &b = corners[i][2];
		Point &c = corners[i][3];

		if( m[s.r][s.c]==0 ){
			cornerScore += m[a.r][a.c]*-3;
			cornerScore += m[b.r][b.c]*-3;
			cornerScore += m[c.r][c.c]*-6;
			continue;
		}
		cornerScore += m[s.r][s.c]*15;
		stabilityScore += m[s.r][s.c];
		for(int j=0; j<2; j++){
			Point &d = corners[i][j+4];
			if( uk[ ( (s.r+d.r)*8 + (s.c+d.c) )%12 ] ) continue;
			bool eb=true; int tmp=0, k;
			for(k=1; k<7; k++){
				int t = m[ s.r+d.r*k ][ s.c+d.c*k ];
				if( t==0 ){
					break;
				}else if( eb and t==m[s.r][s.c] ){
					stabilityScore += t;
				}else{
					eb = false;
					tmp += t;
				}
			}
			if( k==7 and m[ s.r+d.r*k ][ s.c+d.c*k ]*k!=0 ){
				stabilityScore += tmp;
				uk[ ( (s.r+d.r*6)*8 + (s.c+d.c*6) )%12 ] = 1;
			}
		}
	}


	for(int i=1; i<7; i++){
		for(int j=1; j<7; j++){
			if( m[i][j]==0 ) continue;
			for(int d=0; d<8; d++){
				if( m[ i+directions[d].r ][ j+directions[d].c ] == 0 ){
					innerScore -= m[i][j];
					break;
				}
			}
		}
	}


	mobilityScore = available;


	parityScore = space<18 ? ( space%2==0 ? -1 : 1) : 0;


    int score = (
        tileScore     *weight[0]
    ) + (
        cornerScore   *weight[1]
    ) + (
        stabilityScore*weight[2]
    ) + (
        innerScore    *weight[3]
    ) + (
        mobilityScore *weight[4]
    ) + (
        parityScore   *weight[5]
    ) + (
        boardScore    *weight[6]
    );
    return score;
}



int a[8][8] = {
    {1, 0, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0},
    {1, 0, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 1},
    {1, 1, 1, 0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0}
};
int b[8][8] = {
    {0, 0, 1, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0}
};
signed main(){
	State ns;
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			ns.exist[i][j] = a[i][j];
			ns.pos[i][j] = b[i][j];
		}
	}

	int sc = evaluation(ns, 12);
	cout << sc;
}




















