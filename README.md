# Restricted Turing Machine

Written in pure C

## Usage

```
./restrictedTM <program> <input string> <max transitions>
```

## Explanation
The turing machine supports [0, 1000] states and any amount of unique transitions between them.
The input alphabet is 'limited' to digits, lower case letters or special characters {$,#,\_,%} but in reality any single ascii character should be allowed. For transitions of blanks, use the '\_' character

If the program ever enters an accept or reject state, it immediately halts and prints out the history of the states its visited in order followed by either 'accept' or 'reject'.

If the program attempts to transition more than `<max transitions>` times then the program halts and prints out the history of visited states followed by 'quit'


## Program file
The program file is a list of special states (start, accept, and reject), one per line, followed by the list of transitions, one per line.

The values are tab seperated.

There can only be one start state and 0 or more accept and reject states

The states are formatted like

state	x	type

where 'x' is the state number and type is either 'start' 'accept' 'reject'

e.g.

```
state	7	start
state	12	accept
state	952	reject
```


The transition lines are formatted like

```
transition	q	a	r	b	x
```

the format is 'q, a-\>r, b, x'

where q is the current state, a is the symbol that is read, r is the state that the machine transitions to, b is the value that the machine writes on top of the a, and the x is either L or R denoting which direction the tape head moves

q and r are in [0, 1000], a and b are ascii characters, x is either {L, R}


## Example program files

In examples there are two program:

- examples/example1.txt accepts strings over {$0^n1^n$| n>=0}
- examples/example2.txt accepts something that is left up to the reader to determine
