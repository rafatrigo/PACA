# Phase 1: v0.1 - Foundation and Basic Flags

## Goal

Establish the project skeleton, build system, and support for simple boolean flags and auto-generated help.

## Design & Architecture

- Set up `CMakeLists.txt` for a header-only library with test targets.
- Design the core `cli::Parser` class and the basic `ParseError` exception hierarchy.
- Implement basic `argc/argv` tokenization.

## Implementation Focus

- `include/cli/cli.hpp` *(main include)*
- `include/cli/parser.hpp` *(basic parser logic, adding flags)*
- `include/cli/exceptions.hpp` *(e.g., `cli::UnknownArgument`)*

## Deliverable

A CLI parser that can:

- Accept flags like `--verbose` or `-v`
- Ignore unknown arguments safely *(or throw exceptions)*

---

# Phase 2: v0.2 - Typed Values and Named Options

## Goal

Introduce value parsing with strong C++ type safety for standard types such as strings, integers, and booleans.

## Design & Architecture

- Introduce the `cli::Option` and `cli::Value<T>` abstractions.
- Implement internal conversion mechanisms using:
  - `std::stringstream`
  - `std::from_chars` *(for high performance)*
- Support both:
  - `--key=value`
  - `--key value`

## Implementation Focus

- `include/cli/option.hpp` *(builder pattern for fluent API)*
- `include/cli/value.hpp` *(safe retrieval of parsed values)*
- `include/cli/detail/convert_impl.hpp` *(type traits and string conversions)*

## Deliverable

A parser capable of:

- Extracting typed variables
- Setting default values when options are omitted
- Throwing `cli::ConversionError` for invalid inputs
- Automatically print a basic `--help` menu

---

# Phase 3: v0.3 - Grouped Flags and Validation

## Goal

Enhance usability for power users and enforce option constraints.

## Design & Architecture

- Implement POSIX-style grouped flag parsing:
  - `-abc` → `-a -b -c`
- Add validation logic:
  - `required()`
  - Restrict values to a predefined set of allowed values

## Implementation Focus

- `include/cli/detail/parser_impl.hpp`
  - Advanced token iteration and lookahead
- Updates to `include/cli/exceptions.hpp`
  - Example: `cli::MissingRequiredOption`

## Deliverable

A robust parser that:

- Fully supports standard UNIX command-line idioms
- Strictly validates user input against defined constraints

---

# Phase 4: v0.4 - Subcommands

## Goal

Support complex CLI applications like `git` or `docker` that use nested commands.

## Design & Architecture

- Design the `cli::Subcommand` class
  - Acts as a nested `cli::Parser`
- Implement routing logic:
  - Parsing halts at a subcommand token
  - Remaining `argv` is delegated to the subcommand parser

## Implementation Focus

- `include/cli/subcommand.hpp`
- Update `cli::Parser` to:
  - Hold subcommands
  - Route parsing to subcommands

## Deliverable

Support for commands like:

```bash
app server start --port 8080
```

**With**:

- Separete options
- Independent help meunus
- Isolated execution contexts

**For example**:

- `server`
- `start`

---

# Phase 5: v0.5 - Custom Types and Configuration Files

## Goal

Provide extensibility for user-defined types and alternative input sources.

## Design & Architecture

- Provide a clean template specialization API in `convert_impl.hpp`
- Allow users to parse directly into custom structs

**Example**:

```cpp
struct IPv4 {
    ...
};
```

- Add support for fallback arguments from:
    - `.conf`
    - `.ini`

## Implementation Focus

- Extensions to `include/cli/detail/convert_impl.hpp`
- Add file reading logic to `parser_impl.hpp`
- Inject arguments before or after `argv`

## Deliverable

A highly extensible parser where developers can:

- Inject complex business logic types directly from the command line
- Load arguments from configuration files

---

# Phase 6: v1.0 - Polish, Examples, and Documentation

## Goal

Production readiness.

## Focus

- Refine auto-generated `--help` formatting:
  - Alignment
  - Wrapping
  - Grouping
- Populate the `examples/` directory with real-world scenarios
- Finalize `README.md` with:
  - Integration instructions
  - `CMake FetchContent` examples
  - Quick-start guide

## Deliverable

The official **1.0 release** of **PACA**.

