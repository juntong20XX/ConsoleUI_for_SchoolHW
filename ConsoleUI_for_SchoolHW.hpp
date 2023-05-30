/**
 * @brief a sample tool to build Console UI for school homework
 * @author Juntong
 * @copyright MIT License
 *
 * Copyright (c) 2023 Juntong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <functional>
#include <exception>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <utility>
#include <cassert>
#include <vector>
#include <memory>
#include <map>


namespace ConsoleUI {
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
    int _getch() {
        struct termios old_settings, new_settings;
        tcgetattr(STDIN_FILENO, &old_settings);
        new_settings = old_settings;
        new_settings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

        int ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

        return ch;
    }
#endif
    /**
     * @brief The node to save UI information. Should only used by `UI` and `UiGroupBuilder` (not the tool user).
     */
    class UiNode {
    public:
        /**
         *
         * @param head: is head node or not
         * @param func: The call_void func, the function will be called to get the string to show.
         */
        explicit UiNode(const std::function<std::string(void)> &func) : use_text(false) {
            this->call_void = std::make_unique<std::function<std::string(void)>>(func);
        }

        explicit UiNode(const std::function<std::string(int)> &func) : use_text(false) {
            this->call_int = std::make_unique<std::function<std::string(int)>>(func);
        }

        explicit UiNode(const std::function<std::string(const std::string &)> &func) : use_text(false) {
            this->call_string = std::make_unique<std::function<std::string(const std::string &)>>(func);
        }

        explicit UiNode(std::string str) : use_text(true), text(std::move(str)) {}

        /**
         * @brief 更新 call 函数, 并将模式设置为通过调用函数获取显示文本, 注意不会删除之前设置的文本. Update the call function, and set the mode to get the display text by calling the function.
         * @note The previously set text will not be deleted.
         * @param new_func
         */
        void update_callback(const std::function<std::string(void)> &new_func) {
            this->call_int.reset();
            this->call_string.reset();
            this->call_void = std::make_unique<std::function<std::string(void)>>(new_func);
            this->use_text = false;
        }

        void update_callback(const std::function<std::string(int)> &new_func) {
            this->call_void.reset();
            this->call_string.reset();
            this->call_int = std::make_unique<std::function<std::string(int)>>(new_func);
            this->use_text = false;
        }

        void update_callback(const std::function<std::string(const std::string &)> &new_func) {
            this->call_int.reset();
            this->call_void.reset();
            this->call_string = std::make_unique<std::function<std::string(const std::string &)>>(new_func);
            this->use_text = false;
        }

        /**
         * @brief 更新 显示文本, 注意不会删除之前设置的函数.
         * Update the text to show.
         * @note Note that this operation will not remove the call-functions.
         * @param new_text
         */
        void update_text(const std::string &new_text) {
            this->text.clear();
            this->text.append(new_text);
            this->use_text = true;
        }

        /**
         * @brief 返回要显示的文本, 当需要调用函数返回时, 将为 call_int 传入 0, 为 call_string 传入 "".
         * Returns the text to display, when calling the function to return, pass 0 for call_int and "" for call_string.
         * @return
         */
        std::string get() const {
            return this->use_text ? this->text :
                   this->call_void ? (*this->call_void)() :
                   this->call_int ? (*this->call_int)(0) : (*this->call_string)("");
        }

        bool use_text{};
        std::unique_ptr<std::function<std::string(const std::string &)>> call_string;
        std::unique_ptr<std::function<std::string(void)>> call_void;
        std::unique_ptr<std::function<std::string(int)>> call_int;
        std::string text{};
    };

    /**
     * @brief The worker to update UI information.
     */
    class UiGroupBuilder {
    public:
        std::string group_name;
        std::shared_ptr<std::vector<UiNode>> nodes;

        /**
         * @brief Add the text into the group.
         * @param text the string to be show
         * @return
         */
        UiGroupBuilder &add(const std::string &text) {
            this->nodes->emplace_back(text);
            return *this;
        }

        /**
         * @brief 添加一个节点, 这个节点将在显示时调用传入的函数.
         * Adds a node that will call the passed function when displayed.
         * @param func 触发调用, 返回待显示的文本. Call and return the text to be displayed.
         * @return
         */
        UiGroupBuilder &add(const std::function<std::string(void)> &func) {
            this->nodes->emplace_back(func);
            return *this;
        };

        /**
         * @brief 在由 `ui.call_group_auto` 调用后触发, 用于 UI 自动刷新.
         * Called after `ui.call_group_auto` called.
         * @param func trigger function
         * @return
         */
        UiGroupBuilder &then(const std::function<std::string(void)> &func) {
            (*this->nodes)[0].update_callback(func);
            return *this;
        }

        UiGroupBuilder &then(const std::function<std::string(int)> &func) {
            (*this->nodes)[0].update_callback(func);
            return *this;
        }

        UiGroupBuilder &then(const std::function<std::string(const std::string &)> &func) {
            (*this->nodes)[0].update_callback(func);
            return *this;
        }
    };

    /**
     * @brief The main class.
     * @ You should get it will `auto &ui = UI::get_instance()` because it's a Singleton Class.
     * Be aware that **none** of its methods are thread-safe.
     *
     * @details
     * At start, we should set a group, like `ui.set_group(GROUP_NAME)`. Group names are global, you can set and reset
     * it any time.
     * @details
     * After set a group, you can add rows into it by use `.add` methods chained, such as
     * `ui.set_group("group 1").add("teacher").add("desk").add("students");`, `.then` can add a function when (and only
     * when) the group be called by `ui.call_group_auto`, at this point, it is more like the callback function pattern
     * of traditional UI frameworks.
     * @details
     * There is a preset special group called "EMPTY", the contents of this group are EMPTY, and when
     * `.show_group("EMPTY")` is called, nothing (including the group name) is displayed, or in other cases, the group
     * name is displayed even if there are no members in the group.
     *
     * @details
     * Two useful static function: `UI::clear` and `UI::ask`.
     *
     * @example Create a selection with three options.
     * auto &ui = UI::get_instance();
     * ui.set_group("group 1").add("teacher").add("desk").add("students");
     * int choose = ui.show_group_and_choose("group 1");
     *
     * @example Show a information.
     * auto &ui = UI::get_instance();
     * ui.set_group("NOTICE").add("a information");
     * ui.show_group("NOTICE");
     */
    class UI {
    public:

        static UI &get_instance() {
            static UI instance;
            return instance;
        }

        /**
         * Clear the console by using the system command.
         */
        static void clear() {
#ifdef __WINNT__
            std::system("cls");
#else
            std::system("clear");
#endif
        };

        /**
         * Set a ui group. Reset if the group name exist.
         * @param group_name
         * @return
         */
        UiGroupBuilder &set_group(const std::string &group_name) {
            this->builder.group_name = group_name;
            std::shared_ptr<std::vector<UiNode>> nodes = std::make_shared<std::vector<UiNode>>();
            this->groups[group_name] = nodes;
            this->builder.nodes = nodes;
            nodes->emplace_back("");
            return this->builder;
        }

        /**
         *
         * @param group_name
         */
        std::string call_group_auto(const std::string &group_name) {
            auto &group = this->groups[group_name];
            auto &head = (*group)[0];
            if (head.use_text) {  // shows no callback function
                this->show_group(group_name);
                return "";
            } else if (head.call_void) {
                this->show_group(group_name);
                return (*head.call_void)();
            } else if (head.call_int) {
                return (*head.call_int)(this->show_group_and_choose(group_name));
            } else if (head.call_string) {
                std::string val;
                this->show_and_get_group(group_name, val);
                return (*head.call_string)(val);
            } else {
                throw std::exception();  // UI frame has a bug.
            }
        }

        std::string get_body_text(const std::string &text) const {
            int length = (int) text.size();
            int adjust_length = (this->config_line_characters - length) / 2;
            std::stringstream ss;

            ss << "*" << std::setw(adjust_length) << ((adjust_length > 1) ? " " : "")
               << text
               << std::setw(adjust_length + 1) << "*";

            return ss.str();
        }

        std::string get_select_text(const std::string &text, const std::string &starts_with) const {
            int length = (int) (text.size() + starts_with.size());
            int adjust_length = this->config_line_characters - length;
            std::stringstream ss;

            ss << "*" << starts_with << text << std::setw(adjust_length) << "*";

            return ss.str();
        }

        std::string get_select_text(const std::string &text, bool selected, int row) const {
            std::stringstream ss;
            ss << (selected ? "->" : "  ") << std::setw(2) << row << ": ";
            return this->get_select_text(text, ss.str());
        }

        std::string get_title_text(const std::string &title) const {
            int length = (int) title.size();
            int adjust_length = (this->config_line_characters - length) / 2;
            std::stringstream ss;

            ss.fill('*');
            if (adjust_length > 1) {
                ss << std::setiosflags(std::ios::right) << std::setw(adjust_length) << " "
                   << title << " " << std::setw(adjust_length) << "";
            } else {
                ss << std::setiosflags(std::ios::right) << std::setw(adjust_length) << ""
                   << title << std::setw(adjust_length) << "";
            }

            return ss.str();
        }

        void show_group(const std::string &group_name) {
            auto &nodes = this->groups[group_name];

            if (group_name == "EMPTY" && nodes->size() == 1) {
                return;
            }

            std::cout << this->get_title_text(group_name);
            for (auto node = nodes->begin() + 1; node != nodes->end(); node++) {
                std::cout << std::endl << this->get_body_text(node->get());
            }
            std::cout << std::endl;
            std::cout << std::setfill('*') << std::setw(this->config_line_characters + 1) << "*";
        };

        /**
         * Show the group and make a selection with the choose from the group.
         * @param group_name
         * @return
         */
        int show_group_and_choose(const std::string &group_name) {
            auto &nodes = this->groups[group_name];
            int key, choose = 1, change;

            assert(nodes->size() > 1);

            while (true) {
                // Clear screen.
                ConsoleUI::UI::clear();
                // Show the information.
                std::cout << this->get_title_text(group_name);
                for (int index = 1; index < nodes->size(); index++) {
                    std::cout << std::endl
                              << this->get_select_text((*nodes)[index].get(), index == choose, index);
                }
                std::cout << std::endl << std::setfill('*') << std::setw(this->config_line_characters + 1) << "*";
                std::cout << std::endl << "use keyboard ↑↓ or TAB to choose, Enter to select ...";
                // Get input.
                change = ConsoleUI::UI::up_down(key);
                if (change == 0) {
                    if (nodes->size() <= 10 && key <= '9' && key >= '1') {
                        return key - '0';
                    } else if (key == 13) {  // '\n'
                        return choose;
                    }
                    // invalid input
                } else {
                    choose += change;
                    if (choose == 0) {
                        choose = (int) (nodes->size() - 1);
                    } else if (choose == nodes->size()) {
                        choose = 1;
                    }
                }
            }
        };

        template<typename T>
        static void ask(T &val, const std::string &prompt) {
            std::string s;
            std::stringstream ss;

            std::cout << std::endl << prompt;

            std::getline(std::cin, s);
            ss << s;
            ss >> val;
        };

        static int up_down() {
            int _;
            return up_down(_);
        }

        static int up_down(int &key) {
            while (true) {
                key = _getch();  // process security
                if (key == 0) {  // Leading bytes of up and down arrows
                    continue;
                }
                if (key == 'w' || key == 'W' || key == 'k' || key == 'K' || key == 72) {
                    // 72 means ↑
                    return -1;
                }
                if (key == 's' || key == 'S' || key == 'j' || key == 'J' || key == 80 || key == 9) {
                    // 80 means ↓ , 9 means TAB
                    return +1;
                }
                return 0;
            }
        }

        template<typename T>
        void show_and_get_group(const std::string &group_name, T &val) {
            this->show_and_get_group(group_name, val, "-> ");
        };

        template<typename T>
        void show_and_get_group(const std::string &group_name, T &val, const std::string &prompt) {
            this->show_group(group_name);
            UI::ask(val, prompt);
        };

        template<typename T>
        void
        show_and_get_group(const std::string &group_name, T &val, const std::string &prompt, T range_min, T range_max) {
            do {
                this->show_group(group_name);

                UI::ask(val, prompt);
            } while ((val < range_min) || (range_max <= val));
        };

        template<typename T>
        void show_and_get_group(const std::string &group_name, T &val, T range_min, T range_max) {
            std::stringstream ss;
            ss << "[" << range_min << " <= X < " << range_max << "] ->";
            this->show_and_get_group(group_name, val, ss.str(), range_min, range_max);
        };

        std::map<std::string, std::shared_ptr<std::vector<UiNode>>> groups;
        int config_line_characters = 40;

        // Singleton Pattern  Reference: https://cloud.tencent.com/developer/article/1695598
        UI &operator=(UI &other) = delete;

        UI(const UI &) = delete;

        UI &operator=(const UI &) = delete;

    protected:
        UI() {
            this->set_group("EMPTY");
        };

        ~UI() = default;

    private:
        UiGroupBuilder builder;

    };

}
