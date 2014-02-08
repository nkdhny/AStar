AStar
=====

Astar planning implementation

Actual planning algorythm is abstract and works with some domain and actions defined on it domain. Algorythm presumes that:
* `Domain` is copyable (to be stored in stl containers)
* `Action` is a function object from `Domain=>Cost` with `operator()(const Domain&)` being virtual
* `Action` defines virtual method `bool isDefined(const Domain&)`
* To trace actions applyied to the state one could use `TracedDomain<Domain, ActionPtr>`, being essentially a pair of all actions applyied to the state and final state
* `CostFunction` is std function object `Domain=>Cost`
* Cost is an integer by default but could be changed through typedef, one should provide `operator +` and `operator <` for newly defined `Cost
* By default cost of already made actions (`g` in terms of AStar) is just number of actions applyied, but one could specify ones own function by specifying CostStepFunction
* Cost Step function is std function object `TracedDomain<Domain, ActionPtr>=>Cost`, thus it have access to all action already applyied to the state

You use `tree_plan(....)` to build solution of your problem using Tree AStar and `graph_plan` to build plan throug graph

Boost and GTest are required
To build use cmake
