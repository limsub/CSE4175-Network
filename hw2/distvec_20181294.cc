#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <list>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <typeinfo>

using namespace std;

// 연결된 노드 확인하기 위한 vector에 저장될 값들. 하나 기준으로 연결된 노드 번호와 cost 저장
typedef struct _node
{
    int direct_node;
    int cost;
} Node;

// 노드 만드는거
Node *createNode(int x, int y)
{

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->direct_node = x;
    newNode->cost = y;

    return newNode;
}

int main(int argc, char *argv[])
{

    // topology / message / change

    // 인자의 수가 맞지 않을 때
    if (argc != 4)
    {
        printf("usage: distvec topologyfile messagesfile changesfile \n");
        exit(0);
    }

    // file open
    ifstream readTFile; // topology
    ifstream readMFile; // message
    ifstream readCFile; // change
    readTFile.open(argv[1]);
    readMFile.open(argv[2]);
    readCFile.open(argv[3]);

    ofstream writeFile;
    writeFile.open("output_dv.txt");

    

    

    // 1. 배열 하나 만들고 원소를 [인접 노드 번호, cost] 이런 식으로 배열로 넣어주기   -> 생각해보니까 이것도 그냥 3차원 배열로 잡았어도 됬겠다.. Node 막 이런거 사용 안했어도 괜찮았을듯
    vector< vector<Node> > arr2;    // 값이 정해지면 resize로 크기 잡아주기
    int N;  // 값은 파일을 읽은 후에 받는다

    if (readTFile.is_open())
    {
        readTFile >> N;
        arr2.resize(N);

        int x, y, cost;
        int testNum;
        // t.txt 값들로 배열 초기화
        while (readTFile >> testNum)
        {

            x = testNum;
            readTFile >> y >> cost;

            arr2[x].push_back(*createNode(y, cost));
            arr2[y].push_back(*createNode(x, cost));
        }
        readTFile.close();
    }
    else
    {
        printf("Error : open input file.\n");
        exit(0);
    }




    // routing table 만들기
    // (1). 어떤 노드의 table인지 (0번 ~ N-1번) -> 크기 N
    // (2). 목적지 노드 (0번 ~ N-1번) -> 크기 N
    // (3). 해당 목적지 노드로 갈 때 필요한 다음 노드와 cost -> 크기 2에 모든 값 10000으로 초기화
    vector< vector <vector<int> > > tables(N, vector< vector<int> >(N, vector<int>(2, 10000)));

    // 초기 table
    for (int i = 0; i < N; i++)
    {
        // 자기 자신은 무조건 (자기, 0)
        tables[i][i][0] = i;
        tables[i][i][1] = 0;

        // direct로 연결되어 있는 애들만 update
        for (size_t j = 0; j < arr2[i].size(); j++)
        {
            tables[i][arr2[i][j].direct_node][0] = arr2[i][j].direct_node;
            tables[i][arr2[i][j].direct_node][1] = arr2[i][j].cost;
        }
    }


    // 서로서로 update
    int change_flag = 1; // 처음 시작을 위해
    while (change_flag != 0)
    {
        change_flag = 0;
        for (int i = 0; i < N; i++)
        {
            // 자기와 인접한 노드들의 table에 접근한다
            for (size_t j = 0; j < arr2[i].size(); j++)
            {
                // 본인 table의 값과 (상대방 table의 값 + 상대방으로 가는 cost)를 비교해서 작은걸로 update한다. 이 때 바뀌었으면 change_flag를 하나 설정해서 계속 while loop을 돌 수 있게 하자
                for (int k = 0; k < N; k++)
                {
                    if (tables[i][k][1] > tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost)
                    { // 이 순서대로 하니까 ID가 작은 걸 우선적으로 잡지 않을까? -> 확실하진 않다
                        tables[i][k][1] = tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost;
                        tables[i][k][0] = arr2[i][j].direct_node;
                        change_flag = 1;
                    }
                }
            }
        }
    }

    // 테이블 출력
    for (int i = 0; i < N; i++)
    {
        //cout << i << endl;
        
        for (int j = 0; j < N; j++)
        {
            if (tables[i][j][1] < 900) {
                writeFile << j << " " << tables[i][j][0] << " " << tables[i][j][1] << endl;
            }
        }
        //printf("\n");
        writeFile << "\n";
    }




    /*---------------------여기까지 table update---------------------*/

    // message file open
    /*
    1 0 here is a message from 1 to 0
    2 4 this one gets sent from 2 to 4!
    */

    // 와 while문 안에서 뭐 해보려니까 계속 segfault 떠서 걍 string vector만들어서 거기다가 message file에 있는거 한줄씩 다 저장하자
    // 이거 segfault 다른 이유인 것 같아서 다시 while문 안에서 하는걸로 시도한다
    // 아 근데 어차피 나중에 업데이트 할 때도 같은 메세지로 출력해줘야 해서 vector에 저장하는 게 나을 것 같다. 파일 오픈 반복하는건 너무 낭비인듯
    vector<string> in_messageFile;

    if (readMFile.is_open())
    {
        //cout << "message file is open" << endl;

        string m_messaged2;

        while (getline(readMFile, m_messaged2))
        {
            //cout << m_messaged << endl;
            in_messageFile.push_back(m_messaged2);  // 일단 벡터에 메시지 저장만

        }
        readMFile.close();
    }
    else
    {
        printf("Error : open input file.\n");
        exit(0);
    }


    // 본격적으로 모든 메세지 뜯어서 분석
    for (size_t i = 0; i < in_messageFile.size(); i++) {
        string m_messaged = in_messageFile[i];


            // 공백으로 문자열 걸러서 문장 뜯어내기
            int m_start2;
            int m_dest2;
            string m_message2;

            int space_cnt = 0;
            int first_space_idx;
            int second_space_idx;
            for (size_t t = 0; t < m_messaged.size(); t++) {
                if (m_messaged[t] == ' ') {
                    space_cnt++;
                    //printf("space_cnt ++, 현재 위치 : %d\n", t);

                    if (space_cnt == 1) {
                    first_space_idx = t;
                    //printf("space_cnt 1이기 때문에 first idx 지정\n");
                    }
                    if (space_cnt == 2) {
                    second_space_idx = t;
                    //printf("space_cnt 1이기 때문에 second idx 지정\n");
                    }
                }
                
            }
            m_start2 = atoi(m_messaged.substr(0, first_space_idx).c_str());
            m_dest2 = atoi(m_messaged.substr(first_space_idx+1, second_space_idx).c_str());
            m_message2 = m_messaged.substr(second_space_idx+1, m_messaged.length() - 1);

            //cout << first_space_idx << " " << second_space_idx << endl;
            //cout << m_start2 << " " << m_dest2 << " " << m_message2 << endl;


            // 길이 존재하긴 한건지 확인
            // 길이 없는 경우
            if ( tables[m_start2][m_dest2][0] > 1000) {
                writeFile << "from " << m_start2 << " to " << m_dest2 << " cost infinite hops unreachable message ";
                writeFile << m_message2 << endl;
            }
            // 길이 있는 경우
            else {
                // 메세지에 적힌 start node와 destination node로 가기 위한 루트 (노드들)
                vector<int> route;
                int cost = tables[m_start2][m_dest2][1];    // cost는 바로 알 수 있다.

                int initial_start2 = m_start2;              // m_start 값을 계속 바꿀 예정이기 때문에 나중에 start node 출력용으로 하나 써둔다

                while (m_start2 != m_dest2) {
                    route.push_back(m_start2);
                    m_start2 = tables[m_start2][m_dest2][0];
                }

                // 출력

                writeFile << "from " << initial_start2 << " to " << m_dest2 << " cost " << cost << " hops ";

                for (size_t i = 0; i < route.size(); i++)
                {
                    //printf("%d ", route[i]);
                    writeFile << route[i] << " ";
                }

                writeFile << "message " << m_message2 << endl;
            }
    }
    writeFile << "\n";
    //printf("\n");





/*
    for (size_t i = 0; i < in_messageFile.size(); i++) {
        int m_start2;
        int m_dest2;
        string m_message2;

        int space_cnt = 0;
        int space_idx = 0;
        for (size_t j = 0; j < in_messageFile[i].length(); j++) {
            if (in_messageFile[i][j] == ' ' ) {
                space_cnt++;
                if (space_cnt == 2 ) {
                    space_idx = j;
                    break;
                }
            }
        }
        printf("%d\n", space_idx);

        // 첫번째 숫자, 두 번째 숫자, 나머지 문자열
        string a1, a2;
        stringstream s(in_messageFile[i]);
        s >> a1 >> a2;

        //m_start2 = 8;
        //m_dest2 = atoi(a2.c_str());

        m_message2 = in_messageFile[i].substr(space_idx);

        //printf("1 : %d\n2 : %d\n3 : %s\n", m_start2, m_dest2, m_message2);
    }
*/
/*
    int m_start = 2;
    int m_dest = 4;
    string m_message = "this is from 1 to 0";
    int initial_start = m_start;

    int cost = tables[m_start][m_dest][1];
    vector<int> route; // 루트 저장할 vector
    while (m_start != m_dest)
    {
        route.push_back(m_start);
        m_start = tables[m_start][m_dest][0];
    }
*/

/*
    cout << "from " << initial_start << " to " << m_dest << " cost " << cost << " hops ";

    for (size_t i = 0; i < route.size(); i++)
    {
        printf("%d ", route[i]);
    }

    cout << "message " << m_message << endl;
*/
    

    /*---------------------여기까지 message---------------------*/

    // change -999일 때와 아닐 때로 나눠서 해야겠다.
    // (1). 1 3 1
    // (2). 1 3 -999

    //cout << "/*---------------------여기까지 message---------------------*/" << endl;


    // update 시작할 건데...
    // update -> message 출력 -> update -> message 출력 -> ... 이런 식으로 해야 하기 때문에 어쩔 수 없이 while문 돌린다
    // 한번에 다 읽고 저장 vs 읽으면서 -> 읽으면서 하는걸로 하자. 어차피 각자 한 번씩만 읽는거니까.
    
    if (readCFile.is_open()) {

        int x, y, change_cost;
        int testNum;

        while (readCFile >> testNum)
        {
            x = testNum;
            readCFile >> y >> change_cost;

            //int x = 1;
            //int y = 3;
            //int change_cost = 1;

            //scanf("%d %d %d", &x, &y, &change_cost); // 그냥 연습용으로 하는거
            /*if (x == -1)
            {
                break;
            }*/


            // infinite hop이 생기는 이유는 아마 여기서 생길거야
            if (change_cost == -999)
            {
                // arr2 update
                //printf("cost가 -999로 입력되었기 때문에 erase를 진행합니다\n");
                // x에 대해
                for (size_t j = 0; j < arr2[x].size(); j++)
                { // 이미 있었으면 링크 끊어주고, 애초에 없던 거면 아무것도 할 거 없다
                    if (arr2[x][j].direct_node == y)
                    {
                        //printf("%d - %d가 있기 때문에 제거합니다.\n", x, y);
                        arr2[x].erase(arr2[x].begin() + j);
                        break;
                    }
                }
                // y에 대해
                for (size_t j = 0; j < arr2[y].size(); j++)
                {
                    if (arr2[y][j].direct_node == x)
                    {
                        //printf("%d - %d가 있기 때문에 제거합니다.\n", y, x);
                        arr2[y].erase(arr2[y].begin() + j);
                        break;
                    }
                }

                // 만약 서로 direct로 연결되어 있는데, 그 방향으로 가는게 최선인 경우가 있다면, 그걸 초기화해준다.
                for (int j = 0; j < N; j++)
                {
                    if (tables[x][j][0] == y)
                    {
                        tables[x][j][0] = 10000;
                        tables[x][j][1] = 10000;
                    }
                    if (tables[y][j][0] == x)
                    {
                        tables[y][j][0] = 10000;
                        tables[y][j][1] = 10000;
                    }
                }
            }
            else
            {
                // arr2 update (if 원래 값이 있었으면 값 변경, else 새로운 노드로 추가) -> 이거 해야하냐 근데..? -> update돌 때 본인 인접한 노드 개수를 이걸로 확인하기 때문에 해야 한다
                // x에 대해
                int thereIsY = 0;
                for (size_t j = 0; j < arr2[x].size(); j++)
                {
                if (arr2[x][j].direct_node == y)
                { // 이미 연결되어 있었으면 cost만 변경
                    arr2[x][j].cost = change_cost;
                    thereIsY = 1;
                }
                }
                if (thereIsY == 0)
                { // 애초에 연결이 안되어있었으면 새로운 노드로 추가
                    arr2[x].push_back(*createNode(y, change_cost));
                }
                // y에 대해
                int thereIsX = 0;
                for (size_t j = 0; j < arr2[y].size(); j++)
                {
                if (arr2[y][j].direct_node == x)
                {
                    arr2[y][j].cost = change_cost;
                    thereIsX = 1;
                }
                }
                if (thereIsX == 0)
                {
                arr2[y].push_back(*createNode(x, change_cost));
                }



                // table update -> 이거 단순히 direct인 애들만 update하면 안되겠다
                // x에 대해
                if (tables[x][y][1] > change_cost || (tables[x][y][1] == change_cost && tables[x][y][0] > y))
                { // 만약 값이 같으면 ID가 작을 때만 바꿔준다
                tables[x][y][1] = change_cost;
                tables[x][y][0] = y;
                }
                // y에 대해
                if (tables[y][x][1] > change_cost || (tables[y][x][1] == change_cost && tables[y][x][0] > x))
                { // 만약 값이 같으면 ID가 작을 때만 바꿔준다
                tables[y][x][1] = change_cost;
                tables[y][x][0] = x;
                }
            }

            // update 해주기 (기존꺼 복붙 -> 근데 -999일 때는 잘 안먹는듯..? 아.. direct_node가 얘인지도 확인을 해야한다)
            int change_flag2 = 1; // 처음 시작을 위해
            while (change_flag2 != 0)
            {

                change_flag2 = 0;
                for (int i = 0; i < N; i++)
                {

                // 자기와 인접한 노드들의 table에 접근한다
                for (size_t j = 0; j < arr2[i].size(); j++)
                {

                    // 본인 table의 값과 (상대방 table의 값 + 상대방으로 가는 cost)를 비교해서 작은걸로 update한다. 이 때 바뀌었으면 change_flag를 하나 설정해서 계속 while loop을 돌 수 있게 하자
                    for (int k = 0; k < N; k++)
                    {

                        // 단순히 값이 작은 경우. 더 좋은 cost인 경우
                        if (tables[i][k][1] > tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost)
                        { // 이 순서대로 하니까 ID가 작은 걸 우선적으로 잡지 않을까? -> 확실하진 않다
                            tables[i][k][1] = tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost;
                            tables[i][k][0] = arr2[i][j].direct_node;
                            change_flag2 = 1;
                            
                        }

                        // direct node였던 애가 값이 바뀌었을 경우? 갑자기 더 높아진 경우
                        // tables[i][k][0] == arr2[i][j].direct_node; // 원래 가는 길이 얘를 통해서 가는 길이었어 (i가 k를 갈 때, j를 거쳐서 가는거였어)
                        // tables[i][k][1] < tables[arr2[i][j].direct_node][k][1];// 근데 얘 값이 이상해

                        if (tables[i][k][0] == arr2[i][j].direct_node && tables[i][k][1] != tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost)
                        {
                            tables[i][k][1] = tables[arr2[i][j].direct_node][k][1] + arr2[i][j].cost;
                            change_flag2 = 1;

                            if (tables[i][k][1] > 1000) {
                                change_flag2 = 0;
                            }
                            //printf("ho\n");
                        }



                    }
                }
                }
            }


            // 테이블 출력
            for (int i = 0; i < N; i++)
            {
                //cout << i << endl;
                for (int j = 0; j < N; j++)
                {
                    if (tables[i][j][1] < 900) {
                        writeFile << j << " " << tables[i][j][0] << " " << tables[i][j][1] << endl;
                    }
                }
                //printf("\n");
                writeFile << "\n";
            }

            // 메세지 출력
            for (size_t i = 0; i < in_messageFile.size(); i++)
            {
                string m_messaged = in_messageFile[i];

                // 공백으로 문자열 걸러서 문장 뜯어내기
                int m_start2;
                int m_dest2;
                string m_message2;

                int space_cnt = 0;
                int first_space_idx;
                int second_space_idx;
                for (size_t t = 0; t < m_messaged.size(); t++)
                {
                    if (m_messaged[t] == ' ') {
                        space_cnt++;
                        //printf("space_cnt ++, 현재 위치 : %d\n", t);

                        if (space_cnt == 1) {
                        first_space_idx = t;
                        //printf("space_cnt 1이기 때문에 first idx 지정\n");
                        }
                        if (space_cnt == 2) {
                        second_space_idx = t;
                        //printf("space_cnt 1이기 때문에 second idx 지정\n");
                        }
                    }
                
                }
                m_start2 = atoi(m_messaged.substr(0, first_space_idx).c_str());
                m_dest2 = atoi(m_messaged.substr(first_space_idx+1, second_space_idx).c_str());
                m_message2 = m_messaged.substr(second_space_idx+1, m_messaged.length() - 1);

                //cout << first_space_idx << " " << second_space_idx << endl;
                //cout << m_start2 << " " << m_dest2 << " " << m_message2 << endl;



                if ( tables[m_start2][m_dest2][0] > 1000) {
                    writeFile << "from " << m_start2 << " to " << m_dest2 << " cost infinite hops unreachable message ";
                    writeFile << m_message2 << endl;
                }
                // 길이 있는 경우
                else {
                    // 메세지에 적힌 start node와 destination node로 가기 위한 루트 (노드들)
                    vector<int> route;
                    int cost = tables[m_start2][m_dest2][1];    // cost는 바로 알 수 있다.

                    int initial_start2 = m_start2;              // m_start 값을 계속 바꿀 예정이기 때문에 나중에 start node 출력용으로 하나 써둔다

                    while (m_start2 != m_dest2) {
                        route.push_back(m_start2);
                        m_start2 = tables[m_start2][m_dest2][0];
                    }

                    // 출력

                    writeFile << "from " << initial_start2 << " to " << m_dest2 << " cost " << cost << " hops ";

                    for (size_t i = 0; i < route.size(); i++)
                    {
                        //printf("%d ", route[i]);
                        writeFile << route[i] << " ";
                    }

                    writeFile << "message " << m_message2 << endl;
                }
            }
            //printf("\n");
            writeFile << "\n";
        }
        
        
        readCFile.close();
    }
    else {
        printf("Error : open input file.\n");
        exit(0);
    }
    
    printf("Complete. Output file written to output_dv.txt.\n");
    writeFile.close();
    

    //printf("ok\n");
    return 0;
}
