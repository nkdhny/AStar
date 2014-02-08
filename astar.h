#ifndef ASTAR_H
#define ASTAR_H
#include <algorithm>
#include <functional>
#include <boost/utility.hpp>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <iostream>
#include <assert.h>

typedef int Cost;

template <typename Domain, typename ActionPtr>
class TracedDomain {
private:
    Domain d;
    std::vector<ActionPtr> aa;

public:
    explicit TracedDomain(const Domain& _d):
        d(_d),
        aa()
    {
    }

    TracedDomain(const TracedDomain<Domain, ActionPtr>& other,const ActionPtr a):
        d(other.d),
        aa(other.aa)
    {
        accept(a);
    }

    void accept(const ActionPtr a) {
        assert((*a).isDefined(d));
        d = (*a)(d);
        aa.push_back(a);
    }

    const std::vector<ActionPtr>& actions() const {
        return aa;
    }

    const Domain& domain() const {
        return d;
    }
};

template <typename DomainCostFunction, typename Domain, typename ActionPtr, typename Cost>
struct ToTracedDomainCostFunctionAdapter: std::unary_function<const TracedDomain<Domain, ActionPtr>&, Cost> {

    DomainCostFunction f; //std::unaryfunction<const Domain&, Cost>

    ToTracedDomainCostFunctionAdapter(const DomainCostFunction& cost):
        f(cost)
    {
    }

    Cost operator()(const TracedDomain<Domain, ActionPtr>& d) const {
        f(d.domain());
    }
};

template< typename Domain, typename ActionPtr >
struct StepCountCost {

    StepCountCost()
    {
    }

    Cost operator ()(const TracedDomain<Domain, ActionPtr>& d) const {
        return d.actions().size();
    }

};


template<typename Domain>
struct TreeVisitor {
    void operator()(const Domain& d, std::unordered_set<Domain>& closed_set) {
    }
};

template<typename Domain>
struct GraphVisitor {
    void operator()(const Domain& d, std::unordered_set<Domain>& closed_set) {
        closed_set.insert(d);
    }
};

template<typename Domain>
struct FinalStateGoal: std::unary_function<const Domain&, bool> {
    const Domain final;

    FinalStateGoal(const Domain& _final):
        final(_final)
    {}

    bool operator()(const Domain& d) {
        return d == final;
    }
};

template<typename F, typename G, typename A, typename R>
struct Sum {
    F* f;
    G* g;

    Sum(F* _f, G* _g):
        f(_f),
        g(_g)
    {}

    R operator()(const A& a) const{
        return (*f)(a)+(*g)(a);
    }
};


template<
        typename Domain,
        typename ActionPtr,
        typename Visitor,
        typename ActionIterator,
        typename GoalTest,
        typename CostFunction,
        typename StepCostFunction = StepCountCost< Domain, ActionPtr >
        >
class GenericAStar: public boost::noncopyable
{
public:
    typedef TracedDomain<Domain, ActionPtr> DomainWithHistory;

private:
    typedef ToTracedDomainCostFunctionAdapter<CostFunction, Domain, ActionPtr, Cost> TracedCostFuntction;
    typedef Sum<TracedCostFuntction, StepCostFunction, DomainWithHistory, Cost> TotalCostFunction;


private:
    template <typename _ActionPtr, typename _ActionPtrIterator>
    class ActionUniverse {

        ActionUniverse();

        template<typename A, typename D>
        struct PushIfApplicable {
            std::queue<A>* aa;
            const D* d;

            PushIfApplicable(const D* _d, std::queue<A>* _aa):
                aa(_aa),
                d(_d)
            {}

            void operator()(const A& a) const {
                if(a->isDefined(*d)) {
                    aa->push(a);
                }
            }
        };

    public:
        const std::vector<_ActionPtr> universe;

        explicit ActionUniverse(_ActionPtrIterator _begin, _ActionPtrIterator _end) :
            universe(_begin, _end)
        {}

        void applicable(const Domain& field, std::queue<_ActionPtr>& actionsCanBeApplyied) const {

            PushIfApplicable<_ActionPtr, Domain> pushIfApplicable(&field, &actionsCanBeApplyied);

            std::for_each(
                        universe.begin(),
                        universe.end(),
                        pushIfApplicable);
        }

    };

    template<typename _CostFunction>
    struct CostCompare: std::binary_function<const DomainWithHistory&, const DomainWithHistory&, bool> {
        //a<b
        const _CostFunction* totalCost;

        CostCompare(_CostFunction* _totalCost):
            totalCost(_totalCost)
        {}

        bool operator()(const DomainWithHistory& a, const DomainWithHistory& b) {
            return (*totalCost)(a) > (*totalCost)(b);
        }
    };
    typedef CostCompare<TotalCostFunction> DomainCostCompare;

private:

    GenericAStar();

    TracedCostFuntction tracedHeuristic;
    StepCostFunction cost;
    TotalCostFunction totalCost;


    std::priority_queue<
        DomainWithHistory,
        std::vector<DomainWithHistory>,
        DomainCostCompare
    > open_set;

    std::unordered_set<Domain> closed_set;
    ActionUniverse<ActionPtr, ActionIterator> universe;
    GoalTest goal;

    void expand(const DomainWithHistory& from, std::queue<  DomainWithHistory >& to) const {
        std::queue<ActionPtr> actionCanBeApplyied;
        universe.applicable(from.domain(), actionCanBeApplyied);

        while(!actionCanBeApplyied.empty()) {
            const ActionPtr a = actionCanBeApplyied.front();
            actionCanBeApplyied.pop();
            to.push(DomainWithHistory(from , a));
        }
    }
    DomainWithHistory current() {
        DomainWithHistory c = open_set.top();
        open_set.pop();

        return c;
    }


public:

    GenericAStar(
            const Domain& initial,
            const ActionIterator& actions_begin,
            const ActionIterator& actions_end,
            const GoalTest& _goal,
            const CostFunction& _heuristic,
            const StepCostFunction& _cost = StepCountCost< Domain, ActionPtr >()
            ):
        tracedHeuristic(TracedCostFuntction(_heuristic)),
        cost(_cost),
        totalCost(&tracedHeuristic, &cost),
        open_set(DomainCostCompare(&totalCost)),
        closed_set(),
        universe(actions_begin, actions_end),
        goal(_goal)
    {
        open_set.push(DomainWithHistory(initial));
    }



    GenericAStar(
            const Domain& initial,
            const Domain& _goal,
            const ActionIterator& actions_begin,
            const ActionIterator& actions_end,
            const CostFunction& _heuristic,
            const StepCostFunction& _cost = StepCountCost<Domain, ActionPtr>()
            ):
        tracedHeuristic(_heuristic),
        cost(_cost),
        totalCost(&tracedHeuristic, &cost),
        open_set(DomainCostCompare(&totalCost)),
        closed_set(),
        universe(actions_begin, actions_end),
        goal(FinalStateGoal<Domain>(_goal))
    {
        open_set.push(TracedDomain<Domain, ActionPtr>(initial));
    }


    bool plan(DomainWithHistory& domainWithActionsApplyied);
};

template<
        typename Domain,
        typename ActionPtr,
        typename Visitor,
        typename ActionItertor,
        typename Goal,
        typename CostFunction,
        typename StepCostFunction
        >
bool GenericAStar<Domain,
                  ActionPtr,
                  Visitor,
                  ActionItertor,
                  Goal,
                  CostFunction,
                  StepCostFunction
                 >::plan(DomainWithHistory &domainWithActionsApplyied) {

    DomainWithHistory cur = current();
    Visitor visitor;

    while (! goal(cur.domain())) {

        visitor(cur.domain(), closed_set);

        std::queue<DomainWithHistory> reachable;

        expand(cur, reachable);

        while (!reachable.empty()) {

            DomainWithHistory& r  = reachable.front();
            if(closed_set.find(r.domain())==closed_set.end()){
                open_set.push(reachable.front());
            }
            reachable.pop();
        }

        if(open_set.empty())
            return false;

        cur = current();
    }

    domainWithActionsApplyied =  cur;
    return true;
}

template <typename Domain, typename ActionPtr, typename ActionsIterator, typename CostFunction>
bool tree_plan(
        const Domain& initial,
        const Domain& final,
        const CostFunction heuristic,
        const ActionsIterator actionsBegin,
        const ActionsIterator actionsEnd,
        TracedDomain<Domain, ActionPtr>& history
        ) {

    GenericAStar<
            Domain,
            ActionPtr,
            TreeVisitor<Domain>,
            ActionsIterator,
            FinalStateGoal<Domain>,
            CostFunction
            >
            planner(initial, final, actionsBegin, actionsEnd, heuristic);

    return planner.plan(history);
}

template <typename Domain, typename ActionPtr, typename ActionsIterator, typename CostFunction>
bool graph_plan(
        const Domain& initial,
        const Domain& final,
        const CostFunction heuristic,
        const ActionsIterator actionsBegin,
        const ActionsIterator actionsEnd,
        TracedDomain<Domain, ActionPtr>& history
        ) {

    GenericAStar<
            Domain,
            ActionPtr,
            GraphVisitor<Domain>,
            ActionsIterator,
            FinalStateGoal<Domain>,
            CostFunction
            >
            planner(initial, final, actionsBegin, actionsEnd, heuristic);

    return planner.plan(history);
}

#endif // ASTAR_H
