#ifndef EVALUATION_H
#define EVALUATION_H


#define int long long
#define float double


// 空格 ->  0
// 紅棋 ->  1
// 藍棋 -> -1
const int EMPTY=0;
const int BLUESIDE=-1;
const int REDSIDE=1;


// 把棋盤權重引入
#include "BoardWeights.h"
// 目前表現最佳的棋盤權重是 boardWeight_Type1
// 建議不管表現好壞都留著
// 不然之後會想不起來測試過哪些權重
#define usingBoardWeight boardWeight_Type1


using namespace std;




struct State { // 記得把原檔案的 struct State 刪掉，不然會二次宣告
    bool pos[8][8]; // false: x (red), true: o (blue)
    bool exist[8][8];
};




/* 向量：
點 (3, 3) 到 點 (4, 5) 的向量為：
 => (4-3, 5-3)

反過來的話

點 (4, 5) 到 點 (3, 3) 的向量為：
 => (3-4, 3-5)

結論：
    向量 = (終點x - 起點x , 終點y - 起點y)
    因此可得
    終點 = (起點x + 向量x , 起點y + 向量y)
*/
struct Point { // 記錄點的位置 也可以用來記錄向量
    int r, c;
    Point(int r, int c){
        this->r=r;
        this->c=c;
    }
};




// 主體
class theAI{
    public:
        int coff[7] = {
            12,      // 角落
            11,     // 邊
            3,      // 非邊
            4,      // 行動力
            1,      // 空格數
            3,      // 子數差
            1      // 棋盤位置
        };
        Point corners[4][6] = {
            {
                Point( 0, 0), // 左上
                Point( 0, 1), // 左上 - 右
                Point( 1, 0), // 左上 - 下
                Point( 1, 1), // 左上 - 右下
                Point( 0, 1), // 左上 - 右   - 向量
                Point( 1, 0)  // 左上 - 下   - 向量
            },{
                Point( 0, 7), // 右上
                Point( 0, 6), // 右上 - 左
                Point( 1, 7), // 右上 - 下
                Point( 1, 6), // 右上 - 左下
                Point( 0,-1), // 右上 - 左   - 向量
                Point( 1, 0)  // 右上 - 下   - 向量
            },{
                Point( 7, 0), // 左下
                Point( 7, 1), // 左下 - 右
                Point( 6, 0), // 左下 - 上
                Point( 6, 1), // 左下 - 右上
                Point( 0, 1), // 左下 - 右   - 向量
                Point(-1, 0)  // 左下 - 上   - 向量
            },{
                Point( 7, 7), // 右下
                Point( 7, 6), // 右下 - 左
                Point( 6, 7), // 右下 - 上
                Point( 6, 6), // 右下 - 左上
                Point( 0,-1), // 右下 - 左   - 向量
                Point(-1, 0)  // 右下 - 上   - 向量
            }
        };
        Point directions[8] = {
            Point(-1,-1), // 左上
            Point(-1, 0), // 中上
            Point(-1, 1), // 右上
            Point( 0,-1), // 中左
            Point( 0, 1), // 中右
            Point( 1,-1), // 左下
            Point( 1, 0), // 中下
            Point( 1, 1)  // 右下 
        };
        int boardWeight[8][8];


        theAI(){};

        int cvt(bool exist, bool piece){
            return (exist)? ( (piece)? BLUESIDE : REDSIDE ) : 0 ;
        }

        int evaluation(State& s, bool redTurn, int availableMoves){
            // Terms
            int 
            cornerScore=0,      // 角落
            sideScore=0,        // 邊
            innerScore=0,       // 非邊
            mobilityScore=0,    // 行動力
            parityScore=0,      // 空格數
            pieceScore=0,       // 子數差
            boardScore=0,       // 棋盤位置
            side=redTurn ? REDSIDE : BLUESIDE;


            // board weight
            int m[8][8]={}, w[8][8]={};
            int redP=0, redW=0, blueP=0, blueW=0, space=0;
            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    boardWeight[i][j] = usingBoardWeight[i][j];
                    w[i][j] = 0;
                    m[i][j] = cvt(s.exist[i][j], s.pos[i][j]);
                    switch( m[i][j] ){
                        case EMPTY:
                            space++;
                            break;
                        
                        case BLUESIDE:
                            blueP++;
                            blueW += boardWeight[i][j];
                            break;

                        case REDSIDE:
                            redP++;
                            redW += boardWeight[i][j];
                            break;
                    }
                }
            }
            pieceScore = ( (redP*redP) - (blueP*blueP) )*side;
            boardScore = ( (redW*redW) - (blueW*blueW) )*side;


            for(int i=0; i<4; i++){
                Point* v = corners[i];

                if( m[ v[0].r ][ v[0].c ]==0 ){
                    cornerScore += m[ v[1].r ][ v[1].c ] * -3;
                    cornerScore += m[ v[2].r ][ v[2].c ] * -3;
                    cornerScore += m[ v[3].r ][ v[3].c ] * -6;
                    continue;
                }

                cornerScore += m[ v[0].r ][ v[0].c ] * 15; // 角落很重要
                sideScore   += m[ v[0].r ][ v[0].c ];        // 角落也算在邊上

                for(int j=4; j<6; j++){
                    if( w[ v[0].r+v[j].r ][ v[0].c+v[j].c ] ) continue; // 已經檢查過

                    int eb=1, tmpSideScore=0, k;
                    for(k=1; k<7; k++){ // 1 個角落連接 6 個邊點
                        int t = m[ v[0].r+v[j].r*k ][ v[0].c+v[j].c*k ];
                        if( t==0 ){
                            break;
                        }else if(eb && t==m[ v[0].r ][ v[0].c ]){ // 與角點連續著 並 同色 的
                            sideScore += t;
                        }else{ // 與角點不同色 或者 不連續的
                            eb = 0;
                            tmpSideScore += t;
                        }
                    }

                    if( k==7 && m[ v[0].r+v[j].r*k ][ v[0].c+v[j].c*k ]!=0 ){ // 這個角點連接的 6 個邊點都被占掉 且 另一個角點也被占掉
                        sideScore += tmpSideScore;
                        w[ v[0].r+v[j].r ][ v[0].c+v[j].c ] = 1;
                    }
                }
            }


            for(int i=1; i<7; i++){
                for(int j=1; j<7; j++){
                    if( !m[i][j] ) continue;
                    for(int k=0; k<8; k++){
                        if( m[ i+directions[k].r ][ i+directions[k].c ] ){
                            innerScore -= m[i][j];
                            break;
                        }
                    }
                }
            }


            mobilityScore = availableMoves*side;


            parityScore = space<18 ? ( space%2==0 ? -side : side ) : 0;

            int finalScore = (
                cornerScore*coff[0]     // 角落
            ) + (
                sideScore*coff[1]       // 邊
            ) + (
                innerScore*coff[2]      // 非邊
            ) + (
                mobilityScore*coff[3]   // 行動力
            ) + (
                parityScore*coff[4]     // 空格數
            ) + (
                pieceScore*coff[5]      // 子數差
            ) + (
                boardScore*coff[6]      // 棋盤位置
            );
            
            // Print Result
            /*
            cout << "side           :" << side << endl;
            cout << "cornerScore    :" << cornerScore*coff[0] << endl;
            cout << "sideScore      :" << sideScore*coff[1] << endl;
            cout << "innerScore     :" << innerScore*coff[2] << endl;
            cout << "mobilityScore  :" << mobilityScore*coff[3] << endl;
            cout << "parityScore    :" << parityScore*coff[4] << endl;
            cout << "pieceScore     :" << pieceScore*coff[5] << endl;
            cout << "boardScore     :" << boardScore*coff[6] << endl;
            cout << "finalScore     :" << finalScore*side << endl;
            */
            return finalScore*side;
        };
} AI;

#endif
