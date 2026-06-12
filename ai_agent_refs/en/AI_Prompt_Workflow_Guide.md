# AI Prompt Engineering Workflow Guide

This document defines the official prompt engineering workflow to be followed when creating new AI prompts or maintaining existing ones within the GRC project.

To efficiently build high-quality and reliable prompts, we recommend following the **"Three-Step Prompt Assembly (Build - Audit - Test)"** method, which combines our two core guidelines: `AI_Prompt_Engineering_Guidelines.md` and `AI_Prompt_AntiPatterns_Guidelines.md`.

---

## 🏗️ Step 1: Mindset Setting & Structural Blueprinting (Engineering-Driven)

When designing and drafting a prompt from scratch, use **`AI_Prompt_Engineering_Guidelines.md`** as your blueprint.

- **[Action 1] Structure:** Build the skeleton of the prompt using explicit XML tags such as `<system_directive>`, `<rules>`, and `<output_format>`.
- **[Action 2] Data Isolation:** Ensure user feedback or dynamic context does not mingle with system instructions by encapsulating them in dedicated spaces like `<user_feedback>` or `<worldview>`.
- **[Action 3] Few-Shot Readiness:** If the output format is complex or critical, always include `<example>` tags containing ideal input-output pairs.

---

## 🔍 Step 2: Trap Avoidance & Auditing (Anti-Pattern-Driven)

Once a draft is ready, do not consider it finished. Audit the prompt rigorously using **`AI_Prompt_AntiPatterns_Guidelines.md`** as a safety checklist.

- **[Check 1] Strip Redundancies:** Scan for and remove phrases like "think step-by-step (CoT)" or "do not make mistakes."
- **[Check 2] Remove Vague Negative Rules (Pink Elephant Problem):** Replace negative instructions ("Do not do X") with positive, actionable alternatives ("Only do Y").
- **[Check 3] Optimize Layout (Lost in the Middle):** Ensure critical rules, constraints, or final formatting requirements are not buried in the middle. Relocate them to the very top (System Instructions) or inside the `<final_instruction>` block at the very bottom.

---

## 🛠️ Step 3: Run, Test, and Refine (Execution-Driven)

Execute the prompt in target LLMs (e.g., Gemini). If anomalies occur, debug them using the cross-reference guide below:

### Symptom A: Model ignores rules or exhibits hallucinations
> **💡 Debug Path:** Review `AI_Prompt_AntiPatterns_Guidelines.md`.
- *Check:* Are there passive negative constraints ("Do not...") that can be rephrased positively?
- *Check:* Are critical constraints lost in the middle of a large context window?

### Symptom B: Output format (e.g., JSON schema) is broken or inconsistent
> **💡 Debug Path:** Review `AI_Prompt_Engineering_Guidelines.md`.
- *Check:* Are the few-shot examples inside `<example>` tags insufficient or poorly formatted?
- *Check:* Is the isolation between system instructions and user data weak, leading to prompt leakage or system instruction overrides?

---

**Related Guidelines:**
- Blueprint Design: [AI_Prompt_Engineering_Guidelines.md](file:///c:/Users/user/Documents/GitHub/Obsidian/ai_agent_refs/en/AI_Prompt_Engineering_Guidelines.md)
- Risk Avoidance: [AI_Prompt_AntiPatterns_Guidelines.md](file:///c:/Users/user/Documents/GitHub/Obsidian/ai_agent_refs/en/AI_Prompt_AntiPatterns_Guidelines.md)
