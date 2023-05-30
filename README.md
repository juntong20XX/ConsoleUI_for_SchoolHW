# Console UI for School Homework

It's a sample UI tool for help you finish your boring school homework/assessment/assignment.

You can see examples in `ConsoleUI_for_SchoolHW.cpp`.

##  Features

- [x] Windows :+1:
- [ ] Linux :+1: (without test)
- [x] ≥ C++ 11 :+1:
- [x] CMake :+1:
- [ ] GNU GCC Tool Chain :+1: (without test)
- [x] Visual Studio :+1:
- [x] MinGW :+1:
- [x] Chain Programming :+1:
- [ ] Complete Framework :x:

## Install

To add it into your project, you just need download `ConsoleUI_for_SchoolHW.hpp`. Include it as a header, and enjoy it!

## Usage

Include `ConsoleUI_for_SchoolHW.hpp`, get the main class by reference, like `auto & ui = ConsoleUI::UI::get_instance()`. Be careful that the object is thread-safe, but its methods are not.

Set group for your UI pages. By `ui.set_group(GROUP_NAME)`, then call `.add` method after that.

```cpp
auto &ui = ConsoleUI::UI::get_instance();
ui.set_group("HW")
        .add("Hello")
        .add("world")
        .add("!");
ui.show_group("HW");
```

Show your UI group by `ui.show_group`. For example, in the case above, you will see the following text on your console:

```text
****************** HW *******************
*                 Hello                 *
*                 world                 *
*                   !                   *
*****************************************
```

There are also serval methods to show the groups, all of them starts with `show_group_`, you can check them.
