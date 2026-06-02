
```pseudo
\begin{algorithm}  
\caption{AggregateInitStyleCheck: Core Logic}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry)  
\State 2. $\to$ CheckArrayInit (if array)  
\State 3. $\to$ CheckRecordInit (if struct)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State $\text{ile} \gets$ matched InitListExpr  
\State $\text{ilesyn} \gets$ syntactic form

\If{VarDecl with array or struct}  
\If{array type}  
\State $\text{CheckArrayInit}(\text{ile})$  
\EndIf  
\If{struct type}  
\State $\text{CheckRecordInit}(\text{ile})$  
\EndIf  
\EndIf  
\EndFunction

\State $\phantom{x}$

\Function{CheckArrayInit}{$ILEsyn$}  
\If{zero init}  
\State \Return  
\EndIf

\State $\text{declsize} \gets$ array element count  
\State $\text{initcount} \gets$ actual init elements  
\If{$\text{initcount} \neq \text{declsize}$}  
\State $\text{report}(\text{element count mismatch})$  
\EndIf  
\EndFunction

\State $\phantom{x}$

\Function{CheckRecordInit}{$ILEsyn$}  
\If{zero init}  
\State \Return  
\EndIf

\State $\text{fieldcount} \gets$ struct fields  
\State $\text{initcount} \gets$ actual init elements  
\If{$\text{initcount} \neq \text{fieldcount}$}  
\State $\text{report}(\text{field count mismatch})$  
\EndIf  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```


```pseudo
\begin{algorithm}
\caption{AlwaysConstantConditionCheck Core Logic}
\begin{algorithmic}

  \State \textbf{Function Call Order:}
  \State 1. registerMatchers (Entry)
  \State 2. $\to$ check (on each matched function definition)
  \State 3. $\to$ analyzeFunctionBody (for each function body)
  \State 4. $\to$ FuncVisitor.run (AST traversal)
  \State 5. $\to$ analyzeConditionNode (for each condition expression)
  
  \State $\phantom{x}$

  \Function{registerMatchers}{$Finder$}
    \State $Finder.addMatcher(\text{function definition}, \text{this})$
  \EndFunction

  \State $\phantom{x}$

  \Function{check}{$MatchResult$}
    \State $FD \gets \text{matched function definition}$
    \If{FD has no body}
      \State \Return
    \EndIf
    \State $\text{analyzeFunctionBody}(FD)$
  \EndFunction

  \State $\phantom{x}$

  \Function{analyzeFunctionBody}{$FD$}
    \State $V \gets \text{new FuncVisitor with } FD$
    \State $V.run()$
  \EndFunction

  \State $\phantom{x}$

  \Function{FuncVisitor.run}{}
    \If{FD has no body}
      \State \Return
    \EndIf
    \State $\text{pushGlobalEnv}()$
    \State $\text{TraverseStmt}(FD.body)$
    \State $\text{popEnv}()$
  \EndFunction

  \State $\phantom{x}$

  \Function{analyzeConditionNode}{$Cond, Loc$}
    \If{Cond is null or should not report at Loc}
      \State \Return
    \EndIf
    \If{EnableConstantFoldPhase or EnableTypeRangePhase is true}
      \State $result \gets \text{isAlwaysConstNoContext}(Cond)$
      \If{result indicates always true or false}
        \State $\text{diag}(Cond.location, result)$
        \State \Return
      \EndIf
    \EndIf
    \If{EnablePathConstraintPhase is false}
      \State \Return
    \EndIf
    \If{Cond is var compared to constant value}
      \State $isAlways \gets \text{alwaysUnderInterval for current constraint}$
      \If{isAlways is true}
        \State $\text{diag}(Cond.location, isAlways)$
      \EndIf
    \EndIf
  \EndFunction

\end{algorithmic}
\end{algorithm}

```




```pseudo
\begin{algorithm}  
\caption{NoShadowingCheck: Variable Shadowing Detection}  
\begin{algorithmic}

\State \textbf{Function Call Order:}  
\State 1. Check (Entry)  
\State 2. $\to$ ShadowVisitor::TraverseFunctionDecl (scope initialization)  
\State 3. $\to$ ShadowVisitor::VisitDeclStmt (declaration handling)

\State $\phantom{x}$

\Function{Check}{$MatchResult$}  
\State $\text{target} \gets$ matched FunctionDecl or CXXMethodDecl  
\If{target has no body}  
\State \Return  
\EndIf  
\State $\text{visitor} \gets$ new ShadowVisitor  
\State $\text{visitor.traverse}(target)$  
\EndFunction

\State $\phantom{x}$

\Function{TraverseFunctionDecl}{$FD$}  
\State $\text{ScopeStack.push}()$ new frame  
\If{CheckParamShadowing enabled}  
\For{each parameter in FD}  
\State $\text{register parameter in current scope}$  
\EndFor  
\EndIf  
\State traverse function body  
\State $\text{ScopeStack.pop}()$  
\EndFunction

\State $\phantom{x}$

\Function{VisitDeclStmt}{$DS$}  
\For{each declaration in DS}  
\If{declaration is VarDecl}  
\State $\text{handleVarDecl}(decl)$  
\EndIf  
\If{CheckStructuredBindings and is BindingDecl}  
\State $\text{handleBindingDecl}(decl)$  
\EndIf  
\EndFor  
\EndFunction

\State $\phantom{x}$

\Function{HandleVarDecl}{$VD$}  
\State $\text{name} \gets$ variable name  
\State $\text{kind} \gets \text{classifyShadow}(name, VD)$  
\If{kind is OuterLocal}  
\State $\text{reportDiagnostic}(\text{outer scope shadowing})$  
\EndIf  
\If{kind is Global}  
\State $\text{reportDiagnostic}(\text{global variable shadowing})$  
\EndIf  
\If{OfferRenameFixIt enabled}  
\State $\text{suggestRename}(name)$  
\EndIf  
\State $\text{ScopeStack.back().insert}(name, VD)$  
\EndFunction

\State $\phantom{x}$

\Function{ClassifyShadow}{$Name, Local$}  
\If{ForbidOuterLocalShadowing}  
\For{each outer scope frame in ScopeStack}  
\If{name exists in outer frame}  
\State \Return OuterLocal  
\EndIf  
\EndFor  
\EndIf  
\If{ForbidGlobalShadowing}  
\State $\text{global} \gets \text{lookupGlobal}(name)$  
\If{global found}  
\State \Return Global  
\EndIf  
\EndIf  
\State \Return None  
\EndFunction

\end{algorithmic}  
\end{algorithm}
```




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




