/*
    ============================================================
     SMART NAVIGATION SYSTEM  (Google Maps Mini)
    ============================================================
    Language     : C++ (Standard : C++11, works in Dev-C++ / Code::Blocks)
    Single File  : Yes (everything in one .cpp so it's easy to
                   open, compile and run in Dev-C++)

    Algorithms Implemented
      - Dijkstra's Algorithm      (shortest path, weighted graph)
      - BFS                       (level-order traversal)
      - DFS                       (depth-first traversal)
      - A* Search                 (heuristic shortest path)
      - Priority Queue (Min-Heap) (STL priority_queue, used by
                                    Dijkstra & A*)
      - Graph (Adjacency List)    (vector<vector<Edge>>)
      - File Handling             (Save Graph / Load Graph)

    How to run in Dev-C++
      1) File -> New -> Source File
      2) Paste this whole code
      3) Save as navigation_system.cpp
      4) Execute -> Compile & Run  (F11)
    ============================================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <algorithm>

using namespace std;

const double INF = numeric_limits<double>::infinity();

// ------------------------------------------------------------
//  DATA STRUCTURES
// ------------------------------------------------------------

// A city is a node in the graph. x,y are used only for the
// A* heuristic (straight-line / "as the crow flies" distance).
struct City {
    string name;
    double x, y;
};

// A road is a weighted directed edge in the adjacency list.
// weight = distance / travel-time between two cities.
struct Edge {
    int to;
    double weight;
};

// ------------------------------------------------------------
//  GRAPH CLASS  (Adjacency List based Graph)
// ------------------------------------------------------------
class NavigationGraph {
private:
    vector<City> cities;                    // list of all cities
    unordered_map<string, int> cityIndex;   // name -> index lookup
    vector<vector<Edge>> adj;               // adjacency list

    bool trafficMode = false;
    double trafficMultiplier = 1.5;         // how much traffic slows you down

    // Straight-line distance heuristic used by A*
    double heuristic(int a, int b) const {
        double dx = cities[a].x - cities[b].x;
        double dy = cities[a].y - cities[b].y;
        return sqrt(dx * dx + dy * dy);
    }

    // Reconstructs and prints a path from parent[] array
    void printPath(const vector<int>& parent, int dest) const {
        vector<int> path;
        for (int at = dest; at != -1; at = parent[at])
            path.push_back(at);
        reverse(path.begin(), path.end());

        for (size_t i = 0; i < path.size(); i++) {
            cout << cities[path[i]].name;
            if (i + 1 < path.size()) cout << " -> ";
        }
        cout << "\n";
    }

    double edgeCost(const Edge& e) const {
        return trafficMode ? e.weight * trafficMultiplier : e.weight;
    }

public:
    // ---------------- Add City / Add Road ----------------
    bool cityExists(const string& name) const {
        return cityIndex.find(name) != cityIndex.end();
    }

    void addCity(const string& name, double x = 0, double y = 0) {
        if (cityExists(name)) {
            cout << "City \"" << name << "\" already exists.\n";
            return;
        }
        City c{name, x, y};
        cities.push_back(c);
        adj.push_back(vector<Edge>());
        cityIndex[name] = (int)cities.size() - 1;
        cout << "City \"" << name << "\" added successfully.\n";
    }

    // internal add used while loading from file (no duplicate check message)
    void addCitySilent(const string& name, double x, double y) {
        City c{name, x, y};
        cities.push_back(c);
        adj.push_back(vector<Edge>());
        cityIndex[name] = (int)cities.size() - 1;
    }

    void addRoad(const string& from, const string& to, double weight, bool bidirectional = true) {
        if (!cityExists(from) || !cityExists(to)) {
            cout << "Error: both cities must exist before adding a road.\n";
            return;
        }
        int u = cityIndex[from];
        int v = cityIndex[to];
        adj[u].push_back(Edge{v, weight});
        if (bidirectional) adj[v].push_back(Edge{u, weight});
        cout << "Road added: " << from << (bidirectional ? " <-> " : " -> ") << to
             << " (weight = " << weight << ")\n";
    }

    void addRoadSilent(int u, int v, double weight) {
        adj[u].push_back(Edge{v, weight});
    }

    int getIndex(const string& name) const {
        auto it = cityIndex.find(name);
        return (it == cityIndex.end()) ? -1 : it->second;
    }

    int cityCount() const { return (int)cities.size(); }

    // ---------------- Display Map ----------------
    void displayMap() const {
        if (cities.empty()) {
            cout << "No cities added yet.\n";
            return;
        }
        cout << "\n----------------- MAP -----------------\n";
        for (int i = 0; i < (int)cities.size(); i++) {
            cout << "[" << i << "] " << cities[i].name
                 << " (x=" << cities[i].x << ", y=" << cities[i].y << ")\n";
            if (adj[i].empty()) {
                cout << "      -> (no roads)\n";
                continue;
            }
            for (auto& e : adj[i]) {
                cout << "      -> " << cities[e.to].name
                     << "  [weight: " << e.weight << "]\n";
            }
        }
        cout << "Traffic Mode: " << (trafficMode ? "ON (x1.5 delay)" : "OFF") << "\n";
        cout << "----------------------------------------\n";
    }

    // ---------------- Traffic Mode ----------------
    void toggleTraffic() {
        trafficMode = !trafficMode;
        cout << "Traffic mode is now " << (trafficMode ? "ON" : "OFF") << ".\n";
    }

    // ---------------- BFS ----------------
    void bfs(const string& startName) const {
        int start = getIndex(startName);
        if (start == -1) { cout << "City not found.\n"; return; }

        vector<bool> visited(cities.size(), false);
        queue<int> q;
        visited[start] = true;
        q.push(start);

        cout << "BFS Traversal from " << startName << ":\n";
        while (!q.empty()) {
            int u = q.front(); q.pop();
            cout << cities[u].name << " ";
            for (auto& e : adj[u]) {
                if (!visited[e.to]) {
                    visited[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        cout << "\n";
    }

    // ---------------- DFS ----------------
    void dfsUtil(int u, vector<bool>& visited) const {
        visited[u] = true;
        cout << cities[u].name << " ";
        for (auto& e : adj[u]) {
            if (!visited[e.to]) dfsUtil(e.to, visited);
        }
    }

    void dfs(const string& startName) const {
        int start = getIndex(startName);
        if (start == -1) { cout << "City not found.\n"; return; }

        vector<bool> visited(cities.size(), false);
        cout << "DFS Traversal from " << startName << ":\n";
        dfsUtil(start, visited);
        cout << "\n";
    }

    // ---------------- Dijkstra ----------------
    void dijkstra(const string& fromName, const string& toName) const {
        int src = getIndex(fromName);
        int dest = getIndex(toName);
        if (src == -1 || dest == -1) { cout << "City not found.\n"; return; }

        int n = (int)cities.size();
        vector<double> dist(n, INF);
        vector<int> parent(n, -1);
        // min-heap : pair<distance, node>
        priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;

        dist[src] = 0;
        pq.push(make_pair(0.0, src));

        while (!pq.empty()) {
            double d = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            if (d > dist[u]) continue; // stale entry

            for (auto& e : adj[u]) {
                double w = edgeCost(e);
                if (dist[u] + w < dist[e.to]) {
                    dist[e.to] = dist[u] + w;
                    parent[e.to] = u;
                    pq.push(make_pair(dist[e.to], e.to));
                }
            }
        }

        if (dist[dest] == INF) {
            cout << "No route exists between " << fromName << " and " << toName << ".\n";
            return;
        }

        cout << "\n[Dijkstra] Shortest route " << fromName << " -> " << toName
             << "  (cost = " << dist[dest] << ")\n";
        cout << "Path: ";
        printPath(parent, dest);
    }

    // ---------------- A* Search ----------------
    void astar(const string& fromName, const string& toName) const {
        int src = getIndex(fromName);
        int dest = getIndex(toName);
        if (src == -1 || dest == -1) { cout << "City not found.\n"; return; }

        int n = (int)cities.size();
        vector<double> gScore(n, INF);
        vector<int> parent(n, -1);
        vector<bool> closed(n, false);

        // min-heap : pair<fScore, node>
        priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;

        gScore[src] = 0;
        pq.push(make_pair(heuristic(src, dest), src));

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (u == dest) break;
            if (closed[u]) continue;
            closed[u] = true;

            for (auto& e : adj[u]) {
                double w = edgeCost(e);
                double tentative = gScore[u] + w;
                if (tentative < gScore[e.to]) {
                    gScore[e.to] = tentative;
                    parent[e.to] = u;
                    double f = tentative + heuristic(e.to, dest);
                    pq.push(make_pair(f, e.to));
                }
            }
        }

        if (gScore[dest] == INF) {
            cout << "No route exists between " << fromName << " and " << toName << ".\n";
            return;
        }

        cout << "\n[A*] Shortest route " << fromName << " -> " << toName
             << "  (cost = " << gScore[dest] << ")\n";
        cout << "Path: ";
        printPath(parent, dest);
    }

    // ---------------- Save Graph to File ----------------
    void saveGraph(const string& filename) const {
        ofstream out(filename);
        if (!out) { cout << "Error: could not open file for writing.\n"; return; }

        out << cities.size() << "\n";
        for (auto& c : cities)
            out << c.name << " " << c.x << " " << c.y << "\n";

        int totalEdges = 0;
        for (auto& list : adj) totalEdges += (int)list.size();
        out << totalEdges << "\n";

        for (int u = 0; u < (int)adj.size(); u++)
            for (auto& e : adj[u])
                out << u << " " << e.to << " " << e.weight << "\n";

        out.close();
        cout << "Graph saved to \"" << filename << "\" successfully.\n";
    }

    // ---------------- Load Graph from File ----------------
    void loadGraph(const string& filename) {
        ifstream in(filename);
        if (!in) { cout << "Error: file \"" << filename << "\" not found.\n"; return; }

        cities.clear();
        adj.clear();
        cityIndex.clear();

        int n;
        in >> n;
        for (int i = 0; i < n; i++) {
            string name; double x, y;
            in >> name >> x >> y;
            addCitySilent(name, x, y);
        }

        int e;
        in >> e;
        for (int i = 0; i < e; i++) {
            int u, v; double w;
            in >> u >> v >> w;
            addRoadSilent(u, v, w);
        }

        in.close();
        cout << "Graph loaded from \"" << filename << "\" successfully. ("
             << n << " cities, " << e << " roads)\n";
    }
};

// ------------------------------------------------------------
//  HELPER : read a full line safely (skips leftover newline)
// ------------------------------------------------------------
string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// ------------------------------------------------------------
//  MAIN MENU
// ------------------------------------------------------------
void printMenu() {
    cout << "\n============ SMART NAVIGATION SYSTEM ============\n";
    cout << " 1. Add City\n";
    cout << " 2. Add Road\n";
    cout << " 3. Display Map\n";
    cout << " 4. Find Shortest Route (Dijkstra)\n";
    cout << " 5. BFS Traversal\n";
    cout << " 6. DFS Traversal\n";
    cout << " 7. A* Search\n";
    cout << " 8. Toggle Traffic Mode\n";
    cout << " 9. Save Graph to File\n";
    cout << "10. Load Graph from File\n";
    cout << " 0. Exit\n";
    cout << "===================================================\n";
    cout << "Enter your choice: ";
}

int main() {
    NavigationGraph graph;
    int choice;

    cout << "Welcome to Smart Navigation System (Google Maps Mini)\n";
    cout << "Tip: use single-word city names (e.g. New_York, Los_Angeles)\n";

    do {
        printMenu();
        cin >> choice;

        if (cin.fail()) {           // non-numeric input safety
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input, please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush rest of line

        switch (choice) {
            case 1: {
                string name; double x, y;
                cout << "Enter city name: "; cin >> name;
                cout << "Enter X coordinate (for map/A*, e.g. 0): "; cin >> x;
                cout << "Enter Y coordinate (for map/A*, e.g. 0): "; cin >> y;
                graph.addCity(name, x, y);
                break;
            }
            case 2: {
                string from, to; double w; char dirChoice;
                cout << "From city: "; cin >> from;
                cout << "To city: "; cin >> to;
                cout << "Road weight/distance: "; cin >> w;
                cout << "Bidirectional road? (y/n): "; cin >> dirChoice;
                graph.addRoad(from, to, w, (dirChoice == 'y' || dirChoice == 'Y'));
                break;
            }
            case 3:
                graph.displayMap();
                break;
            case 4: {
                string from, to;
                cout << "From city: "; cin >> from;
                cout << "To city: "; cin >> to;
                graph.dijkstra(from, to);
                break;
            }
            case 5: {
                string start;
                cout << "Start city: "; cin >> start;
                graph.bfs(start);
                break;
            }
            case 6: {
                string start;
                cout << "Start city: "; cin >> start;
                graph.dfs(start);
                break;
            }
            case 7: {
                string from, to;
                cout << "From city: "; cin >> from;
                cout << "To city: "; cin >> to;
                graph.astar(from, to);
                break;
            }
            case 8:
                graph.toggleTraffic();
                break;
            case 9: {
                string filename;
                cout << "Enter filename to save (e.g. graph_data.txt): ";
                cin >> filename;
                graph.saveGraph(filename);
                break;
            }
            case 10: {
                string filename;
                cout << "Enter filename to load (e.g. graph_data.txt): ";
                cin >> filename;
                graph.loadGraph(filename);
                break;
            }
            case 0:
                cout << "Exiting... Thanks for using Smart Navigation System!\n";
                break;
            default:
                cout << "Invalid choice, try again.\n";
        }
    } while (choice != 0);

    return 0;
}
