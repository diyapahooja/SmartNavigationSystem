# 🗺️ Smart Navigation System (Google Maps Mini)

A lightweight, console-based C++ navigation system that simulates core routing algorithms used in GPS applications. This project implements graph theory concepts to find the shortest path between cities.

## ✨ Features

- **Add Cities & Roads** – Build your own map dynamically.
- **Dijkstra's Algorithm** – Find the shortest path (weighted) between two locations.
- **BFS (Breadth-First Search)** – Traverse the graph level by level.
- **DFS (Depth-First Search)** – Traverse the graph depth by depth.
- **A* Search Algorithm** – Uses a heuristic (straight-line distance) for faster pathfinding.
- **Traffic Mode** – Toggle traffic delays (multiplies road weights by 1.5x).
- **File Handling** – Save your map to a `.txt` file and load it later.

## 🛠️ Technologies Used

- **Language:** C++ (C++11 Standard)
- **Data Structures:** Adjacency List (`vector<vector<Edge>>`), Min-Heap (`priority_queue`)
- **Algorithms:** Dijkstra, BFS, DFS, A*, Heuristic Evaluation

## 🚀 How to Run

### Option 1: Dev-C++ / Code::Blocks
1. Open the IDE.
2. Create a new Source File.
3. Paste the code from `navigation_system.cpp`.
4. Save as `navigation_system.cpp`.
5. Compile & Run (F11).

### Option 2: Command Line (g++)
g++ -std=c++11 navigation_system.cpp -o navigation_system
./navigation_system

##📋 Menu Options
Option	Description
1	Add City (with X/Y coordinates for heuristic)
2	Add Road (Bidirectional/Unidirectional with weight)
3	Display Map
4	Shortest Route (Dijkstra)
5	BFS Traversal
6	DFS Traversal
7	A* Search
8	Toggle Traffic Mode (ON/OFF)
9	Save Graph to File
10	Load Graph from File
0	Exit

##🤝 Contributing
Feel free to fork this repository and add new features like GUI integration, real-time traffic updates, or support for more complex map formats!

