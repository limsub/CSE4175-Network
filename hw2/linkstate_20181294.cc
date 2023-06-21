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
#include <queue>
 
#define MAX 100        // 최대 정점의 개수 
#define INF 99999999
 
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

  
vector< vector<int> > dijkstra(int start, int N, vector< vector<Node> > arr2) {

    // start의 routing table
    vector< vector<int> > dist(N, vector<int>(2, INF));    // 전부 INF로 초기화 

    // 다익스트라 돌리기 위한 우선순위 큐. 여기서만 사용.
    priority_queue<pair<int, int> > pq;
    
    // 자기 자신으로 가는건 (본인, 0)
    dist[start][0] = start;
    dist[start][1] = 0;

    // 일단 초기로 (0, 본인) 큐에 넣어주기. 이거 우선순위 주려고 위에 dist랑은 순서 다르니까 주의
    // 가중치는 -cost로 할 예정 (cost가 작은 것부터 돌기 위해)
    pq.push(make_pair(0, start));    // 시작 정점 방문 
 
    // 큐에 있는거 다 뺄때까지 loop
    while (!pq.empty()) {
        int cost = -pq.top().first;    // 방문한 정점의 dist 값 
        int cur = pq.top().second;    // 현재 방문한 정점 
        pq.pop();
 
        for (size_t i = 0; i < arr2[cur].size(); i++) {    // 현재 방문한 정점의 주변 정점 모두 조사 
            int next = arr2[cur][i].direct_node;    // 조사할 다음 정점 
            int nCost = cost + arr2[cur][i].cost;    // 현재 방문한 정점을 거쳐서 다음 정점을 갈때의 비용 
            
            if (nCost < dist[next][1] ) {     // 기존 비용보다 현재 방문한 정점을 거친 비용이 더 싸다면 
                dist[next][1] = nCost;    // 갱신
                // direct로 연결되어 있는 노드와 그렇지 않은 노드를 다르게 업데이트시킨다
                if (cur == start) {
                    dist[next][0] = next;
                }
                else {
                    dist[next][0] = dist[cur][0];
                }
                // dist[next][0] = dist[cur][0];
                pq.push(make_pair(-nCost, next));    // pq에 저장 
            }
        }
    }
    
    return dist;
}
 
int main(int argc, char *argv[])
{
    // 결국, 차이는
    // (1). 초기 테이블 만드는 알고리즘
    // (2). 테이블이 수정되었을 때 -> 다익스트라를 다시 실행시켜서 싹 다 해야 하나, 아니면 앞에처럼 왔다갔다를 해줘야 하나...

    // 나머지는 일단 다 똑같이 구현해보자.

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
    writeFile.open("output_ls.txt");

    // (dist와 동일) 1. 연결된 노드들을 저장하고 있는 배열 만들기
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


    // 2. routing table 만들기. 마찬가지로 3차원 벡터. 저장된 값은 dist와 동일하고, 저장하는 방법에 차이 주기
    // (1). 어떤 노드의 table인지 (0번 ~ N-1번) -> 크기 N
    // (2). 목적지 노드 (0번 ~ N-1번) -> 크기 N
    // (3). 해당 목적지 노드로 갈 때 필요한 다음 노드와 cost -> 크기 2에 모든 값 10000으로 초기화

    vector< vector <vector<int> > > tables(N);
    // 결국은 이 table을 함수 통해서 받을 거라서 크기만 잡아주자. -> 연결 안되어있는 애들 10000으로 초기화 (5/30) -> 를 어차피 다익스트라 함수 내에서 한다 (5/30)
    // 0 ~ N번째 노드의 테이블을 함수 통해서 받자.
    for (int i = 0; i < N; i++) {
        tables[i] = dijkstra(i, N, arr2);
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
        
        // printf("\n");
        writeFile << "\n";
    }

    

    /*-------------- 메세지 읽기 --------------*/
    // 메세지 읽는 부분은 distance vector랑 100프로 똑같이 구현
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


            if (tables[m_start2][m_dest2][0] > 1000) {
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


    /*-------------- 테이블 업데이트 (arr2, tables) --------------*/
    if (readCFile.is_open()) {

        int x, y, change_cost;
        int testNum;
        
        while (readCFile >> testNum) {
            x = testNum;
            readCFile >> y >> change_cost;

            // 연결을 끊는 경우
            if (change_cost == -999) {
                // arr2 update
                for (size_t j = 0; j < arr2[x].size(); j++) {
                    if (arr2[x][j].direct_node == y) {
                        arr2[x].erase(arr2[x].begin() + j);
                        //printf("hi\n");
                        break;
                    }
                    
                }
                for (size_t j = 0; j < arr2[y].size(); j++) {
                    if (arr2[y][j].direct_node == x) {
                        arr2[y].erase(arr2[y].begin() + j);
                        //printf("hiho\n");
                        break;
                    }
                }
                

                // (x) table update
                // 다익스트라에서는 table을 업데이트 하는게 아니라 아예 처음부터 다시 만들자
            }
            // 연결 cost가 변경된 경우
            else {
                // arr2 update
                int thereIsY = 0;
                for (size_t j = 0; j < arr2[x].size(); j++) {
                    if (arr2[x][j].direct_node == y) {
                        arr2[x][j].cost = change_cost;
                        thereIsY = 1;
                    }
                }
                if (thereIsY == 0) {
                    arr2[x].push_back(*createNode(y, change_cost));
                }

                int thereIsX = 0;
                for (size_t j = 0; j < arr2[y].size(); j++) {
                    if (arr2[y][j].direct_node == x) {
                        arr2[y][j].cost = change_cost;
                        thereIsX = 1;
                    }
                }
                if (thereIsX == 0) {
                    arr2[y].push_back(*createNode(x, change_cost));
                }

                // (x) table update
                // 다익스트라에서는 table을 업데이트 하는게 아니라 아예 처음부터 다시 만들자
            }

            

            

            // table update

            // 클리어
            tables.clear();

            // 크기 배정
            tables.resize(N);

            // 새로 배정
            for (int i = 0; i < N; i++) {
                tables[i] = dijkstra(i, N, arr2);
            }   


            /*----------- update 종료, 출력 시작 -----------*/
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
            //printf("\n");
            writeFile << "\n";
        }

        readCFile.close();
    }
    else {
        printf("Error : open input file.\n");
        exit(0);
    }
    

    printf("Complete. Output file written to output_ls.txt.\n");
    writeFile.close();
    





/*
    int V,E;
    vector<pair<int, int> > adj[MAX];

    cout << "정점의 개수 입력 : ";
    cin >> V;
    cout << "간선의 개수 입력 : ";
    cin >> E;
 
    for (int i = 0; i < E; i++) {
        int from, to, cost;
        cout << "그래프 입력 [정점 정점 가중치]: ";
        cin >> from >> to >> cost;
        adj[from].push_back(make_pair(to, cost));    // 양방향 그래프 
        adj[to].push_back(make_pair(from, cost));
    }
 
    printf("\n===다익스트라 결과===\n");
    vector<int> dist = dijkstra(1, V, adj);
    for (int i = 0; i < V; i++) {
        printf("1번 정점에서 %d번 정점까지 최단거리 : %d\n", i, dist[i]);
    }

*/
    return 0;
}
