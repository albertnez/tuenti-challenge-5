"""
This is a LinearProgramming problem. We can model our variables as:
Xij, where i is the person i, and j is the tree j. That is, the
number of minutes that person i dedicates to tree j.
Then we have to minimize the sum of all Xij, that is, the sum of time taken.

Our constraints are:

For each person i: Xi0 .. Xin <= Mi. That is, he must spend at most Mi time
in total.

For each tree t: (1/C0t)*X0t ... (1/Cnt)Xnt >= 1. Where C_i_j is the time
that takes for person i to cut tree j. With this we can force people to cut the
tree. 

Obviously we should make every variable non-negative to be coherent.
"""

from pulp import *


def main():
    num_cases = int(sys.stdin.readline().strip())
    for case in range(0, num_cases):
        num_trees, time_predicted, num_people = tuple(
                map(int, sys.stdin.readline().split()))

        # Create the problem
        problem = LpProblem("problem", LpMinimize)

        # Matrix with times
        person_times = []

        tree_times = []
        tree_variables = []

        for person in range(0, num_people):
            times = list(map(int, sys.stdin.readline().split()))
            person_times.append(times[0]);
            tree_times.append(times[1:])

            # Add all variables regarding tree times of that person
            tree_variables.append([LpVariable('x{0:02d}{1:02d}'.format(person, i), 0) for i in range(0, num_trees)])
            # Add constraint to not work more than given ours
            problem += lpSum([var for var in tree_variables[person]]) <= person_times[person]

        # Add restriction for each tree
        for tree in range(0, num_trees):
            # Each Tree must be cut
            problem += lpSum([(1.0/tree_times[p][tree]) * tree_variables[p][tree] for p in range(0, num_people)]) >= 1

        # Create the objective
        problem += lpSum([var for l in tree_variables for var in l])
        GLPK(msg = 0).solve(problem)

        # Create the result
        solution = value(problem.objective)
        print('Test case #{0}: '.format(case+1), end='')
        if (solution == 0.0):
            print('IMPOSSIBLE')
        else:
            if (solution <= time_predicted):
                print('RIGHT')
            else:
                print('{0:.2f}'.format(solution-time_predicted))


if __name__ == "__main__":
    main()
