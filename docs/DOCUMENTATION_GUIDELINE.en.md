# Documentation Guideline — Async, Multi-Agent (Human + AI) Projects

> **Status:** authoritative, tool-facing copy · **Companion:** `DOCUMENTATION_GUIDELINE.ko.md` (human-reading mirror — keep structurally 1:1) · **Scope:** project-agnostic; drop into any repository as its documentation contract.

## 0. Read this first (purpose)

This project is worked on by **many people and many AI tools, in different threads and at different times, none of which share memory**. The documentation is the **only shared, persistent source of truth** for what the project is, why it is built the way it is, and where it currently stands.

This guideline defines how to write and maintain docs so that **any agent — human or AI — starting with no prior context can (1) reconstruct accurate project state from the docs alone, and (2) leave the docs accurate for whoever comes next.** Treat it as a contract, not a suggestion.

**The acceptance test for every document:** *Could a competent stranger, or a fresh AI thread, take over this work using only the docs, without asking the original author?* If not, the docs are incomplete.

## 1. Why we document

- **Docs are the project's memory.** Per-thread and per-person memory is private and temporary; docs are shared and permanent. **If it is not in the docs, it does not exist for the next agent.**
- **There is no shared "we."** A collaborator in another timezone was not in the conversation; an AI in a new thread has zero memory of previous threads. Never write as if the reader shares your context.
- **Docs carry state, decisions, and rationale** — not just what the code does, but *why it is this way, what was tried, what was rejected, what is settled, and what is still open.*

## 2. What must always be recoverable from the docs

1. **What exists** — the components and what each is for.
2. **Why it is this way** — the rationale and constraints behind the design.
3. **What is in progress** — current state, next steps, open questions.
4. **What is decided vs. open** — settled decisions (with reasons) kept distinct from proposals and unknowns.
5. **What must not be changed** — the invariants, and why they are load-bearing.

## 3. The document set (structure)

**Entry point — the index (`README`).** A map: which docs exist and when to read each. Every reader, human or AI, starts here.

**One document, one job.** Each doc has a single *purpose*, *audience*, and *change-axis*. Do not mix concerns in one file (see the merge/split test, §7).

**Standard document types:**

| Type | Answers | Changes when |
|---|---|---|
| Index / README | "What is here, where do I start?" | the doc set changes |
| Purpose & rationale | "What is this for, and why?" | goals/strategy change |
| Architecture | "How is it built, how do I extend it?" | structure changes |
| Reference (per component) | "Exactly how does X work / what are its I/O?" | that component changes |
| **Changelog** (append-only) | "What changed, and when?" | every change |
| **Decision log** | "Why this choice? What else was considered? Still in force?" | a decision is made or superseded |
| **Status / progress** | "Where are we right now?" | continuously |
| Troubleshooting / incidents | "This broke — cause and fix?" | a problem is solved |
| Contributor rules | "How do I change things safely?" | the process changes |

**Every document's header carries:** a one-line purpose, its intended audience, its status and last meaningful update, and links to related docs.

## 4. How to write docs (so they work as input for humans and AIs)

- **Bottom line first (BLUF).** Lead with the conclusion / most important fact; details follow.
- **Write for zero prior context.** No "as we discussed", no implicit "we", no unstated assumptions. The next reader was not here.
- **Label status explicitly.** Use a fixed vocabulary — **DECIDED / PROPOSED / OPEN / DEPRECATED** — and date it. An undated, unlabeled statement is ambiguous to an async reader.
- **One fact, one place.** State each fact once and link to it from elsewhere. Duplicated facts drift out of sync and become lies.
- **Stable, descriptive headings.** So they can be linked and cited precisely ("see §X" must keep its meaning over time).
- **Show, don't allude.** Include the actual command, branch name, code, or value — not a description of it.
- **Skimmable and complete.** Headings and short paragraphs for scanning; no missing steps for the reader who needs all of them.
- **Define each term once,** then use it consistently.

## 5. How we develop (workflow)

1. **Before changing anything, read the relevant docs.** They are your context. (AI threads: state what you read.)
2. **Log every change in the changelog as you make it** — not afterward, not in a batch.
3. **Record every decision** in the decision log: the choice, the rationale, the alternatives considered, and its status.
4. **Record every problem and its fix** in troubleshooting: symptom → cause → fix.
5. **Keep the status doc current.** It is the single place any agent checks to answer "where are we?"
6. **Never silently reopen a DECIDED item, and never treat a PROPOSED item as settled.** If you disagree with a decision, change its status explicitly and say why.
7. **Make changes small and legible,** each tied to a stated reason.
8. **End of session — update the docs before you stop.** Summarize what changed and reflect it in the affected docs. For AI threads this is mandatory: your in-thread memory does not carry to the next thread; only the docs do.

## 6. Rules specifically for AI agents

- **You begin with no memory of other threads.** The docs are your only reliable context. Read them before acting; do not assume any state the docs do not establish.
- **Do not invent state.** If the docs are silent on something, it is not established — find out, or mark it **OPEN** and ask. Never present an assumption as a settled fact.
- **Preserve nuance.** When recording a human's decision, capture it faithfully (quote where it matters); do not paraphrase the nuance away.
- **Leave the docs continuable.** Finishing the task includes leaving the docs in a state where the next agent can continue without you. This is part of the work, not an extra.
- **When unsure whether something is decided,** consult the decision log; if it is absent there, treat it as **OPEN**.

## 7. Merge or split? (a reusable test)

**Merge two documents only if they are read together for the same task *and* change together. Otherwise keep them separate and cross-link** at the points where they touch.

- The "why" (purpose / physics / product rationale) and the "how" (architecture / engineering) usually have **different audiences and different change-axes**, so they are normally **separate** documents, linked where the why constrains the how.
- A single combined document is justified only at a **narrow scope** (e.g., one small module) where the why and how are short, coupled, and always read together.

---
*This document obeys its own rules: bottom line first, explicit labels, one fact one place, stable headings. Keep it structurally 1:1 with the Korean reading mirror `DOCUMENTATION_GUIDELINE.ko.md`; this English copy is authoritative for tools.*
