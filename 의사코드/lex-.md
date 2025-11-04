
```pseudo
\begin{algorithm}
\caption{Include Path Validation Check}
\begin{algorithmic}

  \State \textbf{Function Call Order:}
  \State 1. HandleInclusionDirective (Entry)
  \State 2. $\to$ ExtractRawHeaderToken
  \State 3. $\to$ ValidateIncludePath
  
  \State $\phantom{x}$

  \Function{HandleInclusionDirective}{$hashLoc, fileName, isAngled, filenameRange$}
    \If{reporting is required for the source location}
      \State $rawToken \gets \text{ExtractRawHeaderToken}(filenameRange, fileName, isAngled)$
      \State $\text{ValidateIncludePath}(rawToken, filenameRange.getBegin())$
    \EndIf
  \EndFunction

  \State $\phantom{x}$

  \Function{ValidateIncludePath}{$rawToken, reportLocation$}
    \If{$size(rawToken.inner) > \text{MaxPathLength}$}
      \State $\text{reportError}(reportLocation, \text{Path is too long})$
    \EndIf
    \If{EnforceForwardSlashOnly is true and rawToken.inner contains a backslash}
      \State $\text{reportError}(reportLocation, \text{Backslash is not allowed})$
    \EndIf
    \If{AllowUnicode is false and rawToken.inner is not ASCII only}
      \State $\text{reportError}(reportLocation, \text{Non-ASCII characters found})$
    \EndIf
    \If{AllowSpaces is false and rawToken.spelled contains whitespace}
      \State $\text{reportError}(reportLocation, \text{Whitespace is not allowed})$
    \EndIf
    \If{rawToken.spelled contains comment patterns}
      \State $\text{reportError}(reportLocation, \text{Comment pattern found})$
    \EndIf
    \If{rawToken.inner contains forbidden characters}
      \State $\text{reportError}(reportLocation, \text{Forbidden characters found})$
    \EndIf
    \If{AllowParentPath is false and rawToken.inner contains parent path reference}
      \State $\text{reportError}(reportLocation, \text{Parent path is not allowed})$
    \EndIf
    \If{AllowDoubleSep is false and rawToken.inner contains double separators}
      \State $\text{reportError}(reportLocation, \text{Double separator is not allowed})$
    \EndIf
    \If{AllowAbsolutePath is false and rawToken.inner is an absolute path}
      \State $\text{reportError}(reportLocation, \text{Absolute path is not allowed})$
    \EndIf
    \If{delimiters in rawToken.spelled are mismatched}
      \State $\text{reportError}(reportLocation, \text{Mismatched delimiters})$
    \EndIf
  \EndFunction
  
  \State $\phantom{x}$
  
  \Function{ExtractRawHeaderToken}{$range, fallbackName, isAngled$}
    \State $spelledText \gets \text{getSourceText}(range)$
    \If{spelledText is empty}
      \State $rawToken.spelled \gets fallbackName$
      \State $rawToken.inner \gets fallbackName$
      \State \Return $rawToken$
    \EndIf
    \State $rawToken.spelled \gets spelledText$
    \If{spelledText starts and ends with matching delimiters}
      \State $rawToken.inner \gets \text{substring of spelledText without delimiters}$
    \EndIf
    \If{spelledText does not start and end with matching delimiters}
      \State $rawToken.inner \gets spelledText$
    \EndIf
    \State \Return $rawToken$
  \EndFunction

\end{algorithmic}
\end{algorithm}

```


```pseudo
\begin{algorithm}
\caption{Macro Definition Check Logic}
\begin{algorithmic}

  \State \textbf{Function Call Order:}
  \State 1. registerPPCallbacks (Entry)
  \State 2. $\to$ analyseCondition (on conditional directive)
  \State 3. $\to$ checkMacroDefinition (for each identifier in condition)

  \State $\phantom{x}$

  \Function{registerPPCallbacks}{$SM, PP$}
    \If{the Preprocessor PP is not valid}
      \State \Return
    \EndIf
    \State $PP.addPPCallbacks(\text{new MacroPPCallbacks}(SM, PP, \text{this}))$
  \EndFunction

  \State $\phantom{x}$

  \Function{analyseCondition}{$CondRange$}
    \If{the condition range is invalid}
      \State \Return
    \EndIf
    \State $SpellingLoc \gets \text{get spelling location of } CondRange.getBegin()$
    \If{OnlyMainFile is true and location is not in the main file}
      \State \Return
    \EndIf
    \If{IgnoreSystemHeaders is true and location is in a system header}
      \State \Return
    \EndIf

    \State $Buffer \gets \text{get source text from } CondRange$
    \If{the buffer is empty}
      \State \Return
    \EndIf

    \State $Lexer \gets \text{create a new Lexer for the buffer}$
    \State $Tracker \gets \text{create a new DefinedTracker}$

    \While{the lexer has more tokens}
      \State $Token \gets \text{get the next token from the lexer}$
      
      \If{the token is a raw identifier}
        \State $II \gets \text{get identifier info for the token}$
        \If{the identifier name is defined keyword}
          \State $Tracker.foundDefined()$
        \EndIf
        \If{the identifier name is not defined keyword}
          \If{$Tracker.shouldSkipMacro()$ is true}
            \State $Tracker.reset()$
          \EndIf
          \If{$Tracker.shouldSkipMacro()$ is false}
            \State $\text{checkMacroDefinition}(II, Token)$
          \EndIf
        \EndIf
      \EndIf
      
      \If{the token is not a raw identifier}
        \If{the token is a left parenthesis}
          \State $Tracker.foundLeftParen()$
        \EndIf
        \If{the token is not a left parenthesis}
          \If{the token is a right parenthesis}
            \State $Tracker.foundRightParen()$
          \EndIf
          \If{the token is not a right parenthesis}
            \State $Tracker.reset()$
          \EndIf
        \EndIf
      \EndIf
    \EndWhile
  \EndFunction

  \State $\phantom{x}$

  \Function{checkMacroDefinition}{$II, Token$}
    \State $Name \gets II.getName()$
    \If{IgnoreBuiltinMacros is true and the name is a builtin macro}
      \State \Return
    \EndIf

    \State $DiagLoc \gets \text{get spelling location of the token}$
    \State $Status \gets \text{get macro definition status for } II$

    \If{$Status$ is UNDEFINED}
      \If{WarnOnUndefined is true}
        \State $\text{diag}(DiagLoc, \text{"macro is not defined"})$
      \EndIf
    \EndIf
  \EndFunction

\end{algorithmic}
\end{algorithm}

```


