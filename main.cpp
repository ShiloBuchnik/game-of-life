#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <conio.h>
#include "game_logic.h"
#include "patterns.h"
#include "globals.h"

inline float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
    return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

// Although it's a long function, it gets called every iteration, so dear compiler - please accept my proposal.
inline void changeViewWithKeys(sf::RenderWindow& window, sf::View& view, sf::Vector2i& left_top_view_pos, int delta_time,
                               int window_width, int window_height){
    // We're not allowing the user to move the view beyond the grid's bounds.
    int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && 0 <= left_top_view_pos.y - delta_pos){
        left_top_view_pos.y -= delta_pos;
        view.move(sf::Vector2f(0, -1 * delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && 0 <= left_top_view_pos.x - delta_pos){
        left_top_view_pos.x -= delta_pos;
        view.move(sf::Vector2f(-1 * delta_pos, 0));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view_pos.y + window_height + delta_pos <= GRID_HEIGHT){
        left_top_view_pos.y += delta_pos;
        view.move(sf::Vector2f(0, delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view_pos.x + window_width + delta_pos <= GRID_WIDTH){
        left_top_view_pos.x += delta_pos;
        view.move(sf::Vector2f(delta_pos, 0));
        window.setView(view);
    }
}

int main(){
    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;
    bool getting_input = introduction(grid);

    int window_width = 1440, window_height = 810;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Game of life", sf::Style::Default);
    bool focus = true; // True iff focus is on window.
    bool first_input = getting_input; // On the first time the user inputs a pattern, we want to query whether to save it to an .rle file.
    bool clicking = false, dragging = false, wait = false;
    // TODO: maybe just ask to save pattern before opening window? It's easier and cleaer.
    // TODO: Anyway, commit what you did tomorrow so you'd have the version with asking after opening the window.

    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    sf::Vector2i left_top_view_pos(window_width * (MULTIPLE / 2), window_height * (MULTIPLE / 2));
    // 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
    // Our initial view is exactly at the middle of the grid.
    sf::View view(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));
    // Since our initial view is different from the default view, we have to set it before we continue.
    window.setView(view);

    short int timestep = 350; // By default, we "sleep" for 350ms.
    sf::Clock timestep_clock, key_press_clock;
    sf::Vector2i old_pos, click_pos;
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
                    else if (evnt.key.code == sf::Keyboard::Enter && !wait){
                        if (getting_input){ // Submitting input (finish getting input).
                            if (first_input){
                                first_input = false;
                                wait = true;
                                std::cout << "Would you like to save this pattern in an .rle file? [Y/N] ";
                            }
                            else{
                                system("cls"); // Flush introduction or previous generations.
                                getting_input = false;
                            }
                        }
                        else{ // Resetting grid (start getting input).
                            getting_input = true;
                            grid.clear();
                            gen = 0;
                        }
                    }
                    // Speed down
                    else if (evnt.key.code == sf::Keyboard::Z) timestep = std::max(25, timestep - 25);
                    // Speed up
                    else if (evnt.key.code == sf::Keyboard::X) timestep = std::min(700, timestep + 25);
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
                        handleLeftClick(grid, view_pos);
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
                    if (0 <= left_top_view_pos.x + delta_pos.x && left_top_view_pos.x + window_width + delta_pos.x <= GRID_WIDTH){
                        real_delta_pos.x = delta_pos.x;
                        left_top_view_pos.x += delta_pos.x;
                    }
                    if (0 <= left_top_view_pos.y + delta_pos.y && left_top_view_pos.y + window_height + delta_pos.y <= GRID_HEIGHT){
                        real_delta_pos.y = delta_pos.y;
                        left_top_view_pos.y += delta_pos.y;
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

                case sf::Event::Resized: {
                    /* By default, when resizing, everything is squeezed/stretched to the new size.
                    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down
                    just like in a windowed video game. We pass that corner with the new width and height to the 'reset()' method.

                    If we'd only change the width and height, without keeping the top-left corner the same,
                    then it would change everytime we'd resize.

                    We perform bound checking below, as to not show beyond the grid's bound, when resizing close to the edge. */
                    window_width = evnt.size.width;
                    window_height = evnt.size.height;
                    left_top_view_pos.x = std::min(left_top_view_pos.x, GRID_WIDTH - window_width);
                    left_top_view_pos.y = std::min(left_top_view_pos.y, GRID_HEIGHT - window_height);

                    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));
                    window.setView(view);
                    break;
                }

                /* Passing on implementing 'zoom' for now.
                case sf::Event::MouseWheelScrolled:
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
            }
        }

        // 'kbhit()' is a *non-blocking* function that returns true iff stdin is not empty.
        // In this case, we know that we have a char to get with 'getch()' (which is blocking, and that's why we check with 'kbhit()' before.
        // Note that using 'isKeyPressed' here won't work, since it doesn't take into account whether we pressed "on" the terminal or not.
        // TODO: refactor a bit, and add documentation to what you did.
        if (wait && kbhit()){
            char YN = getch();
            fflush(stdin);

            if (YN == 'Y' || YN == 'y'){
                wait = false;
                getting_input = false;
                std::string file_path = gridToRLE(grid);
                system("cls");
                std::cout << "Saved! file path is: " << file_path << std::endl;
            }
            else if (YN == 'N' || YN == 'n'){
                wait = false;
                getting_input = false;

                system("cls");
            }
            else std::cout << "\nInvalid input, please try again: ";
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
        if (focus) changeViewWithKeys(window, view, left_top_view_pos, delta_time, window_width, window_height);

        /* Using 'sleep' would lag other events like resizing or restarting the grid (since 'sleep' pauses the entire program for its duration).
        A better solution is to use 'sf::clock', which lets us check for elapsed time.
        When that elapsed time is bigger than our 'TIMESTEP' duration, we reset the clock and redraw the grid. */
        if (!getting_input && sf::milliseconds(timestep) <= timestep_clock.getElapsedTime()){
            timestep_clock.restart();

            gen++;
            std::cout << "Generation: " << gen << std::endl;

            updateGrid(grid);
            if (grid.empty()){
                gen = 0;
                getting_input = true; // If grid clears itself, we begin to get input again.
            }
        }

        window.clear(DEAD_CELL_COLOR);
        // Note to self: drawing takes almost 100%(!) of the runtime of each iteration. This is the "heaviest" operation.
        // By drawing only the viewed cells, we've reduced the draw lag tremendously, and made the grid more responsive.
        drawGrid(window, grid, left_top_view_pos, window_width, window_height);
        window.display();
    }

    return 0;
}