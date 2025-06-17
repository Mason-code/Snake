// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE


/* 
multi que movement strat. 

---no : do movement based off of tiles
*/


#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>
#include <cmath>
#include <deque>

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>


//#include <imgui.h>
//#include <imgui-SFML.h>


enum class GameState {
    MAIN_MENU,
    GAME_PLAY,
    GAME_OVER,
    PLAYER_SELECTION
};
enum class SnakeColor {
    PINKY,
    INKY,
    BLINKY,
    CLYDE
};
GameState current_state = GameState::MAIN_MENU;
SnakeColor current_color = SnakeColor::BLINKY;


struct ScoreBoard {
    std::string which_snake;
    int score;
};

std::vector<ScoreBoard> score_board_vector;


//global
int adjustment = 0;

// quick vector
std::deque<sf::Vector2f> need_turn = {};


/*apple*/
sf::Texture apple_texture;

/*snake*/
float x_velocity = 0;
float y_velocity = 0;
const int movement_offset = 50;

/*tail*/
struct TailVectorData {
    sf::CircleShape shape;
    bool active = false; // ready to move: 44 away
    std::deque<sf::Vector2f> every_position;
};

std::vector<TailVectorData> snake_tail_vec = {};


const float uni_speed = .8f; //.75
int apples_eaten = 0;

int frame_count = 0;


//  function declarations
sf::Sprite rand_apple(sf::Sprite& the_apple_sprite);
void move_snake();
sf::Vector2f get_current_tile(sf::Transformable& item);
sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);
void add_tail(sf::CircleShape& last_tail);
void move_tail(sf::CircleShape& current_tail, bool ready_to_move, int tail_iteration);
float distance_between_two_pos(sf::Vector2f pos_one, sf::Vector2f pos_two);
bool apple_is_on_snake(sf::Sprite& the_apple_sprite);
void setup_snake();
sf::Color get_color();

int main()
{

    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Snake");

    /*Map--Start*/
    //map border
    sf::RectangleShape map_border({ 800.f, 800.f });
    map_border.setFillColor(sf::Color(87, 138, 52));

    // map header
    sf::RectangleShape header_rectangle({ 800.f, 70.f });
    header_rectangle.setFillColor(sf::Color(74, 117, 44));
    
    // map -> array > vector
    std::array<sf::RectangleShape, 17*15> map_tiles;

    float tile_width = 44;
    float tile_height = 44;
    sf::RectangleShape tile({ tile_width, tile_height });
    tile.setPosition({ -tile_width + 25, 70 + 35 });
    int index = 0;
    for (int i = 0; i < 15; i++) {
        for (int k = 0; k < 17; k++) {
            tile.move({ tile_width, 0 });

            if ((k + i) % 2 == 0) {
                tile.setFillColor(sf::Color(162, 209, 73));
            }
            else {
                tile.setFillColor(sf::Color(170, 215, 81));
            }

            map_tiles[index++] = tile;
        }
        tile.move({ tile_width * -17, tile_height });
    }
    /*Map--End*/

    /*Apple--Start*/
    if (!apple_texture.loadFromFile("apple.png")) {
        return -1; 
    }
    sf::Sprite apple_sprite(apple_texture);
    apple_texture.setSmooth(true);
    apple_sprite.setPosition({ -40 + 19.5 + (13 * 44), 70 + -25 + (8 * 44) });

    sf::Vector2u texture_size = apple_texture.getSize();

    // Calculate the scale factors
    float scale_x = 50.f / texture_size.x;
    float scale_y = 70.f / texture_size.y;

    apple_sprite.setScale({ scale_x, scale_y });
    /*Apple--End*/

    /*TaskBar_AppleCount--Start*/
    sf::Texture apple_texture_two;
    if (!apple_texture_two.loadFromFile("apple.png")) {
        return -1;
    }
    sf::Sprite task_bar_apple(apple_texture_two);
    apple_texture_two.setSmooth(true);
    task_bar_apple.setPosition({ 28.5, 1});

    sf::Vector2u texture_size_two = apple_texture_two.getSize();

    // Calculate the scale factors
    float scale_x_two = 50.f / texture_size_two.x;
    float scale_y_two = 70.f / texture_size_two.y;

    task_bar_apple.setScale({ scale_x_two, scale_y_two });


    sf::Font aligatai_font("Aligatai_YzzL.ttf");
    sf::Text apple_text(aligatai_font);
    apple_text.setCharacterSize(70); 
    apple_text.setFillColor(sf::Color::White);
    apple_text.setPosition({ 79, -9 });
    /*TaskBar_AppleCount--End*/
    
    /*Main Menu--Start*/

    // menu window
    sf::RectangleShape background_rectangle({ 430.f, 600.f });
    background_rectangle.setFillColor(sf::Color::Transparent);
    background_rectangle.setPosition(sf::Vector2f((window.getSize().x / 2) - 215, (window.getSize().y / 2) - 300));
    background_rectangle.setOutlineColor(sf::Color(74, 117, 44));
    background_rectangle.setOutlineThickness(10.f);

    // Highsccore window 
    sf::RectangleShape hs_win_rectangle({ 390.f, 300.f });
    hs_win_rectangle.setFillColor(sf::Color(77, 193, 249));
    hs_win_rectangle.setPosition(sf::Vector2f((window.getSize().x / 2) - 195, background_rectangle.getPosition().y + 12));
    hs_win_rectangle.setOutlineColor(sf::Color(87, 138, 52));
    hs_win_rectangle.setOutlineThickness(5.f);

    //play button
    sf::RectangleShape play_btn_rectangle({ 390.f, 80.f });
    play_btn_rectangle.setFillColor(sf::Color(87, 138, 52));
    play_btn_rectangle.setPosition(sf::Vector2f(hs_win_rectangle.getPosition().x, hs_win_rectangle.getPosition().y + hs_win_rectangle.getSize().y + 20));
    play_btn_rectangle.setOutlineThickness(3.f);
    play_btn_rectangle.setOutlineColor(sf::Color::Transparent);

    sf::Text play_text(aligatai_font);
    play_text.setString("Start!");
    play_text.setCharacterSize(80); 
    play_text.setFillColor(sf::Color::White);
    play_text.setPosition({ (play_btn_rectangle.getPosition().x + play_btn_rectangle.getSize().x / 2.f) - 72, (play_btn_rectangle.getPosition().y + play_btn_rectangle.getSize().y / 2.f) - 60 });
    play_text.setOutlineColor(sf::Color::Black);
    play_text.setOutlineThickness(1.f);

    // Player Selection
    sf::RectangleShape player_slctn_btn_rectangle({ 390.f, 80.f });
    player_slctn_btn_rectangle.setFillColor(sf::Color(87, 138, 52));
    player_slctn_btn_rectangle.setPosition(sf::Vector2f(play_btn_rectangle.getPosition().x, play_btn_rectangle.getPosition().y + play_btn_rectangle.getSize().y + 10 ));
    player_slctn_btn_rectangle.setOutlineThickness(3.f);
    player_slctn_btn_rectangle.setOutlineColor(sf::Color::Transparent);

    sf::Text player_slctn_text(aligatai_font);
    player_slctn_text.setString("Player Selection");
    player_slctn_text.setCharacterSize(70); 
    player_slctn_text.setFillColor(sf::Color::White);
    player_slctn_text.setPosition(sf::Vector2f ((player_slctn_btn_rectangle.getPosition().x + player_slctn_btn_rectangle.getSize().x / 2.f) - 186, (player_slctn_btn_rectangle.getPosition().y + player_slctn_btn_rectangle.getSize().y / 2.f) - 55 ));
    player_slctn_text.setOutlineColor(sf::Color::Black);
    player_slctn_text.setOutlineThickness(1.f);


    // exit button
    sf::RectangleShape exit_btn_rectangle({ 390.f, 80.f });
    exit_btn_rectangle.setFillColor(sf::Color(87, 138, 52));
    exit_btn_rectangle.setPosition(sf::Vector2f(play_btn_rectangle.getPosition().x, play_btn_rectangle.getPosition().y + play_btn_rectangle.getSize().y + 100));
    exit_btn_rectangle.setOutlineThickness(3.f);
    exit_btn_rectangle.setOutlineColor(sf::Color::Transparent);

    sf::Text exit_text(aligatai_font);
    exit_text.setString("Exit");
    exit_text.setCharacterSize(80); 
    exit_text.setFillColor(sf::Color::White);
    exit_text.setPosition({ (exit_btn_rectangle.getPosition().x + exit_btn_rectangle.getSize().x / 2.f) - 58,(exit_btn_rectangle.getPosition().y + exit_btn_rectangle.getSize().y / 2.f) - 60 });
    exit_text.setOutlineColor(sf::Color::Black);
    exit_text.setOutlineThickness(1.f);


    // blur background
    sf::Texture blur_texture;
    if (!blur_texture.loadFromFile("blur.jpg")) {
        return -1;
    }
    sf::Sprite blur_sprite(blur_texture);
    // Desired size
    float target_width = 800;
    float target_height = 800;
    // Get original size of the image
    sf::Vector2u textureSize = blur_texture.getSize();
    // Calculate scale factors
    scale_x = target_width / texture_size.x;
    scale_y = target_height / texture_size.y;
    // Apply scale
    blur_sprite.setScale({ scale_x, scale_y });
    blur_sprite.setPosition(sf::Vector2f(0, 0));
    blur_sprite.setColor(sf::Color(255, 255, 255, 155)); 

    /*Main Menu--End*/


    /* PLAYER_SELECTION --Start*/
    sf::Vector2f snake_images_size = { 390.f, 200.f };

    sf::Texture inky_texture;
    if (!inky_texture.loadFromFile("INKY.png")) {
        return -1; // Error loading file
    }
    inky_texture.setSmooth(true);
    sf::Sprite inky_image(inky_texture);
    texture_size = inky_texture.getSize();
    // Calculate the scale factors
    scale_x = snake_images_size.x / texture_size.x;
    scale_y = snake_images_size.y / texture_size.y;
    inky_image.setScale({ scale_x, scale_y });
    inky_image.setPosition(sf::Vector2f((window.getSize().x / 2) - 195, background_rectangle.getPosition().y + 112));

    sf::Texture pinky_texture;
    if (!pinky_texture.loadFromFile("PINKY.png")) return -1; // Error loading file
    pinky_texture.setSmooth(true);
    sf::Sprite pinky_image(pinky_texture);
    texture_size = pinky_texture.getSize();
    // Calculate the scale factors
    scale_x = snake_images_size.x / texture_size.x;
    scale_y = snake_images_size.y / texture_size.y;
    pinky_image.setScale({ scale_x, scale_y });
    pinky_image.setPosition(sf::Vector2f((window.getSize().x / 2) - 195, background_rectangle.getPosition().y + 112));

    sf::Texture blinky_texture;
    if (!blinky_texture.loadFromFile("BLINKY.png")) return -1; // Error loading file
    blinky_texture.setSmooth(true);
    sf::Sprite blinky_image(blinky_texture);
    texture_size = blinky_texture.getSize();
    // Calculate the scale factors
    scale_x = snake_images_size.x / texture_size.x;
    scale_y = snake_images_size.y / texture_size.y;
    blinky_image.setScale({ scale_x, scale_y });
    blinky_image.setPosition(sf::Vector2f((window.getSize().x / 2) - 195, background_rectangle.getPosition().y + 112));

    sf::Texture clyde_texture;
    if (!clyde_texture.loadFromFile("CLYDE.png")) return -1; // Error loading file
    clyde_texture.setSmooth(true);
    sf::Sprite clyde_image(clyde_texture);
    texture_size = clyde_texture.getSize();
    // Calculate the scale factors
    scale_x = snake_images_size.x / texture_size.x;
    scale_y = snake_images_size.y / texture_size.y;
    clyde_image.setScale({ scale_x, scale_y });
    clyde_image.setPosition(sf::Vector2f((window.getSize().x / 2) - 195, background_rectangle.getPosition().y + 112));





    sf::RectangleShape top_left_btn({ 188.f, 122.f });
    top_left_btn.setFillColor(sf::Color(87, 138, 52));
    top_left_btn.setPosition(sf::Vector2f(hs_win_rectangle.getPosition().x, 
        hs_win_rectangle.getPosition().y + hs_win_rectangle.getSize().y + 20));
    top_left_btn.setOutlineThickness(3.f);
    top_left_btn.setOutlineColor(sf::Color::Black);

    sf::Text top_left_text(aligatai_font);
    top_left_text.setString("BLINKY");
    top_left_text.setCharacterSize(52); 
    top_left_text.setFillColor(sf::Color(233, 28, 29));
    top_left_text.setPosition( sf::Vector2f( top_left_btn.getPosition().x + top_left_btn.getSize().x / 2.f - 90, top_left_btn.getPosition().y + top_left_btn.getSize().y / 2.f - 35));
    top_left_text.setOutlineThickness(1.f);
    top_left_text.setOutlineColor(sf::Color::Black);


    sf::RectangleShape btm_left_btn({ 188.f, 122.f });
    btm_left_btn.setFillColor(sf::Color(87, 138, 52));
    btm_left_btn.setPosition(sf::Vector2f(play_btn_rectangle.getPosition().x, play_btn_rectangle.getPosition().y + play_btn_rectangle.getSize().y + 55));
    btm_left_btn.setOutlineThickness(1.f);
    btm_left_btn.setOutlineColor(sf::Color::Transparent);

    sf::Text btm_left_text(aligatai_font);
    btm_left_text.setString("PINKY");
    btm_left_text.setCharacterSize(52);
    btm_left_text.setFillColor(sf::Color(234, 90, 151));
    btm_left_text.setPosition(sf::Vector2f(btm_left_btn.getPosition().x + btm_left_btn.getSize().x / 2.f - 77, btm_left_btn.getPosition().y + btm_left_btn.getSize().y / 2.f -35));
    btm_left_text.setOutlineThickness(1.f);
    btm_left_text.setOutlineColor(sf::Color::Transparent);

    sf::RectangleShape top_right_btn({ 188.f, 122.f });
    top_right_btn.setFillColor(sf::Color(87, 138, 52));
    top_right_btn.setPosition(sf::Vector2f(hs_win_rectangle.getPosition().x + 188 + 14, hs_win_rectangle.getPosition().y + hs_win_rectangle.getSize().y + 20));
    top_right_btn.setOutlineThickness(1.f);
    top_right_btn.setOutlineColor(sf::Color::Transparent);

    sf::Text top_right_text(aligatai_font);
    top_right_text.setString("INKY");
    top_right_text.setCharacterSize(52);
    top_right_text.setFillColor(sf::Color(109, 203, 220));
    top_right_text.setPosition(sf::Vector2f(top_right_btn.getPosition().x + top_right_btn.getSize().x / 2.f - 62, top_right_btn.getPosition().y + top_right_btn.getSize().y / 2.f - 35));
    top_right_text.setOutlineThickness(1.f);
    top_right_text.setOutlineColor(sf::Color::Transparent);

    sf::RectangleShape btm_right_btn({ 188.f, 122.f });
    btm_right_btn.setFillColor(sf::Color(87, 138, 52));
    btm_right_btn.setPosition(sf::Vector2f(play_btn_rectangle.getPosition().x + 188 + 14, play_btn_rectangle.getPosition().y + play_btn_rectangle.getSize().y + 55));
    btm_right_btn.setOutlineThickness(1.f);
    btm_right_btn.setOutlineColor(sf::Color::Transparent);


    sf::Text btm_right_text(aligatai_font);
    btm_right_text.setString("CLYDE");
    btm_right_text.setCharacterSize(52);
    btm_right_text.setFillColor(sf::Color(247, 122, 24));
    btm_right_text.setPosition(sf::Vector2f( btm_right_btn.getPosition().x + btm_right_btn.getSize().x / 2.f - 77, btm_right_btn.getPosition().y + btm_right_btn.getSize().y / 2.f - 35));
    btm_right_text.setOutlineThickness(1.f);
    btm_right_text.setOutlineColor(sf::Color::Transparent);



    sf::RectangleShape back_btn({ 40.f, 90.f });
    back_btn.setFillColor(sf::Color::Transparent);
    back_btn.setPosition(sf::Vector2f((window.getSize().x / 2) - 215 - 10 - 5 - 30 - 20 + 28- 15-1-6+1, (window.getSize().y / 2) + 300 + 5 - 3 - 50 - 50+10));
    back_btn.setOutlineThickness(8.f);
    back_btn.setOutlineColor(sf::Color(74, 117, 44));

    sf::Text back_text(aligatai_font);
    back_text.setString("Back");
    back_text.setFillColor(sf::Color::Black);
    back_text.setCharacterSize(50);
    back_text.setPosition(sf::Vector2f((window.getSize().x / 2) - 215 - 10 - 5 - 30-24-5, (window.getSize().y / 2) + 300+5 - 5));
    back_text.setRotation(sf::degrees(270.f));

    /* PLAYER_SELECTION -- End */



    /* Game Over */

    sf::Font minecraftia_font("Minecraftia_Regular.ttf");

    sf::Text you_died_text(minecraftia_font);
    you_died_text.setCharacterSize(71);
    you_died_text.setFillColor(sf::Color(242, 241, 244));
    you_died_text.setPosition(sf::Vector2f((window.getSize().x / 2) - 186, (window.getSize().y / 2) -170));
    you_died_text.setString("You Died!");
    sf::Text you_died_text_shadow(minecraftia_font);
    you_died_text_shadow.setCharacterSize(71);
    you_died_text_shadow.setFillColor(sf::Color(110, 105, 110));
    you_died_text_shadow.setPosition(sf::Vector2f((window.getSize().x / 2) - 186 + 9, (window.getSize().y / 2) - 170 + 9));
    you_died_text_shadow.setString("You Died!");

    sf::Text score_text(minecraftia_font);
    score_text.setCharacterSize(37);
    score_text.setFillColor(sf::Color(242, 241, 244));
    score_text.setPosition(sf::Vector2f((window.getSize().x / 2) + -93, (window.getSize().y / 2) -22));
    sf::Text score_text_shadow(minecraftia_font);
    score_text_shadow.setCharacterSize(37);
    score_text_shadow.setFillColor(sf::Color(110, 105, 110));
    score_text_shadow.setPosition(sf::Vector2f((window.getSize().x / 2) + -93 + 5, (window.getSize().y / 2) - 22 + 5));


    sf::RectangleShape red_screen({ 800.f, 800.f });
    red_screen.setFillColor(sf::Color(255, 0, 50, 130));
    red_screen.setPosition({ 0, 0 });

    sf::Texture respawn_texture;
    if (!respawn_texture.loadFromFile("respawn_btn.png")) return -1; // Error loading file
    respawn_texture.setSmooth(true);
    sf::Sprite respawn_btn(respawn_texture);
    respawn_btn.setScale(sf::Vector2f( .45, .45 ));
    respawn_btn.setPosition(sf::Vector2f((window.getSize().x / 2) + -289, (window.getSize().y / 2) + 56));


    sf::Texture title_screen_texture;
    if (!title_screen_texture.loadFromFile("title_screen_btn.png")) return -1; // Error loading file
    title_screen_texture.setSmooth(true);
    sf::Sprite title_screen_btn(title_screen_texture);
    title_screen_btn.setScale(sf::Vector2f(.45, .45));
    title_screen_btn.setPosition(sf::Vector2f((window.getSize().x / 2) + -289, (window.getSize().y / 2) + 84));

    /* Game Over -- End */



    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {

            float ing_donuts = uni_speed;

            /*

            If you have a regular object, you use the dot '.' operator:
            sf::Event event;
            event.type; // access the 'type' member of the event object

            If you have a pointer to an object, you use the arrow '->' operator:
            sf::Event* eventPtr = &event;
            eventPtr->type; // access the 'type' member through the pointer

            */

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) { //Check if the current event is a KeyPressed event. | If it is, get a pointer to the KeyPressed data and store it in 'key'.

                if (key->code == sf::Keyboard::Key::W && y_velocity != ing_donuts) {
                    need_turn.push_back(sf::Vector2f(0.f, -ing_donuts));

                }
                if (key->code == sf::Keyboard::Key::A && x_velocity != ing_donuts) {
                    need_turn.push_back(sf::Vector2f(-ing_donuts, 0.f));

                }
                if (key->code == sf::Keyboard::Key::S && y_velocity != -ing_donuts) {
                    need_turn.push_back(sf::Vector2f(0.f, ing_donuts));

                }
                if (key->code == sf::Keyboard::Key::D && x_velocity != -ing_donuts) {
                    need_turn.push_back(sf::Vector2f(ing_donuts, 0.f));
                }
            }

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        

        // draw everything here...
        window.draw(map_border);
        window.draw(header_rectangle);

        // draw map 
        // for tile (immutable) in{ : } map_tiles -> loop
        for (const sf::RectangleShape& tile : map_tiles) {
            window.draw(tile);
        }


        switch (current_state) {
            case GameState::GAME_PLAY: {
                
                TailVectorData& sh = snake_tail_vec[0];

                int iteration_value = 0;
                int ultima_value = snake_tail_vec.size() - 1;
                for (auto& tail : snake_tail_vec) {
                    if (iteration_value == 0) {
                        tail.active = true;
                        iteration_value++;
                        continue;
                    }
                    if (!tail.active && iteration_value < ultima_value) ultima_value = iteration_value;
                    move_tail(tail.shape, tail.active, iteration_value);
                    window.draw(tail.shape);
                    iteration_value++;

                    if (tail.active && distance_between_two_pos(tail.shape.getPosition(), sh.shape.getPosition()) < 15) current_state = GameState::GAME_OVER;
                    
                    

                }
                move_snake();
                window.draw(sh.shape);
                sf::Vector2f snake_pos = sh.shape.getPosition();
                if (!(snake_pos.x > 25 && snake_pos.x < 765) || !(snake_pos.y > 105 && snake_pos.y < 763)) current_state = GameState::GAME_OVER; 
                


                // do I move last tail?
                TailVectorData& ultima = snake_tail_vec[ultima_value];
                if (!ultima.active && snake_tail_vec.size() > 1) {
                    TailVectorData& penultima = snake_tail_vec[ultima_value - 1];
                    float distance = distance_between_two_pos(penultima.shape.getPosition(), ultima.shape.getPosition()); // math oooooo
                    if (distance >= 44) {
                        ultima.active = true;
                    }
                }


                sh = snake_tail_vec[0];
                // Collision detection
                sf::Vector2f apple_location = get_current_tile(apple_sprite);
                sf::Vector2f snake_location = get_current_tile(sh.shape);

                if (apple_location == snake_location) {
                    TailVectorData& data = snake_tail_vec.back();
                    add_tail(data.shape);

                    rand_apple(apple_sprite);
                    while (apple_is_on_snake(apple_sprite)) {
                        // if infinite loop game over!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        rand_apple(apple_sprite);
                    }


                    apples_eaten++;


                }


                apple_text.setString(std::to_string(apples_eaten));
                window.draw(apple_text);


                window.draw(apple_sprite);



                window.draw(task_bar_apple);


                break;


            }
            case GameState::MAIN_MENU: {
                window.draw(blur_sprite);
                window.draw(background_rectangle);
                window.draw(exit_btn_rectangle);
                window.draw(play_btn_rectangle);
                window.draw(hs_win_rectangle);
                window.draw(player_slctn_btn_rectangle);
                window.draw(play_text);
                window.draw(player_slctn_text);
                window.draw(exit_text);

                play_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                exit_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                player_slctn_btn_rectangle.setOutlineColor(sf::Color::Transparent);

                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window); // pixel coords
                sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos); // convert to world coords
                if (play_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    play_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {                        
                        //reset game values
                        need_turn = {};
                        x_velocity = 0;
                        y_velocity = 0;
                        snake_tail_vec = {};
                        apples_eaten = 0;
                        frame_count = 0;
                        adjustment = 0;
                        apple_sprite.setPosition({ -40 + 19.5 + (13 * 44), 70 + -25 + (8 * 44) });

                        setup_snake();
                        //set color
                        current_state = GameState::GAME_PLAY;
                    }
                }
                else {
                    play_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }
                if (player_slctn_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    player_slctn_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        current_state = GameState::PLAYER_SELECTION;
                    }
                }
                else {
                    player_slctn_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }
                if  (exit_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    exit_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        window.close();
                    }
                }
                else {
                    exit_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }

                
                break;
            }
            case GameState::PLAYER_SELECTION: {
                window.draw(blur_sprite);
                window.draw(background_rectangle);
                window.draw(hs_win_rectangle);
                window.draw(btm_left_btn);
                window.draw(top_left_btn);
                window.draw(top_right_btn);
                window.draw(btm_right_btn);
                window.draw(btm_right_text);
                window.draw(top_right_text);
                window.draw(btm_left_text);
                window.draw(top_left_text);
                window.draw(back_btn);
                window.draw(back_text);

                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window); // pixel coords
                sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos); // convert to world coords

                if (back_btn.getGlobalBounds().contains(mouseWorldPos)) {
                    back_btn.setFillColor(sf::Color(87, 138, 52));
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        current_state = GameState::MAIN_MENU;
                    }
                }
                else {
                    back_btn.setFillColor(sf::Color::Transparent);
                }
                
                // I pointerified it so the iteration actual affects the original objects
                std::array<sf::Text*, 4> texts = { &btm_left_text, &btm_right_text, &top_left_text, &top_right_text };
                std::array<sf::RectangleShape*, 4> buttons = { &btm_left_btn, &btm_right_btn, &top_left_btn, &top_right_btn };


                //more pointerfying: If you have a pointer to an object, you use '->' to access its members (like functions or variables), instead of '.' which is used for regular non-pointer objects. A '->' is a member access operator 
                // imagine if all the '->' were replaced with the '.' , its not that complicated - just another operator that does almost the same thing
                for (int i = 0; i < 4; i++) {
                    if (buttons[i]->getGlobalBounds().contains(mouseWorldPos) && texts[i]->getOutlineColor() != sf::Color::Black) {
                        buttons[i]->setOutlineColor(sf::Color::Black);
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                            switch (i) {
                                case 0: current_color = SnakeColor::PINKY; break;
                                case 1: current_color = SnakeColor::CLYDE; break;
                                case 2: current_color = SnakeColor::BLINKY; break;
                                case 3: current_color = SnakeColor::INKY; break;
                            }

                            for (int b = 0; b < 4; b++) {
                                if (i == b) continue;
                                texts[b]->setOutlineColor(sf::Color::Transparent);
                                buttons[b]->setOutlineColor(sf::Color::Transparent);
                                buttons[b]->setOutlineThickness(1.f);
                            }

                            texts[i]->setOutlineColor(sf::Color::Black);
                            buttons[i]->setOutlineColor(sf::Color::Black);
                            buttons[i]->setOutlineThickness(3.f);
                            
                        }
                    }
                    else if (texts[i]->getOutlineColor() != sf::Color::Black) {
                        buttons[i]->setOutlineColor(sf::Color::Transparent);
                    }
                }

                switch (current_color) {
                    case SnakeColor::BLINKY: window.draw(blinky_image); break;
                    case SnakeColor::PINKY: window.draw(pinky_image); break;
                    case SnakeColor::INKY: window.draw(inky_image); break;
                    case SnakeColor::CLYDE: window.draw(clyde_image); break;
                }
                break;
            }
            case GameState::GAME_OVER: {
                window.draw(red_screen);
                
                window.draw(you_died_text_shadow);
                window.draw(you_died_text);
                
                score_text.setString("Score: " + std::to_string(apples_eaten));
                score_text_shadow.setString("Score: " + std::to_string(apples_eaten));
                window.draw(score_text_shadow);
                window.draw(score_text);



                // save data for scoreboard

                window.draw(title_screen_btn);
                window.draw(respawn_btn);


            }
        }
        

        // end the current frame
        window.display();
        frame_count++;
    }
}



/*function definitions*/
bool apple_is_on_snake(sf::Sprite& the_apple_sprite) {
    TailVectorData& data = snake_tail_vec[0];
    for (auto& pos : data.every_position) { // sorry if this is disgusting
        float distance = distance_between_two_pos(pos, the_apple_sprite.getPosition());
        if (distance < 22) {
            return true;
        }
    }

    return false;
}

sf::Sprite rand_apple(sf::Sprite& the_apple_sprite) {
   /* fancy  random */
    // Create a random device and seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range [0, 15]
    std::uniform_int_distribution<> distrib_y(1, 15);

    // Generate and print a random number in the range [0, 15]
    float random_y = distrib_y(gen);

    // rando y
    std::uniform_int_distribution<> distrib_x(1, 17); 
    float random_x = distrib_x(gen);



    float starting_x = -40; 
    float starting_y = 70; 

    float center_x = 19.5;
    float center_y = -25;
    
    the_apple_sprite.setPosition({ starting_x + center_x + (random_x * 44), starting_y + center_y + (random_y * 44) });
    
    return the_apple_sprite;
}

void move_snake() {
   
    TailVectorData& data = snake_tail_vec[0];
    sf::CircleShape& the_snake_circle = data.shape;

    /*direction: TSA-Screening*/
    if (!need_turn.empty() && need_turn[0] == sf::Vector2f(x_velocity, y_velocity)) {
        need_turn.pop_front();
    }

    /*direction: Boarding*/
   
    //top-left corner
    sf::Vector2f current_position = the_snake_circle.getPosition();

    sf::Vector2f current_tile = get_current_tile(the_snake_circle);

    sf::Vector2f center_position = get_center_position_of_tile(current_tile.x, current_tile.y);
    
    // epsilon tolerance
    float epsilon = std::max(0.5f, std::abs(uni_speed) / 2.f);// chat said this was better -> it is
    bool at_center_turn_point =
        std::abs(center_position.x - (current_position.x + 20)) < epsilon &&
        std::abs(center_position.y - (current_position.y + 20)) < epsilon;



    sf::Vector2f current_velocity = sf::Vector2f(x_velocity, y_velocity);
    

    /*direction: Lift - off*/ 
    sf::Vector2f where_tail_turn = sf::Vector2f(-1.f, -1.f);
    if (at_center_turn_point && !need_turn.empty()) {
        if (!(x_velocity == -need_turn[0].x) && !(y_velocity == -need_turn[0].y) || (x_velocity == 0.f && y_velocity == 0.f)) {
            x_velocity = need_turn[0].x;
            y_velocity = need_turn[0].y;
            where_tail_turn = the_snake_circle.getPosition();
        }
        need_turn.pop_front();
    }
    
    
     the_snake_circle.move(sf::Vector2f(x_velocity, y_velocity));
    
     data.every_position.push_back(the_snake_circle.getPosition());

     
     if (data.every_position.size() > (25*17* movement_offset)+2) {
         data.every_position.pop_front();
         adjustment++;
     }

    }

sf::Vector2f get_current_tile(sf::Transformable& item) { // I can pass any object derived from sf::Transformable (like sf::Sprite, sf::CircleShape, sf::RectangleShape, etc.)

    sf::Vector2f position = item.getPosition();


    float x_tile = ceil((position.x + 20.0f - 25.0f) / 44.0f);
    float y_tile = ceil((position.y + 20.0f - 105.0f) / 44.0f);

    

    return sf::Vector2f(x_tile, y_tile);
}

sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile) { // I think this is incorrect

    int x_pos = 25 + 22 + (44 * (x_tile - 1));
    int y_pos = 105 + 22 + (44 * (y_tile - 1));

    return sf::Vector2f(x_pos, y_pos);
}

void add_tail(sf::CircleShape& last_tail) {

    sf::CircleShape new_snake_tail(20.f);
    new_snake_tail.setFillColor(get_color());
    new_snake_tail.setOutlineColor(sf::Color::Black);
    new_snake_tail.setOutlineThickness(1.f);

    sf::Vector2f tail_pos = last_tail.getPosition();
    new_snake_tail.setPosition(tail_pos);

    TailVectorData data;
    data.shape = new_snake_tail;
    data.active = false;

    snake_tail_vec.push_back(data);
}

void move_tail(sf::CircleShape& current_tail, bool ready_to_move, int tail_iteration) {

    TailVectorData& positions = snake_tail_vec[0];

    if (!ready_to_move){ 
        return;
    }
    int frame = frame_count - (movement_offset * tail_iteration) - adjustment; // movement_offset is just for distancing bewteen tails
    if (frame > positions.every_position.size()) frame = positions.every_position.size()-1;

    current_tail.setPosition(positions.every_position[frame]);
}

float distance_between_two_pos(sf::Vector2f pos_one, sf::Vector2f pos_two) {
    // there is a formula for this
    float dx = pos_two.x - pos_one.x;
    float dy = pos_two.y - pos_one.y;
    float distance = std::sqrt(dx * dx + dy * dy); 
    return distance;
}

void setup_snake() {
    /*Snake_head--Start*/
    sf::CircleShape snake_head(20.f);
    snake_head.setFillColor(get_color());
    snake_head.setPosition({ -20 + 25 + 22 + (44 * (4 - 1)), -20 + 105 + 22 + (44 * (8 - 1)) });
    snake_head.setOutlineColor(sf::Color::Black);
    snake_head.setOutlineThickness(1.f);

    TailVectorData data;
    data.shape = snake_head;
    data.active = true;
    snake_tail_vec.push_back(data);
    /*Snake_head--End*/

    //change amount for fun
    for (int i = 0; i < 2;i++) {
        add_tail(data.shape);
    }
}

sf::Color get_color() {
    switch (current_color) {
        case SnakeColor::BLINKY: return sf::Color(233, 28, 29);
        case SnakeColor::PINKY: return sf::Color(234, 90, 151);
        case SnakeColor::INKY: return sf::Color(109, 203, 220);
        case SnakeColor::CLYDE: return sf::Color(247, 122, 24);
    }
    
}