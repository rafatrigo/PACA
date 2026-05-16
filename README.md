# PACA (Parser of Arguments for C++ Applications)

PACA is a modern, header-only command-line argument parsing library for C++ focused on simplicity, performance, and strong type safety.

---

## Project Status & Roadmap

We are incrementally building **PACA** towards a stable **v1.0** release. The project is structured into the following phases:

### Phase 1 (v0.1)
Foundation, tokenization, basic boolean flags, and auto-help generation.

### Phase 2 (v0.2)
Typed values (`std::string`, `int`, `bool`), named options, and conversion error handling.

### Phase 3 (v0.3)
Grouped flags and validation constraints (e.g., `required()`).

### Phase 4 (v0.4)
Subcommands with isolated execution contexts and independent help menus.

### Phase 5 (v0.5)
Extensibility for custom C++ structs and loading arguments from configuration files.

### Phase 6 (v1.0)
Polish, real-world examples, documentation, and production readiness.

---

For detailed technical goals, architectural designs, and implementation focus areas for each phase, please refer to the complete [ROADMAP.md](./ROADMAP.md).
