#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Converts inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

	start_node = &m_Model.FindClosestNode(start_x, start_y);
  	end_node = &m_Model.FindClosestNode(end_x, end_y);
}

// Calculates distance between the node parameter and the end node using RouteModel::distance(...) method.
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
	return node->distance(*end_node);
}



// This method uses the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  	current_node->FindNeighbors();
	for (RouteModel::Node * i : current_node->neighbors) {
      i->parent = current_node;
      i->h_value = CalculateHValue(i);
      i->g_value = current_node->g_value + i->distance(*current_node);
      open_list.emplace_back(i);
      i->visited = true;
    }
}




// This method helps sort the open_list according to the sum of the h value and g value.
bool Compare(RouteModel::Node * a, RouteModel::Node * b) {
  return (a->h_value + a->g_value) > (b->h_value + b->g_value);
}

RouteModel::Node *RoutePlanner::NextNode() {
  std::sort(open_list.begin(), open_list.end(), Compare);
  RouteModel::Node * next_node = open_list.back();
  open_list.pop_back();
  return next_node;
}



// This method takes the current (final) node as an argument and iteratively follows the 
//   chain of parents of nodes until the starting node is found. With this information,
// a shortest final path is constructed between the two points.
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    // TODO: Implement your solution here.
  	do {
      distance += current_node->distance(*current_node->parent);
      path_found.emplace(path_found.begin(), *current_node);
      current_node = current_node->parent;  
    } while (current_node->parent != nullptr);
  	path_found.emplace(path_found.begin(), *start_node);
    distance *= m_Model.MetricScale();
    return path_found;

}


// This method begins the search of finding the shortest path between two points by calling necessary methods.
void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

  current_node = start_node;
  	AddNeighbors(current_node);
  start_node->visited = true;
  open_list.push_back(start_node);
  	while (open_list.size() > 0) {
      current_node = NextNode();
      if(current_node != end_node) {
        AddNeighbors(current_node);
      } 
      else {
        m_Model.path = ConstructFinalPath(current_node);
        break;
      }
    }
}