# AI Prompt Anti-Patterns Guidelines (2024-2026)

This document defines the official prompt anti-patterns guidelines for the GRC Project, outlining pitfalls to avoid when working with state-of-the-art Large Language Models (LLMs) and Reasoning/Thinking Models. All prompt engineering work must strictly adhere to these guidelines in conjunction with `AI_Prompt_Engineering_Guidelines.md`.

These instructions are tailored specifically for high-performance models (2024–2026+), such as Google Gemini 3.x, OpenAI o1/o3/GPT-4o, and Anthropic Claude 3.5+.

---

## 1. Do Not Force Chain-of-Thought (CoT) on Reasoning/Thinking Models

**❌ Anti-Pattern (Bad)**
> *"Think step-by-step."*
> *"Explicitly explain your reasoning process before outputting the final answer."*

**✅ Best Practice (Good)**
> Provide clear objectives, constraints, and data, and delegate the reasoning process entirely to the model.

**💡 Rationale:**
In earlier model generations, prompting the model to "think step-by-step" (CoT) was necessary to induce logical reasoning. However, modern reasoning/thinking models (such as OpenAI o-series, Gemini 2.0 Pro+, etc.) utilize an internal "Hidden Reasoning" (thought tokens) mechanism to determine the optimal reasoning path autonomously. 
Forcing a specific reasoning format or demanding "step-by-step thinking" disrupts the model's internal optimization, wastes compute tokens, and frequently results in **Performance Degradation**.

---

## 2. Emotional Appeals and Superfluous Politeness (Junk Tokens)

**❌ Anti-Pattern (Bad)**
> *"Please, I really need your help with this."*
> *"This is extremely important for my career. Please do your best."*
> *"Hello! How are you doing today? Could you please perform this task?"*

**✅ Best Practice (Good)**
> *"Analyze the following data and output the result."* (Be Direct)

**💡 Rationale:**
While some early research suggested that emotional appeals (emotional prompting) slightly boosted LLM performance, modern high-capacity models gain no performance benefits from such expressions. They merely waste context window space.
These expressions act as "Junk Tokens" that introduce noise, diluting the focus on core instructions. Adhere to Anthropic's **"Be Direct"** principle: omit conversational pleasantries and emotional framing, and deliver instructions in a clear, concise, and objective manner.

---

## 3. Ambiguous Negative Instructions (The Pitfalls of Negative Prompting)

**❌ Anti-Pattern (Bad)**
> *"Do not use markdown in your response."*
> *"Never mention apples."*
> *"Do not include the word [X] in the output."*

**✅ Best Practice (Good)**
> *"Output exclusively in plain text."*
> *"Describe only oranges and bananas."*

**💡 Rationale:**
This mimics the psychological **"Pink Elephant Problem"** (or white bear effect). LLMs predict tokens based on probability distributions. The moment a word you wish to forbid (e.g., "markdown", "apples") appears in the prompt, the attention weights (Attention) for that token increase.
Paradoxically, this increases the probability that the model will hallucinate or mistakenly output the forbidden term. Instead of listing exclusions, explicitly instruct the model on **what to do instead** using positive framing.

---

## 4. Placing Core Instructions in the Middle (Lost in the Middle Phenomenon)

**❌ Anti-Pattern (Bad)**
> Prompt Start (pleasantries)
> ... 
> (Thousands of tokens of RAG data/context)
> ...
> **[Core constraints or output format instructions]**
> ...
> (Additional data)
> ...
> Prompt End

**✅ Best Practice (Good)**
> **[Core rules and constraints (System Instructions / Top-level)]**
> ...
> (Thousands of tokens of context)
> ...
> **[Final execution prompt and output format re-emphasis (Final Instruction / Bottom-level)]**

**💡 Rationale:**
Even though state-of-the-art models support massive context windows (1M–2M+ tokens), deep learning attention mechanisms inherently prioritize information at the beginning (**Primacy Effect**) and the end (**Recency Effect**) of a document. Information buried in the middle suffers from significantly lower recall, a phenomenon known as **"Lost in the Middle."**
Crucial constraints, critical rules, or strict formatting requirements must be placed either at the **very top** (e.g., System Instructions) or at the **very bottom** (e.g., within a `<final_instruction>` tag).

---
**Scope:** All prompts in the GRC Project (MemoryManager, SessionArchitect, Lorebook, ReplySuggestion, etc.)
**Version:** 2026-06-12 (Based on 2024–2026 AI Research Papers & Developer Forum Guidelines)
