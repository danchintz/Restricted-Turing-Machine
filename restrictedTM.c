#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	int from;
	int read;
	int to;
	int write;
	int dir;
} transition;


typedef struct node {
	struct node *next;
	transition trans;
} node;

typedef struct {
	int size;
	struct node **buckets;
} Map;

typedef struct {
	int type;
	Map transitions;
} State;

int map_size = 4+26+10;

State STATES[1001] = {0};
const int ACCEPT = 1;
const int REJECT = 2;

int start;

int *history;
int maxHistory;

int currentState = 0;
int currentInput = 0;

void
push(struct node ** head, transition t) {
	struct node *newnode = malloc(sizeof(struct node));
	newnode->trans=t;
	newnode->next=*head;
	*head=newnode;
}

struct node *
pop(struct node **head) {
	struct node *ret = *head;
	if(*head)
		*head = (*head)->next;
	return ret;
}

void
addtransition(transition t) {
	int bucket = t.read % map_size;
	push(STATES[t.from].transitions.buckets+bucket, t);
}

char *
strcopy(char *str) {
	size_t len = strlen(str);
	char *copied = malloc(len*sizeof(char));
	strcpy(copied, str);
	return copied;
}

char **
str_split(char* a_str, const char a_delim) {
	char** result    = 0;
	size_t count     = 0;
	char* tmp        = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	while (*tmp) {
		if (a_delim == *tmp){
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	count += last_comma < (a_str + strlen(a_str) - 1);
	count++;

	result = malloc(sizeof(char*) * count);

	if (result) {
		size_t idx  = 0;
		char* token = strtok(a_str, delim);

		while (token) {
			*(result + idx++) = strcopy(token);
			token = strtok(0, delim);
		}
		*(result + idx) = 0;
	}

	return result;
}

void
filltransitions(FILE *fp) {
	int n = 0;
	char line[2048];
	while(fgets(line, sizeof(line), fp)) {
		char** tokens = str_split(line, '\t');

		if(strncmp("state", tokens[0], strlen("state")) == 0) {
			int state = atoi(tokens[1]);
			if(strncmp("start", tokens[2], strlen("start")) == 0) start = state;
			else if(strncmp("accept", tokens[2], strlen("accept")) == 0) STATES[state].type = ACCEPT;
			else if(strncmp("reject", tokens[2], strlen("reject")) == 0) STATES[state].type = REJECT;
		} else {
			transition trans;
			trans.from  = atoi(tokens[1]);
			trans.read  = tokens[2][0];
			trans.to    = atoi(tokens[3]);
			trans.write = tokens[4][0];
			if('L' == tokens[5][0])
				trans.dir = -1;
			else
				trans.dir = 1;
			addtransition(trans);
			n++;
		}
		for(int i =0;*(tokens+i);i++) {
			free(*(tokens+i));
		}
		free(tokens);
	}
}

void
printhistory(const char *status) {
	fprintf(stdout, "%d", history[0]);
	int i;
	for(i = 1;i<maxHistory; ++i) {
		if(history[i]==-1)break;
		fprintf(stdout, "->%d", history[i]);
	}
	fprintf(stdout, " %s\n", status);
}

transition
gettransition(int inchar) {
	int bucket = inchar % map_size;
	struct node *current = STATES[currentState].transitions.buckets[bucket];
	while(current){
		transition trans = current->trans;
		if(trans.read == inchar) return trans;
		current = current->next;
	}
	transition trans = {0};
	return trans;
}

void
grow(char **tape, size_t *tapelength) {
	char *tmp = malloc(*tapelength * 2* sizeof(char));
	memset(tmp, '_', *tapelength*2*sizeof(char));
	memcpy(tmp, *tape, sizeof(char) **tapelength);
	free(*tape);
	*tape = tmp;
}

int
main(int argc, char** argv) {
	if(argc != 4) {
		fprintf(stderr, "Usage: %s <program> <input string> <max transitions>", argv[0]);
		exit(1);
	}

	//initialize
	for(int i=0;i<sizeof(STATES)/sizeof(STATES[0]);i++) {
		STATES[i].transitions.size = map_size;
		STATES[i].transitions.buckets = malloc(sizeof(struct node *) * map_size);
	}

	FILE *fp = fopen(argv[1], "r");
	filltransitions(fp);
	fclose(fp);

	size_t tapelength = strlen(argv[2]);
	char *input = strcopy(argv[2]);

	maxHistory = atoi(argv[3])+1;
	history = malloc((maxHistory+10) * sizeof(int));

	for (int i =0;i<maxHistory;i++) history[i] = -1;

	int n = 0;

	currentState = start;
	history[n++] = start;

	while(1) {
		if(n > maxHistory) {printhistory("quit");break;}

		if(currentInput > tapelength - 1) { //double the size of the tape
			grow(&input, &tapelength);
		}

		if(currentInput < 0) { // double the size of the tape and shift everything over
			grow(&input, &tapelength);
			memcpy(input+tapelength/2, input, sizeof(char) * tapelength/2);
			currentInput+=tapelength/2;
		}
		int inchar = input[currentInput];
		transition trans = gettransition(inchar);

		input[currentInput] = trans.write;
		currentState = trans.to;
		currentInput += trans.dir;
		history[n++] = currentState;

		if(STATES[currentState].type == REJECT) {printhistory("reject");break;}
		if(STATES[currentState].type == ACCEPT) {printhistory("accept");break;}
	}

	//Cleanup
	for(int i=0;i<sizeof(STATES)/sizeof(STATES[0]);i++) {
		for(int j=0;j<map_size;j++) {
			struct node *head;
			while((head = pop(&STATES[i].transitions.buckets[j]))) free(head);
			free(STATES[i].transitions.buckets[j]);
		}
		free(STATES[i].transitions.buckets);
	}

	free(history);

}

