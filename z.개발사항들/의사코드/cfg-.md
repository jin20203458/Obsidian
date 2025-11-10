
```pseudo
\begin{algorithm}  
\caption{InitBeforeUseCheck: Uninitialized Variable Detector}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry)  
\State 2. $\to$ analyzeFunction (Worklist fixpoint)  
\State 3. $\to$ processBlock (Transfer function)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State $\text{FD} \gets$ matched FunctionDecl  
\State $\text{analyzeFunction}(\text{FD})$  
\EndFunction

\State $\phantom{x}$

\Function{analyzeFunction}{$FD$}  
\State $\text{CFG} \gets \text{buildCFG}(\text{FD.body})$  
\State collect local vars and build index map

\State initialize InState and OutState for all blocks  
\State $\text{InState}[\text{Entry}] \gets$ all uninit

\State $\text{worklist} \gets$ Entry successors

\While{worklist not empty}  
\State $B \gets$ pop block  
\State $\text{NewIn} \gets$ meet all pred OutStates  
\If{NewIn changed}  
\State $\text{InState}[B] \gets \text{NewIn}$  
\EndIf  
\State $\text{NewOut} \gets \text{processBlock}(B, \text{InState}[B])$  
\If{NewOut changed}  
\State $\text{OutState}[B] \gets \text{NewOut}$  
\State enqueue all successors  
\EndIf  
\EndWhile  
\EndFunction

\State $\phantom{x}$

\Function{processBlock}{$B, InBits$}  
\State $S \gets \text{InBits}$

\For{element in B}  
\If{is DeclStmt}  
\For{Decl with init}  
\State $S \gets \text{evalExpr}(\text{init}, S)$  
\State mark var as initialized  
\EndFor  
\EndIf  
\If{is Expr}  
\State $S \gets \text{evalExpr}(\text{Expr}, S)$  
\EndIf  
\EndFor

\State \Return $S$  
\EndFunction

\State $\phantom{x}$

\Function{evalExpr}{$E, State$}  
\If{DeclRefExpr read}  
\If{not initialized}  
\State report use-before-init  
\EndIf  
\EndIf

\If{assignment}  
\State eval RHS then mark LHS init  
\EndIf

\If{logical operator}  
\State meet both branch states  
\EndIf

\State eval children recursively  
\State \Return State  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```



```pseudo
\begin{algorithm}  
\caption{NonZeroDivisorGuardCheck: Division-by-Zero Guard Detector}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry - match division/modulus operators)  
\State 2. $\to$ isDefinitelyNonZeroAtUse (CFG dataflow analysis)  
\State 3. $\to$ applyTransferInBlockUntil (Transfer function)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State $\text{Op} \gets$ matched binary or operator call  
\State $\text{Den} \gets$ divisor expression  
\If{Den is constant zero}  
\State report "constant zero divisor"  
\State \Return  
\EndIf

\State $\text{VD} \gets$ referenced variable from Den  
\If{VD is null or floating-point excluded}  
\State \Return  
\EndIf

\State $\text{FD} \gets$ enclosing function of Op  
\State $\text{Safe} \gets \text{isDefinitelyNonZeroAtUse}(VD, Op, FD)$  
\If{not Safe}  
\State report "divisor not guaranteed nonzero on all paths"  
\EndIf  
\EndFunction

\State $\phantom{x}$

\Function{isDefinitelyNonZeroAtUse}{$VD, UseStmt, FD$}  
\State build CFG for function body  
\State find CFGBlock containing UseStmt

\State initialize In/Out bitsets to true (top=nonzero)  
\State $\text{In}[\text{Entry}] \gets$ false, $\text{Out}[\text{Entry}] \gets$ transfer

\State worklist $\gets$ all blocks except Entry  
\State $\text{steps} \gets 0$

\While{worklist not empty and steps $<$ Budget}  
\State $B \gets$ pop block  
\State $\text{newIn} \gets$ true

\For{Pred in preds of B}  
\State $\text{stateOnEdge} \gets \text{Out}[Pred]$  
\If{edgeImpliesNonZero of Term on edge}  
\State $\text{stateOnEdge} \gets$ implication result  
\EndIf  
\State $\text{newIn} \gets \text{newIn} \land \text{stateOnEdge}$  
\EndFor

\If{newIn changed}  
\State $\text{In}[B] \gets \text{newIn}$  
\State $\text{newOut} \gets \text{applyTransfer}(B, \text{In}[B])$  
\If{newOut changed}  
\State enqueue successors  
\EndIf  
\EndIf

\State $\text{steps} \gets \text{steps} + 1$  
\EndWhile

\State \Return $\text{applyTransferUntil}(\text{UseBlock}, \text{In}, UseStmt)$  
\EndFunction

\State $\phantom{x}$

\Function{applyTransferInBlockUntil}{$B, In, StopAt$}  
\State $S \gets In$

\For{Stmt in B until StopAt}  
\If{is decl init or assign nonzero}  
\State $S \gets$ true  
\EndIf  
\If{is decl init or assign zero}  
\State $S \gets$ false  
\EndIf  
\If{is assert nonzero}  
\State $S \gets$ true  
\EndIf  
\If{is unknown assign to VD}  
\State $S \gets$ false (lose info)  
\EndIf  
\EndFor

\State \Return $S$  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```




```pseudo
\begin{algorithm}  
\caption{NullDereferenceGuardCheck: Null Pointer Dereference Detector}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry - match dereference operations)  
\State 2. $\to$ isDefinitelyNonNullAtUse (CFG forward dataflow)  
\State 3. $\to$ applyTransferInBlockUntil (Transfer function)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State $\text{Deref} \gets$ matched dereference (*p, p->m, p[i])  
\State $\text{BaseE} \gets$ base expression  
\If{BaseE is not simple variable ref}  
\State \Return  
\EndIf

\State $\text{VD} \gets$ referenced VarDecl from BaseE  
\State $\text{FD} \gets$ enclosing function

\State $\text{Safe} \gets \text{isDefinitelyNonNullAtUse}(VD, Deref, FD)$

\If{not Safe}  
\State report "pointer may be null at dereference"  
\EndIf  
\EndFunction

\State $\phantom{x}$

\Function{isDefinitelyNonNullAtUse}{$VD, UseStmt, FD$}  
\State build CFG, find UseBlock containing UseStmt  
\State $\text{initialState} \gets$ AssumeNonNullParams or AssumeNonNullGlobals

\State initialize In/Out: Entry $\gets$ initialState, others $\gets$ true

\State worklist $\gets$ all blocks

\While{changed and steps $<$ Budget}  
\For{block B in worklist}  
\State $\text{newIn} \gets$ true

\For{pred in preds of B}  
\State $\text{stateOnEdge} \gets \text{Out}[\text{pred}]$  
\If{edgeImpliesNonNull of Term}  
\State $\text{stateOnEdge} \gets$ edge implication result  
\EndIf  
\State $\text{newIn} \gets \text{newIn} \land \text{stateOnEdge}$  
\EndFor

\If{newIn changed}  
\State $\text{In}[B] \gets \text{newIn}$  
\EndIf

\State $\text{newOut} \gets \text{applyTransfer}(B, \text{In}[B])$  
\If{newOut changed}  
\State $\text{Out}[B] \gets \text{newOut}$  
\EndIf  
\EndFor  
\EndWhile

\State \Return $\text{applyTransferUntil}(\text{UseBlock}, \text{In}, UseStmt)$  
\EndFunction

\State $\phantom{x}$

\Function{applyTransferInBlockUntil}{$B, In, StopAt$}  
\State $S \gets In$

\For{Stmt in B until StopAt}  
\If{is addr-of init or new expr}  
\State $S \gets$ true  
\EndIf  
\If{is null init or null assign}  
\State $S \gets$ false  
\EndIf  
\If{is assert non-null}  
\State $S \gets$ true  
\EndIf  
\If{is unknown init or assign}  
\State $S \gets$ false (lose guarantee)  
\EndIf  
\EndFor

\State \Return $S$  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```



```pseudo
\begin{algorithm}  
\caption{NullPointerArithmeticCheck: Null Pointer Arithmetic Detector}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry - match pointer arithmetic operations)  
\State 2. $\to$ operandMayBeNullOnSomePathCFG (Path-sensitive analysis)  
\State 3. $\to$ existsWitnessPathNullToUse (Backward CFG search)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State match: $*p++$, $p+i$, $p+=i$, $p[i]$, or similar  
\If{operand may be null on some path}  
\State report "null pointer arithmetic"  
\EndIf  
\EndFunction

\State $\phantom{x}$

\Function{operandMayBeNullOnSomePathCFG}{$PtrExpr, UseSite$}  
\If{PtrExpr is null constant}  
\State \Return true  
\EndIf

\State $\text{VD} \gets$ referenced var from PtrExpr  
\If{VD is null}  
\State \Return false  
\EndIf

\State build CFG for enclosing function  
\State find CFGPos of UseSite in CFG

\If{same block, write to VD before Use}  
\State \Return false (always non-null)  
\EndIf

\State check if same block write exists  
\State \Return existsWitnessPathNullToUse  
\EndFunction

\State $\phantom{x}$

\Function{existsWitnessPathNullToUse}{$CFG, UsePos, VD$}  
\State queue $\gets$ UseBlock  
\State visited $\gets$ empty

\While{queue not empty and budget $>$ 0}  
\State $\text{cur} \gets$ pop from queue

\For{pred in preds of cur}  
\If{already visited}  
\State continue  
\EndIf

\If{edge pred$\to$cur implies null}  
\State \Return true (found witness path)  
\EndIf

\State push pred to queue  
\EndFor  
\EndWhile

\State \Return false  
\EndFunction

\State $\phantom{x}$

\Function{edgeConditionImpliesNull}{$Pred, Succ, VD$}  
\State $\text{Term} \gets \text{terminator of Pred}$  
\State determine if pred$\to$succ is then or else branch

\If{$\text{Term}$ is IfStmt with condition $C$}  
\If{$C$ is (not p) and in then branch}  
\State \Return true  
\EndIf  
\If{$C$ is (p) and in else branch}  
\State \Return true  
\EndIf  
\If{$C$ is (p == 0/nullptr) and in then}  
\State \Return true  
\EndIf  
\If{$C$ is (p != 0/nullptr) and in else}  
\State \Return true  
\EndIf  
\EndIf

\State \Return false  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```



