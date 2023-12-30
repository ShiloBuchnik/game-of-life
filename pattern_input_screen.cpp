#include "screens.h"

void PatternInputScreen::handleLeftClick(const sf::Vector2i& pixel_pos) const{
    /* Regarding 'pixel_pos' and 'view_pos':
    Even when changing the view, *the objects themselves always remain in the same place in the "world"*.
    It's like a camera in a video game - it changes our perspective, but doesn't change the world.
    For example, if we click on a certain pixel, move the view (without moving the mouse), and click again,
    *SFML would register that as a click on that exact same pixel*.
    So, essentially, we have the location in the real world and the location in the current view.

    What we do below is transforming mouse coordinates (which are always the literal pixels on the screen),
    to the "relative" or "view" coordinates, which compensate for the fact that our view is moved from the real world objects.
    It's easier to understand with an example:
    If our cursor is on cell (1,1), and we move the view by pressing "D" (right) and "S" (down), and then click;
    We need to a way to "tell SFML" to click on cell (2,2), and not cell (1,1) (which is off-screen now).
    So we use 'mapPixelToCoords()', which takes a literal position on the window, fixes it according to the view, and returns it. */

    sf::Vector2f view_pos = window.mapPixelToCoords(pixel_pos);
    sf::Vector2i view_pos_integer = static_cast<sf::Vector2i>(view_pos);

    // We only register clicks inside the grid. Note that we use the grid's width and height, and not the window's, as it contains the window.
    if (0 <= view_pos_integer.x && view_pos_integer.x < grid_width && 0 <= view_pos_integer.y && view_pos_integer.y < grid_height){
        // Converting raw coordinate to cell location in the grid. This is how we get from an exact coordinate to the cell it's in.
        view_pos_integer.x /= CELL_SIZE;
        view_pos_integer.y /= CELL_SIZE;

        // We allow selecting and deselecting cells
        if (grid.count(view_pos_integer)) grid.erase(view_pos_integer);
        else grid.insert(view_pos_integer);
    }
}

short int PatternInputScreen::run(){
    bool clicking = false, dragging = false;
    sf::Vector2i old_pos, initial_click_pos;
    sf::Clock key_press_clock;

    while (true){
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                // The program exits when user closes the window by pressing 'X'.
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        grid.clear();
                        return PATTERN_MENU_SCREEN;
                    }
                    // Submitting input
                    else if (evnt.key.code == sf::Keyboard::Enter) return SAVE_GRID_SCREEN;
                    break;

                /* When left button is pressed, we save cursor position in 'initial_click_pos' and 'old_pos'.
                They serve different purposes:
                'initial_click_pos' - stores location of cursor when clicking.
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

                    initial_click_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    old_pos = initial_click_pos;
                    clicking = true;
                    break;

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    sf::Vector2i pixel_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    // When dragging, we don't want to click on the board (and changing a cell's color).
                    // So we count it as a click only if the release location (pixel_pos) is close enough to the initial click location (initial_click_pos).
                    if (!dragging) handleLeftClick(pixel_pos);

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

                    handleDrag(old_pos, new_pos);
                    break;
                }

                case sf::Event::Resized:
                    resize(evnt);
                    break;

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
        // Because 'isKeyPressed' is "connected" to the actual device, it's getting input even when window is out of focus.
        // We want to accept keyboard input only when the window has focus, so we check for it.
        if (window.hasFocus()) checkForChangeViewWithKeys(delta_time);

        window.clear(dead_cell_color);
        // Note to self: in previous versions, drawing took almost 100%(!) of the iteration's runtime, because we drew the *entire grid* every time.
        // Modifying it to draw only what we view has reduced it to a measly 3-5ms.
        drawGrid();
        window.display();
    }
}