
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstring>

const int MAX_NODES = 9; // 图中节点总数

struct Graph {  // 定义图
    int adjMatrix[MAX_NODES][MAX_NODES]; // 邻接矩阵
    int vertexNum; // 顶点数

    Graph(int n) : vertexNum(n) {  // 初始化图
        // 初始化邻接矩阵，所有元素设为0（表示无边）
        for (int i = 0; i < vertexNum; ++i) {
            for (int j = 0; j < vertexNum; ++j) {
                adjMatrix[i][j] = 0;
            }
        }
    }

    void addEdge(int u, int v) {  // 添加无向边
        if (u >= 0 && u < vertexNum && v >= 0 && v < vertexNum) {
            adjMatrix[u][v] = 1;
            adjMatrix[v][u] = 1; // 无向图，双向连接
        }
    }

    void printMatrix() {  // 输出邻接矩阵
        std::cout << "图的邻接矩阵表示:" << std::endl;
        std::cout << "  ";
        for (int i = 0; i < vertexNum; ++i) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < vertexNum; ++i) {
            std::cout << i << " ";
            for (int j = 0; j < vertexNum; ++j) {
                std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

std::vector<int> bfs_shortest_path(const Graph& graph, int start, int end) {
    if (start == end) {
        return std::vector<int>{start};
    }
    bool visited[MAX_NODES] = {false};  // 记录节点是否被访问过
    int predecessor[MAX_NODES];  // 记录每个节点的前驱节点，用于回溯路径
    std::memset(predecessor, -1, sizeof(predecessor));
    std::queue<int> q;
    visited[start] = true;  // 从起点开始BFS
    q.push(start);
    predecessor[start] = -1; // 起点没有前驱
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        std::cout << "访问节点: " << current << std::endl;
        // 遍历当前节点的所有邻居
        for (int neighbor = 0; neighbor < graph.vertexNum; neighbor++) {
            // 如果存在边且邻居节点未被访问
            if (graph.adjMatrix[current][neighbor] == 1 && !visited[neighbor]) {
                std::cout << "  检查节点 " << current << " 的邻居节点 " << neighbor << std::endl;
                visited[neighbor] = true;
                std::cout << "  将节点 " << neighbor << " 标记为已访问" << std::endl;
                predecessor[neighbor] = current; // 记录前驱节点
                std::cout << "  将节点 " << neighbor << " 的前驱节点设为 " << current << std::endl;
                q.push(neighbor);
                std::cout << "  将节点 " << neighbor << " 加入队列\n";
                // 如果找到目标节点，回溯构建路径
                if (neighbor == end) {
                    std::cout << "找到目标节点 " << end << "，开始回溯路径。" << std::endl;
                    std::vector<int> path;
                    int node = end;
                    // 从终点回溯到起点
                    while (node != -1) {
                        path.push_back(node);
                        node = predecessor[node];
                    }
                    std::reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }
    return std::vector<int>{}; // 未找到路径
}

int test_shortest_path_bfs() {
    // 创建图结构体实例，包含9个节点和8条边
    Graph graph(MAX_NODES);
    int edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4},
        {4, 5}, {3, 6}, {2, 8}, {8, 7},
    };
    int edge_count = 8;
    for (int i = 0; i < edge_count; i++) {
        graph.addEdge(edges[i][0], edges[i][1]);
    }
    // 输出邻接矩阵
    graph.printMatrix();
    // 查找从结点0到结点6的最短路径
    int start_node = 0;
    int end_node = 6;
    std::vector<int> shortest_path = bfs_shortest_path(graph, start_node, end_node);
    std::cout << '\n';
    if (!shortest_path.empty()) {
        std::cout << "从结点" << start_node << "到结点" << end_node << "的最短路径: ";
        for (std::size_t i = 0; i < shortest_path.size(); i++) {
            std::cout << shortest_path[i];
            if (i != shortest_path.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
        std::cout << "最短路径长度: " << shortest_path.size() - 1 << " 条边" << std::endl;
    } else {
        std::cout << "未找到从结点" << start_node << "到结点" << end_node << "的路径" << std::endl;
    }
    return 0;
}
