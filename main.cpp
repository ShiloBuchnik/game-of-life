#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <cmath>
#include "game_logic.h"
#include "patterns.h"

int main(){
    short int mode_num = introduction();

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Default);
    bool focus = true; // True iff focus is on window.
    // 1st and 2nd arguments are top-left coordinates of the rectangle. 3rd and 4th arguments are its width and height.
    // Our initial view is exactly at the middle of the grid.
    sf::View view(sf::FloatRect(WINDOW_WIDTH * (MULTIPLE / 2), WINDOW_HEIGHT * (MULTIPLE / 2), WINDOW_WIDTH, WINDOW_HEIGHT));
    // Since our initial view is different from the default view, we have to set it before we continue.
    window.setView(view);

    if (mode_num == 1) getUserInput(window, view, grid, focus); // Custom pattern
    else Patterns::putPatternInGrid(grid, *Patterns::numToPattern[mode_num]); // Pre-defined pattern

    sf::Clock timestep_clock, key_press_clock;
    long long int gen = 0;
    while (window.isOpen()){
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch(evnt.type){
                // The program exits when user closes the window by pressing 'X'.
                case sf::Event::Closed:
                    window.close();
                    return 0;
                    break;

                case sf::Event::KeyPressed:
                    // The program exits when user presses 'esc'.
                    if (evnt.key.code == sf::Keyboard::Escape){
                        window.close();
                        return 0;
                    }

                    // The grid "restarts" when user presses 'Enter'.
                    if (evnt.key.code == sf::Keyboard::Enter) grid.clear();
                    break;

                // Because 'isKeyPressed' is "connected" to the actual device, it's getting input even when window is out of focus.
                // We want to accept keyboard input only when the window has focus, so we have to keep track of it using a boolean.
                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    break;

                case sf::Event::Resized: {
                    /* By default, when resizing, everything is squeezed/stretched to the new size.
                    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down
                    just like in a windowed video game.

                    By calling 'mapPixelToCoords(0,0)' we get *the view's* top-left corner.
                    We pass that same top-left corner with the new width and height to the 'reset()' method.

                    If we'd only change the width and height, without keeping the top-left corner the same,
                    then it would change everytime we'd resize. */
                    sf::Vector2i top_left_view_pos = static_cast<sf::Vector2i> (window.mapPixelToCoords(sf::Vector2i(0, 0)));
                    view.reset(sf::FloatRect(top_left_view_pos.x, top_left_view_pos.y, evnt.size.width, evnt.size.height));
                    window.setView(view);
                    break;
                }
            }
        }

        /* Rule of thumb: if we need *real time* and continuous keyboard input, it's better to use 'isKeyPressed' than to rely on events.
        This is because events are polling, while 'isKeyPressed' is "connected" to the actual device.
        If we check keyboard input with event, there's a slight delay, plus there's the OS built-in delay on a long press (like in text editors).
        On the other hand, 'isKeyPressed' enables smooth movement.

        But when moving this logic outside the event loop we face a problem:
        The loop executes VERY fast, and pressing a key (even for a split second) would make us enter the 'if' body multiple times.
        Why is this bad? Because the amount of times we'd enter the body *depends on the rest of the iteration*.
        If the following logic takes a long time to execute, we'd enter a small amount of times;
        and if it's short, we'd enter a large amount of times.

        The solution is to use 'delta_time' to measure the time between iterations, and multiplying by 'SPEED', to get *length* by which we move.
        Now we're dependent only at the *duration of time the key was pressed*!
        This is essentially a normalization, since now we move on the same speed no matter the while loop we're in. */
        float delta_time = (float) key_press_clock.restart().asMilliseconds();
        if (focus && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            view.move(sf::Vector2f(0, std::round(-1 * SPEED * delta_time)));
            window.setView(view);
        }
        if (focus && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
            view.move(sf::Vector2f(std::round(-1 * SPEED * delta_time), 0));
            window.setView(view);
        }
        if (focus && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
            view.move(sf::Vector2f(0, std::round(SPEED * delta_time)));
            window.setView(view);
        }
        if (focus && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
            view.move(sf::Vector2f(std::round(SPEED * delta_time), 0));
            window.setView(view);
        }

        // Using 'sleep' would lag other events like resizing or restarting the grid (since 'sleep' pauses the entire program for its duration).
        // A better solution is to use 'sf::clock', which lets us check for elapsed time.
        // When that elapsed time is bigger than our 'TIMESTEP' duration, we reset the clock and redraw the grid.
        // TODO: check if I need to add '|| grid.empty()' in condition.
        if (TIMESTEP <= timestep_clock.getElapsedTime()){
            timestep_clock.restart();

            updateGrid(grid);
            gen++;
            std::cout << "Generation: " << gen << std::endl;
        }

        window.clear();
        // If we got to an empty grid, that means we need to start again and ask for user's input.
        drawGrid(window, grid);
        if (grid.empty()){
            getUserInput(window, view, grid, focus);
            gen = 0;
        }
        window.display();
    }

    return 0;
}