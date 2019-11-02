#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsmn.h"
#include "lfa.h"

#define MEMSIZE 1024
#define LFASIZE 4096
#define CHAR_INIT (char)1
#define CHAR_EMPTY (char)0

#define retrieveJSON(v, p, s) v[p].end - v[p].start, s + v[p].start
#define retrieveJSON_char(v, p, s) 1, s + v[p].start
#define returnJSON_char(v, p, s) s[v[p].start]

static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
static int getAttr(char *str, jsmntok_t *t, int sz, const char* attr);

void parseTuring(const char *filename, automata_t *aut);

int main() {
    char mem[MEMSIZE];

    automata_t turingMachine;
    
    parseTuring("turing.lfa", &turingMachine);

    char input[MEMSIZE];
    printf("Digite a palavra a ser avaliada\n> ");
    scanf("%[^\n]*", input);
    printf("Avaliando palavra [%s]\n", input);

    lfa_eval_word(&turingMachine, input);
    return 0;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static int getAttr(char *str, jsmntok_t *t, int sz, const char* attr) {
    int i;
    for (i = 1; i < sz; i++) {
        if (jsoneq(str, &t[i], attr) == 0) {
            return i + 1;
        }
    }
}

void parseTuring(const char *filename, automata_t *aut) {
    char *lfa_temp = (char*)malloc(LFASIZE * sizeof(char));
    jsmntok_t *lfa_jsmn = (jsmntok_t*)malloc(LFASIZE * sizeof(jsmntok_t));
    jsmn_parser p;
    int j;

    FILE *fd = fopen(filename, "r");
    if (!fd) return;

    int lfa_temp_size = fread(lfa_temp, sizeof(char), LFASIZE, fd);

    jsmn_init(&p);
    int parse_result = jsmn_parse(&p, lfa_temp, lfa_temp_size, lfa_jsmn, LFASIZE);
    
    if (parse_result < 0) {
        printf("Failed to parse JSON: %d\n", parse_result);
        return;
    }

    // Initializing automata
    lfa_init_automata(aut);
    
    // Reading the set of all states
    state_t newState;
    lfa_init_state(&newState, "", STATE_NOTFINAL);
    int qpos = getAttr(lfa_temp, lfa_jsmn, LFASIZE, "Q");
    printf("Reading %d states\n", lfa_jsmn[qpos].size);
    for (j = 0; j < lfa_jsmn[qpos].size; j++) {
        printf(" -> State: %.*s\n", retrieveJSON(lfa_jsmn, qpos + j + 1, lfa_temp));
        sprintf(newState.name, "%.*s", retrieveJSON(lfa_jsmn, qpos + j + 1, lfa_temp));
        //jsmntok_t *g = &lfa_jsmn[qpos + j + 1];
        //sprintf(newState.name, "%.*s", g->end - g->start, lfa_temp + g->start);
        lfa_add_state_automata(aut, newState);
    }

    // Reading the initial state
    qpos = getAttr(lfa_temp, lfa_jsmn, LFASIZE, "q0");
    char temp[10];
    sprintf(temp, "%.*s", retrieveJSON(lfa_jsmn, qpos, lfa_temp));
    lfa_set_initial_state_name(aut, temp);
    printf("Setting initial state to %s\n", temp);

    // Reading the transition functions
    qpos = getAttr(lfa_temp, lfa_jsmn, LFASIZE, "d");
    char r[1], w[1], m[1];
    char src[10], dst[10];
    printf("Reading %d transition functions\n", lfa_jsmn[qpos].size);
    for (j = 0; j < lfa_jsmn[qpos].size; j++) {
        printf(" -> d: %.*s --%.*s,%.*s,%.*s--> %.*s\n", 
            retrieveJSON(lfa_jsmn, qpos+8*j+3, lfa_temp), // Source
            retrieveJSON(lfa_jsmn, qpos+8*j+6, lfa_temp), // Destination
            retrieveJSON(lfa_jsmn, qpos+8*j+7, lfa_temp), // Read
            retrieveJSON(lfa_jsmn, qpos+8*j+8, lfa_temp), // Write
            retrieveJSON(lfa_jsmn, qpos+8*j+4, lfa_temp)  // Move
        );
        sprintf(src, "%.*s", retrieveJSON(lfa_jsmn, qpos+8*j+3, lfa_temp));
        sprintf(dst, "%.*s", retrieveJSON(lfa_jsmn, qpos+8*j+4, lfa_temp));
        r[0] = returnJSON_char(lfa_jsmn, qpos+8*j+6, lfa_temp);
        w[0] = returnJSON_char(lfa_jsmn, qpos+8*j+7, lfa_temp);
        m[0] = returnJSON_char(lfa_jsmn, qpos+8*j+8, lfa_temp);
        lfa_create_transition_params(aut, src, dst, r, w, m);
    }

    // Reading final states
    qpos = getAttr(lfa_temp, lfa_jsmn, LFASIZE, "F");
    char stateName[10];
    printf("Reading %d final states\n", lfa_jsmn[qpos].size);
    for (j = 0; j < lfa_jsmn[qpos].size; j++) {
        printf(" -> State: %.*s\n", retrieveJSON(lfa_jsmn, qpos + j + 1, lfa_temp));
        sprintf(stateName, "%.*s", retrieveJSON(lfa_jsmn, qpos + j + 1, lfa_temp));
        lfa_set_state_final_name(aut, stateName);
    }
    printf("=================================\n");
}