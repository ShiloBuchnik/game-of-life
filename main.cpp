#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <cmath>
#include "game_logic.h"
#include "patterns.h"

inline float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
    return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

int main(){
    short int mode_num = introduction();

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Default);
    bool focus = true; // True iff focus is on window.
    bool getting_input = false, clicking = false, dragging = false;

    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    sf::Vector2i left_top_view(WINDOW_WIDTH * (MULTIPLE / 2), WINDOW_HEIGHT * (MULTIPLE / 2));
    // 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
    // Our initial view is exactly at the middle of the grid.
    sf::View view(sf::FloatRect(left_top_view.x, left_top_view.y, WINDOW_WIDTH, WINDOW_HEIGHT));
    // Since our initial view is different from the default view, we have to set it before we continue.
    window.setView(view);

    if (mode_num == 1) getting_input = true;
    else Patterns::putPatternInGrid(grid, *Patterns::numToPattern[mode_num]); // Pre-defined pattern

    // TODO: fix zoom
    sf::Clock timestep_clock, key_press_clock;
    sf::Vector2i old_pos, click_pos;
    long long int gen = 0;
    float zoom = 1;
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
                    if (evnt.key.code == sf::Keyboard::Enter){
                        if (getting_input){ // Submitting input (finished getting input)
                            system("cls"); // Deleting previous printing of generations.
                            getting_input = false;
                        }
                        else { // Resetting grid (start getting input)
                            getting_input = true;
                            grid.clear();
                            gen = 0;
                        }
                    }
                    break;

                /* When left button is pressed, we save cursor position in 'click_pos' and 'old_pos'.
                They serve different purposes:
                'click_pos' - stores location of cursor when clicking.
                Because 'old_pos' keeps updating in 'MouseMoved' event, we need this variable to store the initial click location.
                'old_pos' - to change the view in 'MouseMoved' event.
                In any tiny movement, a 'MouseMoved' event is generated, and in each one we get 'new_pos', and thus - delta movement.
                We assign 'new_pos' to 'old_pos' and then start all over when there's another movement.
                So old_pos always changes in accordance to our mouse movement.
                Basically, what seems like a continuous drag - comprises a *lot* of calls to 'view.move()', each with a tiny delta

                *Note that, unlike 'KeyPressed' event, that generates multiple events when key is pressed, 'MouseButtonPressed' generates only one.
                **Using events to track mouse movements is far smoother than implementing it by yourself with 'isButtonPressed', outside the loop;
                mainly because we don't need continuous input (each movement is discrete). */
                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    click_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    old_pos = click_pos;
                    clicking = true;
                    break;

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    dragging = false;
                    clicking = false;

                    sf::Vector2i pixel_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    sf::Vector2f view_pos = (window.mapPixelToCoords(pixel_pos));
                    // When dragging, we don't want to click on the board (and changing a cell's color).
                    // So we count it as a click only if the release location (pixel_pos) is close enough to the initial click location (click_pos).
                    if (getting_input && distance(pixel_pos, click_pos) <= CELL_SIZE) {
                        handleLeftClick(window, grid, view_pos);
                    }
                    break;
                }

                case sf::Event::MouseMoved:{
                    // Process mouse movement only if left button is pressed.
                    if (!clicking) break;
                    sf::Vector2i new_pos = sf::Vector2i(evnt.mouseMove.x, evnt.mouseMove.y);
                    /* A human click would sometimes move the mouse a bit, which would generate an unwanted drag.
                    To amend this, we drag only when current cursor position (new_pos) is far enough from the initial click location (click_pos).
                    After the *first* time that distance condition is satisfied, we set 'dragging = true' so that we'd stay in "drag mode",
                    even if we get close again to the initial click location (during the entire click's duration). */
                    if (!dragging && distance(click_pos, new_pos) <= 0.5 * CELL_SIZE) break;
                    else dragging = true;

                    // We subtract 'new_pos' from 'old_pos' (not the other way around) to invert the direction and get the "drag" effect.
                    // Note we use integers, so we're safe from the flicker (for now).
                    sf::Vector2i delta_pos = old_pos - new_pos;

                    // We don't want to allow the user to drag beyond the grid, so we bound-check and put the result in 'real_delta_pos'.
                    sf::Vector2i real_delta_pos(0,0);
                    if (0 <= left_top_view.x + delta_pos.x && left_top_view.x + WINDOW_WIDTH + delta_pos.x <= GRID_WIDTH){
                        real_delta_pos.x = delta_pos.x;
                        left_top_view.x += delta_pos.x;
                    }
                    if (0 <= left_top_view.y + delta_pos.y && left_top_view.y + WINDOW_HEIGHT + delta_pos.y <= GRID_HEIGHT){
                        real_delta_pos.y = delta_pos.y;
                        left_top_view.y += delta_pos.y;
                    }
                    view.move(real_delta_pos.x, real_delta_pos.y);
                    window.setView(view);

                    old_pos = new_pos;
                    break;
                }

                // Because 'isKeyPressed' is "connected" to the actual device, it's getting input even when window is out of focus.
                // We want to accept keyboard input only when the window has focus, so we have to keep track of it using a boolean.
                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    break;

                /*case sf::Event::MouseWheelScrolled:
                    // 'zoom()' is by a factor. a number greater than 1 means zoom-out; a number smaller than 1 means zoom-in.
                    if (evnt.mouseWheelScroll.delta <= -1) // Scroll down - zoom-out
                        zoom = std::min(2.0, zoom + 0.1); // By using 'min' with '2', we set it as a lower limit.
                    else if (evnt.mouseWheelScroll.delta >= 1) // Scroll up - zoom-in
                        zoom = std::max(0.5, zoom - 0.1); // By using 'max' with '0.5', we set it as an upper limit.

                    // We use 'setSize()' here to reset our view (by setting it to the default view's size).
                    // Why? Because, as we've said, 'zoom()' is by a factor. So if we zoomed twice we'd be multiplying instead of adding.
                    // For that we reset the view and then apply the zoom on it.
                    view.setSize(window.getDefaultView().getSize()); // Reset the size
                    view.zoom(zoom);
                    window.setView(view);
                    break;*/

                case sf::Event::Resized: {
                    /* By default, when resizing, everything is squeezed/stretched to the new size.
                    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down
                    just like in a windowed video game.

                    By calling 'mapPixelToCoords(0,0)' we get *the view's* top-left corner.
                    We pass that same top-left corner with the new width and height to the 'reset()' method.

                    If we'd only change the width and height, without keeping the top-left corner the same,
                    then it would change everytime we'd resize */
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
        int delta_time = key_press_clock.restart().asMilliseconds();
        /* Note that the product 'SPEED * delta_time' can be a *float*, so moving by it can make the line not align 1:1 with the pixels,
        because their location is always in integers.
        When a line falls between 2 columns of pixels, openGL has to decide on the fly how to render it,
        and that often causes it to flicker when there is movement.
        By rounding the result, we move the view by an integer, and thus always aligning 1:1 with the pixels. */
        if (focus){
            // We're not allowing the user to move the view beyond the grid's bounds.
            int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && 0 <= left_top_view.y - delta_pos){
                left_top_view.y -= delta_pos;
                view.move(sf::Vector2f(0, -1 * delta_pos));
                window.setView(view);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && 0 <= left_top_view.x - delta_pos){
                left_top_view.x -= delta_pos;
                view.move(sf::Vector2f(-1 * delta_pos, 0));
                window.setView(view);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view.y + WINDOW_HEIGHT + delta_pos <= GRID_HEIGHT){
                left_top_view.y += delta_pos;
                view.move(sf::Vector2f(0, delta_pos));
                window.setView(view);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view.x + WINDOW_WIDTH + delta_pos <= GRID_WIDTH){
                left_top_view.x += delta_pos;
                view.move(sf::Vector2f(delta_pos, 0));
                window.setView(view);
            }
        }

        /* Using 'sleep' would lag other events like resizing or restarting the grid (since 'sleep' pauses the entire program for its duration).
        A better solution is to use 'sf::clock', which lets us check for elapsed time.
        When that elapsed time is bigger than our 'TIMESTEP' duration, we reset the clock and redraw the grid. */
        if (!getting_input && TIMESTEP <= timestep_clock.getElapsedTime()){
            timestep_clock.restart();

            gen++;
            std::cout << "Generation: " << gen << std::endl;

            updateGrid(grid);
            if (grid.empty()){
                gen = 0;
                getting_input = true; // If grid clears itself, we begin to get input again.
            }
        }

        window.clear();
        drawGrid(window, grid);
        window.display();
    }

    return 0;
}