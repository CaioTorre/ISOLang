#include "lfa.h"
// Helper functions
// static int strcmp(const char *a, const char* b) {
//     int i = 0;
//     while (a[i] && b[i++]) 
//         if ((int)a[i] - (int)b[i]) return (int)a[i] - (int)b[i];
//     return (int)a[i] - (int)b[i];
// }

// void lfa_init_automata(automata_t *aut) {
//     aut -> current = &(aut -> states[0]);
//     aut -> n_states = 0;
// }
// void lfa_init_state(state_t *st, const char *name, stateFinal_t isFinal) {
//     int i = 0;
//     while (name[i++]) st->name[i] = name[i];
//     st->name[i] = 0;
//     st->isFinal = isFinal;
//     st->delta = NULL;
// }

// void lfa_add_state_automata(automata_t *aut, state_t st) {
//     aut->states[aut->n_states++] = st;
// }
// void lfa_set_initial_state(automata_t *aut, state_t *st) {
//     aut->current = st;
// }
// void lfa_set_initial_state_name(automata_t *aut, const char *name) {
//     lfa_set_initial_state(aut, get_state_from_automata(aut, name));
// }

// void lfa_add_transition(state_t *st, state_transition_ll_t *trans) {
//     state_transition_ll_t **root = &(st->delta);
//     if (*root == NULL) {
//         *root = trans;
//     } else {
//         while ((*root)->next != NULL) root = &((*root)->next);
//         *root = trans;
//     }
// }

// state_t *get_state(state_t *list, int n, const char *name) {
//     while (--n) {
//         if (!strcmp(list[n].name, name))
//             return &list[n];
//     }
//     return NULL;
// }
// state_t *get_state_from_automata(automata_t *aut, const char *name) {
//     return get_state(aut->states, aut->n_states, name);
// }
