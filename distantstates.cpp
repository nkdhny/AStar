#include "model.h"
#include "actions.h"
#include <unordered_set>

struct AllActionsAtThisPosition {
    std::vector<FifteenAction*>* actions;

    void operator()(const Place& p){
        MoveDown* d = new MoveDown(p.position);
        MoveUp* u = new MoveUp(p.position);
        MoveLeft* l = new MoveLeft(p.position);
        MoveRight* r = new MoveRight(p.position);

        actions->push_back(l);
        actions->push_back(r);
        actions->push_back(u);
        actions->push_back(d);
    }
};

void allPossibleActions(const Field& f, std::vector<FifteenAction*>& actions) {
    AllActionsAtThisPosition actionsFunctor;
    actionsFunctor.actions = &actions;

    std::for_each(f.begin(), f.end(), actionsFunctor);
}

void applyAllActions(const Field& f, const std::vector<FifteenAction*>& a, std::unordered_set<Field>& r) {
    std::for_each(
                a.begin(),
                a.end(),
                [&](FifteenAction* a) {
                    if((*a).isDefined(f)){
                        r.insert((*a)(f));
                    }
                }
                );
}

void allStatesReachableFromOpenSetWithinOneStep(const std::unordered_set<Field>& open_set, const std::vector<FifteenAction*>& actions, std::unordered_set<Field>& reachable) {

    std::for_each(
                open_set.begin(),
                open_set.end(),
                [&](const Field& f) {
                    applyAllActions(f, actions, reachable);
                }
                );
}

void allStatesReachableFromCurrentWithinNSteps(const std::unordered_set<Field>& from,
                                               unsigned numberOfStepsToBeDone,
                                               const std::vector<FifteenAction*> allActionsCanBeApplyied,
                                               std::unordered_set<Field>& targetStates) {

    assert(numberOfStepsToBeDone!=0);
    assert(allActionsCanBeApplyied.size()>0);

    if(numberOfStepsToBeDone==1) {
        allStatesReachableFromOpenSetWithinOneStep(from, allActionsCanBeApplyied, targetStates);
    } else {
        std::unordered_set<Field> next;
        allStatesReachableFromOpenSetWithinOneStep(from, allActionsCanBeApplyied, next);
        allStatesReachableFromCurrentWithinNSteps(next, numberOfStepsToBeDone-1, allActionsCanBeApplyied, targetStates);
    }
}

int main() {
    Field f(3);

    std::cout<<"Starting from: "<<std::endl<<f;

    std::unordered_set<Field> r;


    unsigned N=27;
    std::unordered_set<Field> ff;
    ff.insert(f);

    std::vector<FifteenAction*> a;
    allPossibleActions(f, a);

    allStatesReachableFromCurrentWithinNSteps(ff, N, a, r);

    std::cout<<"have "<<r.size()<<" states can go to through "<<N<<" steps"<<std::endl;


    if(r.size() < 20) {
        std::for_each(
                    r.begin(),
                    r.end(),
                    [](const Field& f) {
                        std::cout<<(f)<<std::endl;
                    }
                    );
    }

}
