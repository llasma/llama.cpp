<!-- markdownlint-disable-file -->

# LLASMA

2026-04-21

(One day after 420!!)

# LLASMA — Large Language + Stack Machine Architecture

**LLM inference in pure C/C++ with embedded cxxforth stack machine + Microsoft BitNet support**

LLASMA is a purpose-built fork of [llama.cpp](https://github.com/ggerganov/llama.cpp) (on the `20260421` branch) that integrates a **trusted Forth-based stack machine** (via [cxxforth](https://github.com/kristopherjohnson/cxxforth)) directly into the inference loop.

The goal is to create **LLASMA agents**: systems that maintain a clear separation between:  
- **untrusted probabilistic knowledge** (from the LLM), and  
- **trusted executable skills** (implemented as reliable Forth words / Phoscript primitives in the stack machine).

## Philosophy

- Start with a tiny, auditable core of primitive stack operations (`DUP`, `SWAP`, `DROP`, `+`, `@`, `!`, `:`, `;`, etc.).
- Use the LLM to **propose** new skills in Forth syntax.
- Validate and **promote** safe proposals to the trusted dictionary.
- This gives the agent incremental, verifiable skill acquisition while keeping execution deterministic and sandboxable.

This approach aligns with reliable agent design: the stack machine is the "doer", the LLM is the "thinker/knower".

## Key Features

- Full llama.cpp compatibility (LLaMA, LLaMA 2/3, Mistral, Mixtral, BitNet b1.58, and many others)
- Embedded **cxxforth** stack machine as the skill engine
- Microsoft **BitNet** (1.58-bit) model support for extreme efficiency
- All standard backends: CUDA, Metal, Vulkan, SYCL, hipBLAS, BLAS, etc.
- Safe skill acquisition pipeline (LLM proposal → validation → registration)
- Clear tracking of changes via the `CHANGES/` directory
- Minimal dependencies, high performance, runs on everything from laptops to servers

## Quick Start

### 1. Clone with submodule (for cxxforth)

```bash
git clone --recursive https://github.com/llasma/llama.cpp.git
cd llama.cpp
git checkout 20260421