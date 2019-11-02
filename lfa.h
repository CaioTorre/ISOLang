#ifndef LFA_H
#define LFA_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STATE_NAME_MAX 10
#define MAX_TAPE 1024
#define MAX_STATES 1024
#define MAX_ALPHABET 50

#define TAPE_PRINT_SIZE 20

#define TYPEDEF_STRUCT(st) typedef struct st_t st
#define getTapePtr(x) x.elements[x.ptr]
#define setTapePtr(x, v) x.elements[x.ptr] = v
#define printLine(s) int v = s; printf("+"); while(v--) printf("---+"); printf("\n");

typedef enum {
  STATE_NOTFINAL = 0,
  STATE_FINAL = 1
} stateFinal_t;

typedef enum {
  MOVE_LEFT = -1,
  MOVE_RIGHT = 1
} tapeMovement_t;

typedef enum {
  SYMBL_BLANK = '_',
  SYMBL_START = '$'
} specialChars_t;

typedef enum {
  WORD_OK = 1,
  WORD_FAIL = 0
} wordEvalResult_t;

// typedef struct alphabet_char_t {
//     char c;
// } alphabet_char_t;
typedef char alphabet_char_t;

typedef struct state_t {
    char name[STATE_NAME_MAX];
    stateFinal_t isFinal;
    struct state_transition_ll_t *delta;
} state_t;
//TYPEDEF_STRUCT(state);

typedef struct state_transition_ll_t {
    alphabet_char_t r;
    alphabet_char_t w;
    struct state_t *s;
    tapeMovement_t m;
    struct state_transition_ll_t *next;
} state_transition_ll_t;
//TYPEDEF_STRUCT(state_transition_ll);

typedef struct tape_t {
    alphabet_char_t elements[MAX_TAPE];
    int ptr;
} tape_t;
//TYPEDEF_STRUCT(tape);

typedef struct automata_t {
    struct state_t states[MAX_STATES];
    int n_states;
    struct state_t *current;
    struct tape_t tape;
} automata_t ;
//TYPEDEF_STRUCT(automata);

// Helper functions
//static int strcmp(const char *a, const char* b);

extern void lfa_init_automata(automata_t *aut);
extern void lfa_init_state(state_t *st, const char *name, stateFinal_t isFinal);
extern void lfa_init_tape(tape_t *tape);

extern void lfa_add_state_automata(automata_t *aut, state_t st);
extern void lfa_set_initial_state(automata_t *aut, state_t *st);
extern void lfa_set_initial_state_name(automata_t *aut, const char *name);
extern void lfa_set_state_final(state_t *st);
extern void lfa_set_state_final_name(automata_t *aut, const char *name);

extern void lfa_add_transition(state_t *st, state_transition_ll_t *trans);
extern void lfa_create_transition_params(automata_t *aut, const char *origin, const char *destination, const char *read, const char *write, const char *move);
extern void lfa_create_transition(automata_t *aut, state_t *src, state_t *dst, alphabet_char_t read, alphabet_char_t write, tapeMovement_t move);

extern void lfa_fill_tape(tape_t *tape, const char *contents);

extern stateFinal_t lfa_eval_word(automata_t *aut, const char *word);
extern stateFinal_t lfa_eval(automata_t *aut);

static state_transition_ll_t *findTransition(state_t *src, alphabet_char_t r);

static state_t *get_state(state_t *list, int n, const char *name);
static state_t *get_state_from_automata(automata_t *aut, const char *name);

static void printTape(tape_t *tape, int ptr, int n_chars);
static void printTransitions(state_t *st);
static void printStates(automata_t *aut);

//Function definitions

extern void lfa_init_automata(automata_t *aut) {
    aut->current = &(aut->states[0]);
    aut->n_states = 0;
    lfa_init_tape(&aut->tape);
}
extern void lfa_init_state(state_t *st, const char *name, stateFinal_t isFinal) {
    int i = 0;
    while (name[i++]) st->name[i] = name[i];
    st->name[i] = 0;
    st->isFinal = isFinal;
    st->delta = NULL;
}
extern void lfa_init_tape(tape_t *tape) {
    memset(tape->elements, SYMBL_BLANK, MAX_TAPE);
    tape->elements[0] = SYMBL_START;
    tape->ptr = 0;
}

extern void lfa_add_state_automata(automata_t *aut, state_t st) {
    aut->states[aut->n_states] = st;
    aut->n_states++;
}
extern void lfa_set_initial_state(automata_t *aut, state_t *st) {
    aut->current = st;
}
extern void lfa_set_initial_state_name(automata_t *aut, const char *name) {
    lfa_set_initial_state(aut, get_state_from_automata(aut, name));
}

extern void lfa_set_state_final(state_t *st) {
    st->isFinal = STATE_FINAL;
}
extern void lfa_set_state_final_name(automata_t *aut, const char *name) {
    lfa_set_state_final(get_state_from_automata(aut, name));
}

extern void lfa_add_transition(state_t *st, state_transition_ll_t *trans) {
    if (st->delta == NULL) {
        st->delta = trans;
    } else {
        state_transition_ll_t *temp = st->delta;
        while (temp->next != NULL) temp = temp->next;
        temp->next = trans;
    }
}

extern void lfa_create_transition_params(automata_t *aut, const char *origin, const char *destination, const char *read, const char *write, const char *move) {
    alphabet_char_t r = read[0];
    alphabet_char_t w = write[0];
    tapeMovement_t m;
    switch (move[0]) {
        case 'E':
        case 'L':
            m = MOVE_LEFT;
            break;
        case 'D':
        case 'R':
            m = MOVE_RIGHT;
            break;
        default:
            printf("Unknown movement %c\n", move[0]);
            return;
    }
    state_t *src = get_state_from_automata(aut, origin);
    state_t *dst = get_state_from_automata(aut, destination);
    lfa_create_transition(aut, src, dst, r, w, m);
}

extern void lfa_create_transition(automata_t *aut, state_t *src, state_t *dst, alphabet_char_t read, alphabet_char_t write, tapeMovement_t move) {
    state_transition_ll_t *newT = (state_transition_ll_t *)malloc(sizeof(state_transition_ll_t));
    if (!newT) return;
    //printf("Creating transition %s --%c,%c,%d--> %s\n", src->name, read, write, move, dst->name);
    newT->r = read;
    newT->w = write;
    newT->m = move;
    newT->s = dst;
    newT->next = NULL;
    lfa_add_transition(src, newT);
    //printf("\tDone\n");
}

extern void lfa_fill_tape(tape_t *tape, const char *contents) {
    int i = 0;
    while (contents[i] != '\0') { tape->elements[i + 1] = contents[i]; i++; }
}

extern stateFinal_t lfa_eval_word(automata_t *aut, const char *word) {
    lfa_fill_tape(&aut->tape, word);
    return lfa_eval(aut);
}


extern stateFinal_t lfa_eval(automata_t *aut) {
    int cont = 1;
    state_transition_ll_t *currentTrans;
    printStates(aut);
    aut->current = (aut->states);
    while (cont) {
        printTape(&(aut->tape), aut->tape.ptr, TAPE_PRINT_SIZE);
        //printTransitions(aut->current);
        if (aut->current->delta == NULL) { cont = 0; break; } //May be wrong
        currentTrans = findTransition(aut->current, getTapePtr(aut->tape));
        if (currentTrans == NULL) { cont=0; break; } //printf("No transitions for the current state (%s) given current symbol (%c)", aut->current->name, getTapePtr(aut->tape));
        setTapePtr(aut->tape, currentTrans->w);
        switch (currentTrans->m) {
            case MOVE_RIGHT:
                aut->tape.ptr++;
                break;
            case MOVE_LEFT:
                aut->tape.ptr--;
                break;
            default:
                return STATE_NOTFINAL;
        }
        aut->current = currentTrans->s;
    }
    printLine(TAPE_PRINT_SIZE)
    if (aut->current->isFinal == STATE_FINAL) {
        printf("Word accepted!\n");
    } else {
        printf("Word rejected!\n");
    }
    return aut->current->isFinal;
}

static state_transition_ll_t *findTransition(state_t *src, alphabet_char_t r) {
    state_transition_ll_t *curr = src->delta;
    while (curr != NULL) {
        if (curr->r == r) return curr;
        curr = curr->next;
    }
    return NULL;
}

static state_t *get_state(state_t *list, int n, const char *name) {
    while (--n >= 0) {
        if (!strcmp(list[n].name, name))
            return &list[n];
    }
    return NULL;
}
static state_t *get_state_from_automata(automata_t *aut, const char *name) {
    return get_state(aut->states, aut->n_states, name);
}

static void printTape(tape_t *tape, int ptr, int n_chars) {
    int pos = 0;
    printLine(n_chars)
    pos = 0;
    while (pos < n_chars) {
        printf("|%c%c%c", pos == ptr ? '<' : ' ', tape->elements[pos], pos == ptr ? '>' : ' ');
        pos++;
    }
    printf("|...\n");
}

static void printTransitions(state_t *st) {
    printf("State: %s\n", st->name);
    state_transition_ll_t *curr = st->delta;
    while (curr != NULL) {
        printf(" --%c,%c,%d--> %s\n",curr->r, curr->w, curr->m, curr->s->name);
        curr = curr->next;
    }
}

static void printStates(automata_t *aut) {
    int i = 0;
    while (i < aut->n_states) {
        printf("State = %s (isFinal = %d)\n", aut->states[i].name, aut->states[i].isFinal);
        i++;
    }
}

#endif //LFA_H