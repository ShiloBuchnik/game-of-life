#include <limits>
#include "patterns.h"
#include "pattern_input_screen.h"
#include "game_logic.h"

PatternInputScreen::PatternInputScreen(){
    dragging = false;
}

// Returns true iff user wants to save pattern to file.
static bool handleYNInput(){
    char YN;
    std::cout << "Would you like to save the pattern in an .rle file? [Y/N] ";
    std::cin >> YN;
    while (YN != 'Y' && YN != 'y' && YN != 'N' && YN != 'n'){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> YN;
    }

    if (YN == 'Y' || YN == 'y') return true;
    return false;
}

short int PatternInputScreen::run(){
    while (true){
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                // The program exits when user closes the window by pressing 'X'.
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    // The program exits when user presses 'esc'.
                    if (evnt.key.code == sf::Keyboard::Escape) return -1;

                    // Submitting input
                    else if (evnt.key.code == sf::Keyboard::Enter){
                        system("cls"); // Flush previous generations printing.
                        if (handleYNInput()){
                            std::cout << "Saved! file path is: " << gridToRLE(grid, born_digits, survive_digits) << std::endl;
                        }
                        return GAME_SCREEN;
                    }
                    break;

                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    initial_click_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    old_pos = initial_click_pos;
                    clicking = true;
                    break;

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    sf::Vector2i pixel_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    sf::Vector2f view_pos = (window.mapPixelToCoords(pixel_pos));
                    // When dragging, we don't want to click on the board (and changing a cell's color).
                    // So we count it as a click only if the release location (pixel_pos) is close enough to the initial click location (initial_click_pos).
                    if (!dragging) handleLeftClick(grid, view_pos);

                    dragging = false;
                    clicking = false;
                    break;
                }

                case sf::Event::MouseMoved:{
                    // Process mouse movement only if left button is pressed.
                    if (!clicking) break;
                    sf::Vector2i new_pos = sf::Vector2i(evnt.mouseMove.x, evnt.mouseMove.y);
                    /* A human click would sometimes move the mouse a bit, which would generate an unwanted drag.
                    To amend this, we drag only when current cursor position (new_pos) is far enough from the initial click location (initial_click_pos).
                    After the *first* time that distance condition is satisfied, we set 'dragging = true' so that we'd stay in "drag mode",
                    even if we get close again to the initial click location (during the entire click's duration). */
                    if (!dragging && distance(initial_click_pos, new_pos) <= 0.5 * CELL_SIZE) break;
                    else dragging = true;

                    // We subtract 'new_pos' from 'old_pos' (not the other way around) to invert the direction and get the "drag" effect.
                    // Note we use integers, so we're safe from the flicker (for now).
                    sf::Vector2i delta_pos = old_pos - new_pos;

                    // We don't want to allow the user to drag beyond the grid, so we bound-check and put the result in 'real_delta_pos'.
                    sf::Vector2i real_delta_pos(0, 0);
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

                case sf::Event::Resized:{
                    /* By default, when resizing, everything is squeezed/stretched to the new size.
                    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down,
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
        if (focus) checkForChangeViewWithKeys(delta_time);

        window.clear(DEAD_CELL_COLOR);
        // Note to self: in previous versions, drawing took almost 100%(!) of the iteration's runtime, because we drew the *entire grid* every time.
        // Modifying it to draw only what we view has reduced it to a measly 3-5ms.
        drawGrid(window, grid, left_top_view_pos, window_width, window_height);
        window.display();
    }
}