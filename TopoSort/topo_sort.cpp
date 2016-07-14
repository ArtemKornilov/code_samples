#include <iostream>
#include "stack.h"
#include "vector.h"
#include "digraph.h"


int main() {

	int n, m; 
	std::cin >> n >> m; 
 	

	TDigraph graph(n);

 	int a, b;
	for (int i = 1; i <= m; i++) {
		std::cin >> a >> b;
		graph.AddConnection(a, b);
	}

	if (graph.IsDigraph()) {
		for (int i = 1; i <= n; i++) {
			std::cout << graph.GetClearVertice() << ' ';
		}	
	}
	else {
		std::cout << -1;
	}
	std::cout << std::endl;

	return 0;
}