#include "bits/stdc++.h"

using namespace std;
typedef long long ll;
//每个地铁站都有名称(name,4位数字)和编号(id,从0开始)

int N;
#define maxn 10005
#define inf INT_MAX
//根据id获取name
vector<int> id2name;
//根据name获取id
int name2id[maxn];
//使用邻接表存储地铁线路图
vector<vector<int>> edges;
//读取地图时用于记录读入的地铁站是否已被编过号(id存在)
bool flag[maxn] = {0};
//从起点出发到达每个顶点所需的最短距离
int steps[maxn] = {0};
//从起点出发到达每个顶点的最短路径上的前趋点
int prev_station[maxn];
//从起点出发到达每个顶点的换乘次数
int transfer_cnt[maxn] = {0};
namespace std {
    template<>
    struct hash<pair<int, int> > {
        size_t operator()(const pair<int, int> &p) const {
            return p.first * 100 + p.second;
        }
    };

//    template<>
//    struct greater<int> {
//        bool operator()(const int &a, const int &b) const {
//            return steps[a] > steps[b] || steps[a] == steps[b] && transfer_cnt[a] > transfer_cnt[b];
//        }
//    };
}

bool cmp(int a, int b) {
    return steps[a] > steps[b] || steps[a] == steps[b] && transfer_cnt[a] > transfer_cnt[b];
}

unordered_map<pair<int, int>, int> line_affiliation;

//当a是b的前趋或后继时，判断从前趋点走到后继点是否需要换乘
bool is_affiliated_to_the_same_line(int a, int b) {
    int line1 = 0, line2 = 0;
    if (prev_station[a] != a) {
        line1 = line_affiliation[make_pair(prev_station[a], a)];
    }
    if (prev_station[b] != b) {
        line2 = line_affiliation[make_pair(prev_station[b], b)];
    }
    return line1 == 0 || line2 == 0 || line1 == line2;
}

unsigned int get_length(int a, int b) {
    auto pos = find_if(edges[a].begin(), edges[a].end(), [&](int t) {
        return t == b;
    });
    return pos == edges[a].end() ? inf : 1;
}

void dijsktra(int start) {
    fill(begin(steps), end(steps), inf);
    memset(prev_station, -1, sizeof(prev_station));
    fill(begin(transfer_cnt), end(transfer_cnt), inf);
    bool done[maxn] = {0};

    done[start] = 1;
    steps[start] = 0;
    prev_station[start] = start;
    transfer_cnt[start] = 0;
    for (int nxt: edges[start]) {
        steps[nxt] = 1;
        prev_station[nxt] = start;
        transfer_cnt[nxt] = 0;
    }

    for (int i = 1; i < id2name.size(); ++i) {
        int min_dist = inf;
        int min_dist_id = -1;
        for (int j = 0; j < id2name.size(); ++j) {
            if (done[j])continue;
            if (min_dist > steps[j]) {
                min_dist = steps[j];
                min_dist_id = j;
            }
        }
        done[min_dist_id] = 1;
        for (int nxt: edges[min_dist_id]) {
            if (done[nxt])continue;
            if (steps[nxt] > steps[min_dist_id] + 1) {
                steps[nxt] = steps[min_dist_id] + 1;
                prev_station[nxt] = min_dist_id;
                transfer_cnt[nxt] =
                        transfer_cnt[min_dist_id] + (is_affiliated_to_the_same_line(min_dist_id, nxt) ? 0 : 1);
            } else if (steps[nxt] == steps[min_dist_id] + 1) {
                auto &tc_nxt = transfer_cnt[nxt], &tc_cur = transfer_cnt[min_dist_id];
                if (ll(tc_nxt) > ll(tc_cur)) {
                    prev_station[nxt] = min_dist_id;
                    tc_nxt = tc_cur + (is_affiliated_to_the_same_line(min_dist_id, nxt) ? 0 : 1);
                }

            }
        }
    }
}

void optimized_dijsktra(int start) {
    priority_queue<int, vector<int>, decltype(&cmp)> pq(cmp);
    bool done[maxn] = {0};

    fill(begin(steps), end(steps), inf);
    memset(prev_station, -1, sizeof(prev_station));
    fill(begin(transfer_cnt), end(transfer_cnt), inf);

    steps[start] = 0;
    prev_station[start] = start;
    transfer_cnt[start] = 0;
    pq.push(start);

    while (!pq.empty()) {
        auto T = pq.top();
        if (done[T]) {
            pq.pop();
            continue;
        }
        done[T] = 1;
        for (int nxt: edges[T]) {
            if (done[nxt])continue;
            if (steps[nxt] > steps[T] + 1) {
                steps[nxt] = steps[T] + 1;
                prev_station[nxt] = T;
                transfer_cnt[nxt] =
                        transfer_cnt[T] + (is_affiliated_to_the_same_line(T, nxt) ? 0 : 1);
                pq.push(nxt);
            } else if (steps[nxt] == steps[T] + 1) {
                auto &tc_nxt = transfer_cnt[nxt], &tc_cur = transfer_cnt[T];
                if (ll(tc_nxt) > ll(tc_cur)) {
                    prev_station[nxt] = T;
                    tc_nxt = tc_cur + (is_affiliated_to_the_same_line(T, nxt) ? 0 : 1);
                    pq.push(nxt);
                }
            }
        }
//        pq.pop();
    }
}

void DFS(int cur) {
    for (int nxt: edges[cur]) {
        if (steps[nxt] > steps[cur] + 1) {
            steps[nxt] = steps[cur] + 1;
            prev_station[nxt] = cur;
            if (cur != prev_station[cur]) {
                transfer_cnt[nxt] = transfer_cnt[cur] + int(line_affiliation[make_pair(cur, nxt)] !=
                                                            line_affiliation[make_pair(cur, prev_station[cur])]);
            } else {
                transfer_cnt[nxt] = transfer_cnt[cur];
            }
            DFS(nxt);
        } else if (steps[nxt] == steps[cur] + 1) {
            auto &tc_nxt = transfer_cnt[nxt], &tc_cur = transfer_cnt[cur];
            if (ll(tc_nxt) > ll(tc_cur)) {
                prev_station[nxt] = cur;
                if (cur != prev_station[cur]) {
                    transfer_cnt[nxt] = transfer_cnt[cur] + int(line_affiliation[make_pair(cur, nxt)] !=
                                                                line_affiliation[make_pair(cur, prev_station[cur])]);
                } else {
                    transfer_cnt[nxt] = transfer_cnt[cur];
                }
                DFS(nxt);
            }
        }
    }
}

void dfs(int start) {
    fill(begin(steps), end(steps), inf);
    memset(prev_station, -1, sizeof(prev_station));
    fill(begin(transfer_cnt), end(transfer_cnt), inf);

    steps[start] = 0;
    prev_station[start] = start;
    transfer_cnt[start] = 0;
    DFS(start);
}

void bfs(int start) {
    memset(steps, 0, sizeof(steps));
    memset(prev_station, -1, sizeof(prev_station));
    memset(transfer_cnt, 0, sizeof(transfer_cnt));

    queue<int> q;
    q.push(start);
    prev_station[start] = start;
    steps[start] = 0;
    int cur_cnt = 1, level = 1;
    while (!q.empty()) {
        auto cur_station = q.front();
        q.pop();
        for (auto station: edges[cur_station]) {
            if (prev_station[station] == -1) {
                prev_station[station] = cur_station;
                q.push(station);
                steps[station] = level;
                if (!is_affiliated_to_the_same_line(cur_station, station)) {
                    transfer_cnt[station] = transfer_cnt[cur_station] + 1;
                } else {
                    transfer_cnt[station] = transfer_cnt[cur_station];
                }
            } else if (prev_station[station] != -1 && steps[station] > level) {
                prev_station[station] = cur_station;
                q.push(station);
                steps[station] = level;
                if (!is_affiliated_to_the_same_line(cur_station, station)) {
                    transfer_cnt[station] = transfer_cnt[cur_station] + 1;
                } else {
                    transfer_cnt[station] = transfer_cnt[cur_station];
                }
            } else if (prev_station[station] != -1 && steps[station] == level &&
                       transfer_cnt[station] > transfer_cnt[cur_station]) {
                prev_station[station] = cur_station;
                q.push(station);
                steps[station] = level;
                if (!is_affiliated_to_the_same_line(cur_station, station)) {
                    transfer_cnt[station] = transfer_cnt[cur_station] + 1;
                } else {
                    transfer_cnt[station] = transfer_cnt[cur_station];
                }
            }
        }
        cur_cnt--;
        if (cur_cnt == 0) {
            cur_cnt = q.size();
            level++;
        }
    }
}

vector<int> route;
vector<int> lines;
vector<int> sub_ending;

void get_route(int end) {
    if (prev_station[end] == end) {
        route.insert(begin(route), end);
    } else {
        route.insert(begin(route), end);
        return get_route(prev_station[end]);
    }
}

void get_lines() {
    lines.clear();
    sub_ending.clear();
    while (route.size() > 1) {
        int cur_line = line_affiliation[make_pair(route[0], route[1])];
        if (lines.empty() || cur_line != lines.back()) {
            lines.push_back(cur_line);
            sub_ending.push_back(route[0]);
        }
        route.erase(begin(route));
    }
    sub_ending.push_back(route.front());
}

void solve(int start, int end) {
//    bfs(start);
//    dijsktra(start);
    optimized_dijsktra(start);
//    dfs(start);
    route.clear();
    get_route(end);
    get_lines();
    cout << steps[end] << endl;
    for (int j = 0; j < sub_ending.size() - 1; j++) {
        printf("Take Line#%d from %04d to %04d.\n", lines[j], id2name[sub_ending[j]], id2name[sub_ending[j + 1]]);
    }
}

int main() {
//    freopen("case.txt", "r", stdin);
    cin >> N;
    for (int i = 0; i < N; ++i) {
        int M;
        cin >> M;
        int _last;
        for (int j = 0; j < M; ++j) {
            int cur_station;
            cin >> cur_station;
            if (!flag[cur_station]) {
                name2id[cur_station] = id2name.size();
                id2name.push_back(cur_station);
                flag[cur_station] = 1;
                edges.push_back(vector<int>(0));
            }
            if (j > 0) {
                edges[name2id[_last]].push_back(name2id[cur_station]);
                edges[name2id[cur_station]].push_back(name2id[_last]);
                line_affiliation[make_pair(name2id[_last], name2id[cur_station])] = i + 1;
                line_affiliation[make_pair(name2id[cur_station], name2id[_last])] = i + 1;
            }
            _last = cur_station;
        }
    }
    int K;
    cin >> K;
    for (int i = 0; i < K; ++i) {
        int start, end;
        scanf("%d%d", &start, &end);
        start = name2id[start];
        end = name2id[end];
        solve(start, end);
        sub_ending.clear();
        lines.clear();
    }
    return 0;
}