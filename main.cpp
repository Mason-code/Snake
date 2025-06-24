// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE

//git push origin master -- force (usefull)


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



//global
int adjustment = 0;
int highscore_value = 0;

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
void move_snake(sf::Sprite& head_sprite);
sf::Vector2f get_current_tile(sf::Transformable& item);
sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);
void add_tail(sf::CircleShape& last_tail);
void move_tail(sf::CircleShape& current_tail, bool ready_to_move, int tail_iteration);
float distance_between_two_pos(sf::Vector2f pos_one, sf::Vector2f pos_two);
bool apple_is_on_snake(sf::Sprite& the_apple_sprite);
void setup_snake(sf::Sprite& head_sprite);
sf::Color get_color();
float volume_percentage(float current_y_pos, int top_pos, int btm_pos);

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


    /*cup--Start*/
    sf::Texture cup_texture;
    if (!cup_texture.loadFromFile("cup.png")) return -1;
    sf::Sprite task_bar_cup(cup_texture);
    cup_texture.setSmooth(true);
    task_bar_cup.setPosition({ 608.5, 1 });

    sf::Vector2u cup_texture_size = cup_texture.getSize();

    // Calculate the scale factors
    float cup_scale_x = 50.f / cup_texture_size.x;
    float cup_scale_y = 70.f / cup_texture_size.y;

    task_bar_cup.setScale({ cup_scale_x, cup_scale_y });


    sf::Font aligatai_font("Aligatai_YzzL.ttf");
    sf::Text cup_text(aligatai_font);
    cup_text.setCharacterSize(70);
    cup_text.setFillColor(sf::Color::White);
    cup_text.setPosition({ 659, -9 });
    /*cup--end*/


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


    sf::Text apple_text(aligatai_font);
    apple_text.setCharacterSize(70); 
    apple_text.setFillColor(sf::Color::White);
    apple_text.setPosition({ 79, -9 });
    /*TaskBar_AppleCount--End*/
    
    /*Head Sprite*/


    sf::Texture inky_head;
    if (!inky_head.loadFromFile("inky_head.png"))  return -1;
    inky_head.setSmooth(true);

    sf::Texture pinky_head;
    if (!pinky_head.loadFromFile("pinky_head.png"))  return -1;
    pinky_head.setSmooth(true);

    sf::Texture blinky_head;
    if (!blinky_head.loadFromFile("blinky_head.png"))  return -1;
    blinky_head.setSmooth(true);

    sf::Texture clyde_head;
    if (!clyde_head.loadFromFile("clyde_head.png"))  return -1;
    clyde_head.setSmooth(true);


    sf::Sprite head_sprite(blinky_head);
    sf::FloatRect bounds = head_sprite.getLocalBounds();
    head_sprite.setOrigin(bounds.getCenter());

    sf::Vector2u head_texture_size = clyde_head.getSize();
    // Calculate the scale factors
    float head_scale_x = 60.f / head_texture_size.x;
    float head_scale_y = 60.f / head_texture_size.y;

    head_sprite.setScale(sf::Vector2f (head_scale_x, head_scale_y));

    /*Head Sprite-end*/


    /*Main Menu--Start*/


    //scoreboard
    sf::RectangleShape reset_btn({ 100.f, 40.f });
    reset_btn.setFillColor(sf::Color(87, 138, 52));
    reset_btn.setPosition(sf::Vector2f((window.getSize().x / 2) + 102 -10 , (window.getSize().y / 2) - 336));
    reset_btn.setOutlineThickness(8.f);
    reset_btn.setOutlineColor(sf::Color(74, 117, 44));

    sf::Text reset_text(aligatai_font);
    reset_text.setString("Reset");
    reset_text.setFillColor(sf::Color::Black);
    reset_text.setCharacterSize(50);
    reset_text.setPosition(sf::Vector2f((window.getSize().x / 2) + 102 -10 + 2, (window.getSize().y / 2) - 350));


    sf::RectangleShape line({ 3, 3 });
    line.setFillColor(sf::Color::Black);

    sf::Font minecraftia_font("Minecraftia_Regular.ttf");

    sf::Text title_text(minecraftia_font);
    title_text.setString("HIGH SCORES");
    title_text.setCharacterSize(33);
    title_text.setFillColor(sf::Color::White);
    title_text.setPosition(sf::Vector2f(274,145));
    title_text.setOutlineColor(sf::Color::Black);
    title_text.setOutlineThickness(2.f);

    sf::Text rank_text(minecraftia_font);
    rank_text.setCharacterSize(18);
    rank_text.setFillColor(sf::Color::White);
    rank_text.setOutlineColor(sf::Color::Black);
    rank_text.setOutlineThickness(1.f);
    rank_text.setPosition({ 240,170});


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

    //sf::Font minecraftia_font("Minecraftia_Regular.ttf");

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
    red_screen.setFillColor(sf::Color(107, 20, 26, 200));
    red_screen.setPosition({ 0, 0 });

    sf::Texture respawn_texture;
    if (!respawn_texture.loadFromFile("respawn_btn.png")) return -1; // Error loading file
    respawn_texture.setSmooth(true);
    sf::Sprite respawn_btn(respawn_texture);
    respawn_btn.setScale(sf::Vector2f( .45, .45 ));
    respawn_btn.setPosition(sf::Vector2f((window.getSize().x / 2) + -289, (window.getSize().y / 2) + 98));


    sf::Texture title_screen_texture;
    if (!title_screen_texture.loadFromFile("title_screen_btn.png")) return -1; // Error loading file
    title_screen_texture.setSmooth(true);
    sf::Sprite title_screen_btn(title_screen_texture);
    title_screen_btn.setScale(sf::Vector2f(.45, .45));
    title_screen_btn.setPosition(sf::Vector2f((window.getSize().x / 2) + -289, (window.getSize().y / 2) + 174));

    /* Game Over -- End */


    /*music/ sound stuff*/
    sf::Music background_music;
    if (!background_music.openFromFile("snake_game_audio.ogg")) return -1; // error
    background_music.setVolume(10);
    background_music.setLooping(true); 
    background_music.play();

    sf::Music apple_crunch;
    if (!apple_crunch.openFromFile("apple_crunch.ogg")) return -1; // error
    apple_crunch.setVolume(100);
    

    sf::Music button_click;
    if (!button_click.openFromFile("btn_boomp.ogg")) return -1; // error
    button_click.setVolume(10);
   


    sf::Texture volume_symbol;
    if (!volume_symbol.loadFromFile("volume_btn.png")) return -1;
    sf::Sprite volume_symbol_sprite(volume_symbol);
    volume_symbol.setSmooth(true);
    volume_symbol_sprite.setPosition({ 10, 10 });
    sf::Vector2u symbol_size = volume_symbol.getSize();
    // Calculate the scale factors
    float symbol_scale_x = 50.f / symbol_size.x;
    float symbol_scale_y = 50.f / symbol_size.y;
    volume_symbol_sprite.setScale({ symbol_scale_x, symbol_scale_y });

    sf::CircleShape symbol_bg(30);
    symbol_bg.setFillColor(sf::Color(74, 117, 44));
    symbol_bg.setPosition({10 + 25 - 30, 10 + 25 - 30});

    sf::RectangleShape slider_bg_line({ 30 , 130});
    slider_bg_line.setFillColor(sf::Color(74, 117, 44));
    slider_bg_line.setPosition({ 10 + 25 - 15, 10 + 34 });

    sf::CircleShape slider_bg_circle(15);
    slider_bg_circle.setFillColor(sf::Color(74, 117, 44));
    slider_bg_circle.setPosition({ 10 + 25 - 15 , 10 + 34 + 115 });


    sf::RectangleShape slider_line({ 10 , 130 });
    slider_line.setFillColor(sf::Color(87, 138, 52));
    slider_line.setPosition({ 10 + 25 - 5, 10 + 34 });
    slider_line.setOutlineThickness(1);
    slider_line.setOutlineColor(sf::Color::Black);

    sf::RectangleShape slider_line_support({ 10 , 1 });
    slider_line_support.setFillColor(sf::Color(87, 138, 52));
    slider_line_support.setPosition({ 10 + 25 - 5, 10 + 34 + 130});

    sf::CircleShape slider_circle(5);
    slider_circle.setFillColor(sf::Color(87, 138, 52));
    slider_circle.setPosition({ 10 + 25 - 5 , 10 + 34 + 125 });
    slider_circle.setOutlineThickness(1);
    slider_circle.setOutlineColor(sf::Color::Black);
    
    sf::CircleShape exit_volume_btn(20.f);
    exit_volume_btn.setFillColor(sf::Color::Black);
    exit_volume_btn.setPosition({ 10 + 25 + 30 + 10+ 4, 10 + 25 - 20  });
    exit_volume_btn.setOutlineColor(sf::Color::Transparent);
    exit_volume_btn.setOutlineThickness(1);



    sf::RectangleShape left_cross({ 4, 28 });
    left_cross.setPosition({ 10 + 25 + 30 + 10 + 4 + 20, 10 + 25});
    left_cross.setFillColor(sf::Color::White);
    left_cross.setOrigin({ 2,14 });
    left_cross.setRotation(sf::degrees(-45));

    sf::RectangleShape right_cross({ 4, 28 });
    right_cross.setPosition({ 10 + 25 + 30 + 10 + 4 + 20, 10 + 25 });
    right_cross.setFillColor(sf::Color::White);
    right_cross.setOrigin({2,14});
    right_cross.setRotation(sf::degrees(45.f));


    // increase radius and position
    sf::CircleShape sliding_circle(5);
    sliding_circle.setFillColor(sf::Color::Black);
    sliding_circle.setPosition({ 10 + 25 - 5 , 65}); //65
    sliding_circle.setOutlineThickness((100 / 15) + 1);
    sliding_circle.setOutlineColor(sf::Color::Transparent);

    bool interacting_with_volume = false;
    bool display_slider = false;
    /*music-end*/

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
                move_snake(head_sprite);
                window.draw(sh.shape);
                head_sprite.setPosition(sf::Vector2f(sh.shape.getPosition().x + 20, sh.shape.getPosition().y + 20));
                window.draw(head_sprite);

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
                    apple_crunch.play();
                }

                apple_text.setString(std::to_string(apples_eaten));
                window.draw(apple_text);

                window.draw(apple_sprite);

                window.draw(task_bar_apple);

                cup_text.setString(std::to_string(highscore_value));
                if(apples_eaten > highscore_value) cup_text.setString(std::to_string(apples_eaten));

                window.draw(cup_text);
                window.draw(task_bar_cup);

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
                
                for (int c = 0; c < 77; c++) { // 233
                    line.setPosition(sf::Vector2f( 446, 167 + c*3 ));
                    if (c % 2 == 0) window.draw(line);
                }
                

                /*score board*/
                window.draw(title_text);


                // get scoreboard data
                struct ScoreInfo{
                    std::string which_snake;
                    int apples;
                };
                std::vector<ScoreInfo> score_info_vec;

                std::ifstream inFile("scoreboard.txt");
                if (!inFile) {
                    std::cerr << "Error: could not open file for reading.\n";
                    return 0;
                }

                std::string x;
                int y;
                while (inFile >> x >> y) {// While there are still x and y values left in the file, read them and use them.
                    score_info_vec.push_back({ x, y });
                }

                inFile.close();
                
                // organize scoreboard 
                /*A lambda (the [] thing) is a quick way to write a tiny function.
                This lambda sorts the vector from greatest to least apples by comparing two elements at a time.*/
                std::sort(score_info_vec.begin(), score_info_vec.end(),
                    [](ScoreInfo& a, ScoreInfo& b) { // im not fully sure why this works, is std::sort iterating through score_info_vec??
                        return a.apples > b.apples;
                    }); // std::sort compares all necessary pairs to figure out full order.


                // assemble scoreboard
                for (int i = 0; i < 7; i++) {
                    if (i >= score_info_vec.size()) {
                        rank_text.setFillColor(sf::Color::White);
                    }
                    rank_text.setPosition(sf::Vector2f( 260,183 + i*34 ));
                    
                    if (i == 1) rank_text.setString(std::to_string(i + 1) + "st");
                    else if (i == 2) rank_text.setString(std::to_string(i + 1) + "nd");
                    else if (i == 3) rank_text.setString(std::to_string(i + 1) + "rd");
                    else rank_text.setString(std::to_string(i + 1) + "th");

                    window.draw(rank_text);

                    if (i < score_info_vec.size()) {

                        ScoreInfo data = score_info_vec[i];

                        if (data.which_snake == "INKY") rank_text.setFillColor(sf::Color(109, 203, 220));   
                        if (data.which_snake == "PINKY") rank_text.setFillColor(sf::Color(234, 90, 151));
                        if (data.which_snake == "BLINKY") rank_text.setFillColor(sf::Color(233, 28, 29));
                        if (data.which_snake == "CLYDE") rank_text.setFillColor(sf::Color(247, 122, 24));


                        window.draw(rank_text);
                        rank_text.setPosition(sf::Vector2f( 320,183 + i * 34 ));
                        rank_text.setString(data.which_snake);
                        window.draw(rank_text);

                        rank_text.setPosition(sf::Vector2f(470, 183 + i * 34));

                        if (data.apples< 10) rank_text.setString("000" + std::to_string(data.apples));
                        else if (data.apples < 100) rank_text.setString("00" + std::to_string(data.apples));
                        else if (data.apples < 1000) rank_text.setString("0" + std::to_string(data.apples));
                        else rank_text.setString("????");
                        window.draw(rank_text);

                    }
                }


                play_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                exit_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                player_slctn_btn_rectangle.setOutlineColor(sf::Color::Transparent);

                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window); // pixel coords
                sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos); // convert to world coords
                if (play_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    play_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {   
                        button_click.play();
                        //reset game values
                        need_turn = {};
                        x_velocity = 0;
                        y_velocity = 0;
                        snake_tail_vec = {};
                        apples_eaten = 0;
                        frame_count = 0;
                        adjustment = 0;
                        apple_sprite.setPosition({ -40 + 19.5 + (13 * 44), 70 + -25 + (8 * 44) });

                        setup_snake(head_sprite);
                        current_state = GameState::GAME_PLAY;
                        sf::sleep(sf::seconds(.3f));

                    }
                }else {
                    play_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }
                if (player_slctn_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    player_slctn_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        button_click.play();
                        current_state = GameState::PLAYER_SELECTION;
                        sf::sleep(sf::seconds(.3f));

                    }
                }else {
                    player_slctn_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }
                if  (exit_btn_rectangle.getGlobalBounds().contains(mouseWorldPos)) {
                    exit_btn_rectangle.setOutlineColor(sf::Color::Black);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        button_click.play();
                        window.close();
                    }
                } else {
                    exit_btn_rectangle.setOutlineColor(sf::Color::Transparent);
                }


                if (reset_btn.getGlobalBounds().contains(mouseWorldPos)) {
                    reset_btn.setFillColor(sf::Color::Red);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        // reset file

                        // ofs = output_file_stream
                        std::ofstream ofs("scoreboard.txt", std::ofstream::trunc);
                        ofs.close();
                    }
                }
                else {
                    reset_btn.setFillColor(sf::Color(87, 138, 52));
                }

                window.draw(reset_btn);
                window.draw(reset_text);



                if (symbol_bg.getGlobalBounds().contains(mouseWorldPos) ) {
                    window.draw(symbol_bg);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !display_slider) {
                        button_click.play();
                        display_slider = true;
                    }
                }
               
                

                if (display_slider) {
                    window.draw(slider_bg_line);
                    window.draw(slider_bg_circle);
                    window.draw(slider_circle);
                    window.draw(slider_line);
                    window.draw(slider_line_support);
                    window.draw(symbol_bg);
                    window.draw(volume_symbol_sprite);
                    window.draw(exit_volume_btn);
                    window.draw(right_cross);
                    window.draw(left_cross);

                    if (sliding_circle.getGlobalBounds().contains(mouseWorldPos) ) {
                        sliding_circle.setOutlineColor(sf::Color::Red);
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                            interacting_with_volume = true;
                            
                        }
                    }
                    else {
                        
                        sliding_circle.setOutlineColor(sf::Color::Transparent);

                    }
                    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        interacting_with_volume = false;
                        
                    }


                    if (interacting_with_volume) {
                        sliding_circle.setOutlineColor(sf::Color::Red);
                        if (mouseWorldPos.y >= 65 && mouseWorldPos.y <= 169) {
                            sliding_circle.setPosition({ 10 + 25 - 5 , mouseWorldPos.y }); 
                            float volume = volume_percentage(sliding_circle.getPosition().y, 65, 169);
                            background_music.setVolume(volume / 10);
                            apple_crunch.setVolume(volume);
                            button_click.setVolume(volume / 10);
                            sliding_circle.setOutlineThickness((volume / 15)+1);

                        }
                    }
                    window.draw(sliding_circle);
                  
                }

                window.draw(volume_symbol_sprite);
                
                if (exit_volume_btn.getGlobalBounds().contains(mouseWorldPos)) {
                    exit_volume_btn.setOutlineColor(sf::Color(74, 117, 44));
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        display_slider = false;
                    }
                }
                else {
                    exit_volume_btn.setOutlineColor(sf::Color::Transparent);
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
                        button_click.play();
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
                            button_click.play();
                            switch (i) {
                            case 0: current_color = SnakeColor::PINKY; head_sprite.setTexture(pinky_head); break;
                                case 1: current_color = SnakeColor::CLYDE; head_sprite.setTexture(clyde_head); break;
                                case 2: current_color = SnakeColor::BLINKY; head_sprite.setTexture(blinky_head); break;
                                case 3: current_color = SnakeColor::INKY; head_sprite.setTexture(inky_head); break;
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


                window.draw(title_screen_btn);
                window.draw(respawn_btn);

                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window); // pixel coords
                sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos); // convert to world coords

                if (title_screen_btn.getGlobalBounds().contains(mouseWorldPos)) {
                    title_screen_btn.setColor(sf::Color(255,255,255, 130));
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        button_click.play();
                        // save data for scoreboard 
                        std::ofstream outFile("scoreboard.txt", std::ios::app);  // open in append mode
                        if (!outFile) {
                            std::cerr << "Error: could not open file for appending.\n";
                            return 0;
                        }
                        std::string which_snake;

                        switch (current_color) {
                        case SnakeColor::BLINKY: which_snake = "BLINKY"; break;
                        case SnakeColor::PINKY: which_snake = "PINKY"; break;
                        case SnakeColor::INKY: which_snake = "INKY"; break;
                        case SnakeColor::CLYDE: which_snake = "CLYDE"; break;
                        }
                        outFile << which_snake << " " << apples_eaten << "\n";
                        outFile.close();

                        
                        current_state = GameState::MAIN_MENU;
                        sf::sleep(sf::seconds(.3f));
                    }
                }
                else {
                    title_screen_btn.setColor(sf::Color(255, 255, 255, 255));
                }

                if (respawn_btn.getGlobalBounds().contains(mouseWorldPos)) {
                    respawn_btn.setColor(sf::Color(255, 255, 255, 130));
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        button_click.play();
                        // save data for scoreboard 
                        std::ofstream outFile("scoreboard.txt", std::ios::app);  // open in append mode
                        if (!outFile) {
                            std::cerr << "Error: could not open file for appending.\n";
                            return 0;
                        }
                        std::string which_snake;

                        switch (current_color) {
                        case SnakeColor::BLINKY: which_snake = "BLINKY"; break;
                        case SnakeColor::PINKY: which_snake = "PINKY"; break;
                        case SnakeColor::INKY: which_snake = "INKY"; break;
                        case SnakeColor::CLYDE: which_snake = "CLYDE"; break;
                        }
                        outFile << which_snake << " " << apples_eaten << "\n";
                        outFile.close();

                        //reset game values
                        need_turn = {};
                        x_velocity = 0;
                        y_velocity = 0;
                        snake_tail_vec = {};
                        apples_eaten = 0;
                        frame_count = 0;
                        adjustment = 0;
                        apple_sprite.setPosition({ -40 + 19.5 + (13 * 44), 70 + -25 + (8 * 44) });
                        setup_snake(head_sprite);

                        current_state = GameState::GAME_PLAY;
                        sf::sleep(sf::seconds(.3f));
                    }
                }
                else {
                    respawn_btn.setColor(sf::Color(255, 255, 255, 255));
                }


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

void move_snake(sf::Sprite& head_sprite) {
   
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
            if (x_velocity > 0) head_sprite.setRotation(sf::degrees(0.f));
            if (x_velocity < 0) head_sprite.setRotation(sf::degrees(180.f));
            if (y_velocity > 0) head_sprite.setRotation(sf::degrees(90.f));
            if (y_velocity < 0) head_sprite.setRotation(sf::degrees(270.f));
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

void setup_snake(sf::Sprite& head_sprite) {

    head_sprite.setPosition({ -20 + 25 + 22 + (44 * (4 - 1))+20, -20 + 105 + 22 + (44 * (8 - 1))+20  });
    head_sprite.setRotation(sf::degrees(0.f));
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


    //also setup high score
    std::vector<int> score_info_vec;

    std::ifstream inFile("scoreboard.txt");
    if (!inFile) {
        std::cerr << "Error: could not open file for reading.\n";
    }
    
    std::string x;
    int y;
    while (inFile >> x >> y) {// While there are still x and y values left in the file, read them and use them.
        score_info_vec.push_back( y );
    }

    inFile.close();

    if (!score_info_vec.empty()) {
        auto highest_value = std::max_element(score_info_vec.begin(), score_info_vec.end()); // returns a pointer I think
        highscore_value = *highest_value;
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

float volume_percentage(float current_y_pos, int top_pos, int btm_pos) {
    float volume_level = (btm_pos - current_y_pos) / (btm_pos - top_pos) * 100;
    return volume_level;
}