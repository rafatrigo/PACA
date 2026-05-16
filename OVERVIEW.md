# Parser of Arguments for C++ Applications (PACA)

## Features

- pass arguments
    - support to `--argument=value`
- pass flags
    - support to group flags `-abc`
- support to subcommands
- auto generated help `--help`
- safe type
    - support custom types
- support to config file

## Structure

```
paca/
├── include/
│   └── cli/
│       ├── cli.hpp                 //include file
│       ├── parser.hpp
│       ├── option.hpp
│       ├── subcommand.hpp
│       ├── value.hpp
│       ├── exceptions.hpp
│       └── detail/
│           ├── parser_impl.hpp
│           └── convert_impl.hpp
│
├── examples/
├── tests/
├── docs/
├── CMakeLists.txt
└── README.md
```

## Usage

```cpp
#include <iostream>
#include <cli/cli.hpp>

int main(int argc, char** argv) {
    cli::Parser app;

    app.add_option("--name")
        .required()
        .help("Nome do usuário");

    app.add_option("--age")
        .default_value("18");

    app.add_flag("--verbose");

    app.parse(argc, argv);

    std::string name = app.get<std::string>("--name");
    int age = app.get<int>("--age");
    bool verbose = app.get<bool>("--verbose");

    std::cout << "Name: " << name << "\n";
    std::cout << "Age: " << age << "\n";

    if (verbose) {
        std::cout << "Verbose mode active\n";
    }
}
```
