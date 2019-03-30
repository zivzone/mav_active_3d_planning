#ifndef MAV_ACTIVE_3D_PLANNING_TRAJECTORY_SEGMENT_H_
#define MAV_ACTIVE_3D_PLANNING_TRAJECTORY_SEGMENT_H_

#include <mav_msgs/eigen_mav_msgs.h>

#include <vector>
#include <memory>

namespace mav_active_3d_planning {

    // Base struct that contains trajectory evaluator information associated with the segment.
    struct TrajectoryInfo {
        virtual ~TrajectoryInfo() {}
    };

    // Struct to store trajectory tree data
    struct TrajectorySegment {
        TrajectorySegment() : parent(nullptr), tg_visited(false), gain(0.0), cost(0.0), value(0.0) {};

        // All trajectory points
        mav_msgs::EigenTrajectoryPointVector trajectory;

        // Associated costs
        double gain;
        double cost;
        double value;

        // trajectory generator flag
        bool tg_visited;

        // pointer to parent trajectory, nullptr for currently active segment (root)
        TrajectorySegment* parent;

        // Pointers to successive trajectory nodes, all nodes are owned by the parent
        std::vector<std::unique_ptr<TrajectorySegment>> children;

        // Information to be carried with this segment, e.g. virtual voxels
        std::unique_ptr<TrajectoryInfo> info;

        // compare function for sorting etc
        static bool compare(TrajectorySegment a, TrajectorySegment b) {
            return (a.value < b.value);
        }
        static bool comparePtr(TrajectorySegment* a, TrajectorySegment* b) {
            return (a->value < b->value);
        }

        // Safely create a child node and return a pointer to it
        TrajectorySegment* spawnChild(){
            children.push_back(std::unique_ptr<TrajectorySegment>(new TrajectorySegment()));
            children.back()->parent = this;
            return children.back().get();
        }

        // The following utility functions assume a tree structure (no loops)
        // Add pointers to all immediate children to the result vector
        void getChildren(std::vector<TrajectorySegment*> *result) {
            for (int i = 0; i < children.size(); ++i) { result->push_back(children[i].get()); }
        }

        // Recursively add pointers to all leaf nodes (have no children) to the result vector
        void getLeaves(std::vector<TrajectorySegment*> *result) {
            if (children.empty()) { result->push_back(this); return;}
            for (int i = 0; i < children.size(); ++i) { children[i]->getLeaves(result); }
        }

        // Recursively add pointers to all subsequent nodes to the result vector
        void getTree(std::vector<TrajectorySegment*> *result) {
            result->push_back(this);
            for (int i = 0; i < children.size(); ++i) { children[i]->getTree(result); }
        }
    };

}  // namespace mav_active_3d_planning
#endif  // MAV_ACTIVE_3D_PLANNING_TRAJECTORY_SEGMENT_H_