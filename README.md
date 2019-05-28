# InferenceEngine2019
J.P. Galovic - 6819710

## Features
This program implements a phaser to read a file and correctly identify a Knowledge Base containing Horn-Form clauses and
Determine a queiry to be solved/proven.

The file provided needs to be made up of 4 descrete in the following format:

TELL
knowledge; base; statements; seperated; by; semicolons;
ASK
queiry

The phaser can recognise and decode the following symbols:
implication, equivalence, conjunction, disjunction, negation and literals.
'=>'       , '<=>'      , '&'        , '\/'       , '~'        , x (any other char).
The phaser ignores any blank statments: ' ';
The phaser recognises and can implement correct use of parenthesis: '(' ')'

The phaser produces knowledgebase of horn clauses, which interally hold the stucture as follows:

```c++
HornClause {
  ClauseType : fOperand; // Operand of defined clause, implicaiton or equivlance take priority over conjuction and disjunction
  HornClause : fP; // Subclause containing definintion of everything left of the Operand. This is also known as the premise
  HornClause : fQ; // Subclause containing definintion of everything right of the Operand. This is also known as the conclusion
  HornClause : fC; // In the cases of Parenthesis or Negation, subclause is defined here.
}
```

the HornClause class contains all relivant functions for determining the truth of the statement based on the current set values of the literals contained, functions for setting truth of literals, and fetching elements such as truth tables.

The program contains three alogrithms for proving the queiry:
### TT: TruthTable Lookup
A truth table containing solutions for all possible settings is generated (this may take some time), printed to the screen. then the number of scenarios where the knowledgebase is validated determined and printed out.

### FC: Forward Chaining
Starting from Known Literals (cases where only a literal is defined in the knowledgebase), this algorithem uses forward chaining to search the knowledge base for connected clauses until the queiry can or cannot be proven. the algorithem should be built on the following psudo-code:

```c++
//kb is the knowlidgebase
//q is teh queiry
vector[clause, int] count; //List of clauses with a count of number of literals in premise.
vector[literal, bool] inferred; // list of infired pairs, its initalised to contain each literal found in knowledgebase and to be false.
queue[literals] agenda; //queue containing literals to be checked, its initally loaded with Literals soly defined in kb

while(!agenda.empty())
{
  p = agenda.pop();
  if(p == q)
    return true;
  else
  {
    if(inferred[p] == false)
    {
      inferred[p] = true; // All literals that end up in agenda are inferred.
      foreach(clause c that contains p in premise)
      {
        decrement count[c]
        if(count[c] == 0)
          add c.conclusion to agenda;
      }
    }
  }
}
return false;
```

### BC: Bacward Chaining
Chains backward from query, showing all literals that lead to query.

## Test Cases
The program was tested with the assignment provided KB and queiries.
In both cases the TT was checked against an online truth table generator to determine clauses are read and interperited as expected.

## Resources
https://web.stanford.edu/class/cs103/tools/truth-table-tool/
Truth table generator used to check Horn Clauses were read correctly and produced the correct output.

https://en.wikipedia.org/wiki/List_of_logic_symbols
used to aid development of truth table definitions (listed above)

## Notes
before the selected algorithm is run, a series of truth tables for each statement will be shown.
press any key to continue to Agent execution.

the program (.exe) requires two arguments be set, firstly the dir address for the file to be read, 
and the selected algorithm, TT, FC or BC. errors will be displayed then upon exit one of the following will be returned:

Code : Description
0    : Program exited on success (meaning no errors encounter)
1    : Program exited on error, Incorrect number of argument set, usage: <FILENAME> <TT|FC|BC METHOD>
2    : Program exited on error, Method not in accepted format, usage: Truth Table (TT), Forward Chaining (FC) and Backward Chaining (BC).
3    : Program exited on error, File cannot be opened or file not found.
4    : Program exited on unknown error.
1000 : Runtime Error, see error text displayed
1001 : KB does not contain literal with name used in query.

## Summary Report
This assignment was fully developed by myself J.P. Galovic. I opted not to work in a group.
