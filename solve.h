#ifndef __SOLVE_H__
#define __SOLVE_H__

struct problem;
struct solution;

void solve_init(solution_t* s, problem_t* p, char *initial_output_fname);
void solve(struct solution*, struct problem*);

#endif /* __SOLVE_H__ */
