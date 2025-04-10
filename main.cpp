// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE


/* 
chat says Use:
x = start_x + j * tile_w + tile_w / 2;
y = start_y + i * tile_h + tile_h / 2;
to place the snake exactly on a tile center.
*/


#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>


//global
sf::Texture apple_texture;

float x_velocity = 0;
float y_velocity = 0;

bool need_turn_up = false;
bool need_turn_down = false;
bool need_turn_left = false;
bool need_turn_right = false;


int main()
{
    //  function declarations
    sf::Sprite rand_apple(sf::Sprite &the_apple_sprite);
    sf::CircleShape move_snake(sf::CircleShape& the_snake_circle);
    sf::Vector2f get_current_tile(sf::CircleShape item);
    sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);

    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Snake");


    /*Position Point: testing*/
    sf::CircleShape point(5.f);
    // point.setPosition({ -point_radius + x_grid_start + half_of_tile_width + (tile_width * (which_tile - offset)), -point_radius + y_grid_start + half_of_tile_width + (tile_width * (which_tile - offset))});
    point.setPosition({ -5 + 25 + 22 +(44 * (5-1)), -5 + 105 + 22 +(44 * (5-1))});


    /*Map--Start*/
    //map border
    sf::RectangleShape map_border({ 800.f, 800.f });
    map_border.setFillColor(sf::Color(87, 138, 52));

    // map header
    sf::RectangleShape header_rectangle({ 800.f, 70.f });
    header_rectangle.setFillColor(sf::Color(74, 117, 44));
    
    // map
    std::vector<sf::RectangleShape> map_tiles = {};

    float tile_width = 44;
    float tile_height = 44;
    sf::RectangleShape tile({ tile_width, tile_height });
    tile.setPosition({ -tile_width + 25, 70 + 35 });
    for (int i = 0; i < 15; i++) {
        for (int k = 0; k < 17; k++) {
            tile.move({ tile_width, 0 });

            if ((k + i) % 2 == 0) {
                tile.setFillColor(sf::Color(162, 209, 73));
            }
            else {
                tile.setFillColor(sf::Color(170, 215, 81));
            }

            map_tiles.push_back(tile);
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
    


    /*Snake--Start*/
    sf::CircleShape snake_head(20.f);
    snake_head.setFillColor(sf::Color::Blue);
    snake_head.setPosition({ -20 + 25 + 22 + (44 * (4 - 1)), -20 + 105 + 22 + (44 * (8 - 1)) });
    
    /*Snake--End*/


    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
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



        /*window.draw(apple_sprite);
        once snake == apple{
            window.draw(rand_apple(apple_sprite));
        } */
        
        move_snake(snake_head);
        //snake_head.move(sf::Vector2f(2, 0));
        window.draw(snake_head);
       

        //snake_head.move(sf::Vector2f(0, 1));

        // end the current frame
        window.display();
    }
}



/*function definitions*/


sf::Sprite rand_apple(sf::Sprite the_apple_sprite) {
   /* fancy fucking random shit*/
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



sf::CircleShape move_snake(sf::CircleShape &the_snake_circle) {
    
    // ?????? function declarations???????

    sf::Vector2f get_current_tile(sf::CircleShape item);
    sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);


    

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && x_velocity != 1 && (!need_turn_down || !need_turn_right || !need_turn_up)) {
        need_turn_left = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && x_velocity != -1 && (!need_turn_down || !need_turn_left || !need_turn_up)) {
        need_turn_right = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && y_velocity != 1 && (!need_turn_down || !need_turn_right || !need_turn_left)) {
        need_turn_up = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && y_velocity != -1 && (!need_turn_left || !need_turn_right || !need_turn_up)) {
        need_turn_down = true;
    }

    //top-left corner
    sf::Vector2f position = the_snake_circle.getPosition();

    int x_pos = ceil(position.x) + 20;
    int y_pos = ceil(position.y) + 20;


    sf::Vector2f current_tile = get_current_tile(the_snake_circle);


    sf::Vector2f center = get_center_position_of_tile(current_tile.x, current_tile.y);

    int x_center_pos = center.x;;
    int y_center_pos = center.y;


    // 1 is true; 0 is false
    bool at_center_turn_point = x_pos == x_center_pos && y_pos == y_center_pos;

    //std::cout << at_center_turn_point << "\n";
    std::cout << "\n\n\n";
    std::cout << x_pos << " -> " <<  + x_center_pos << "\n";
    std::cout << y_pos << " -> " << + y_center_pos <<  "\n";
    std::cout << "\n\n\n";

    if (need_turn_up && at_center_turn_point) {
        y_velocity = -1;
        x_velocity = 0;
        need_turn_up = false;
    }

    if (need_turn_down && at_center_turn_point) {
        y_velocity = 1;
        x_velocity = 0;
        need_turn_down = false;
    }

    if (need_turn_left && at_center_turn_point) {
        x_velocity = -1;
        y_velocity = 0;
        need_turn_left = false;
    }

    if (need_turn_right && at_center_turn_point) {
        x_velocity = 1;
        y_velocity = 0;
        need_turn_right = false;
    }


    // this fixes the conversion error that you get when using variables inside a function (usually .move function) that uses a sf::Vector2f variable
    // don't do this: the_snake_circle.move({ x_velocity, y_velocity });
    the_snake_circle.move(sf::Vector2f(x_velocity, y_velocity));


    

    return the_snake_circle;

}


sf::Vector2f get_current_tile(sf::CircleShape item) {
    float radius = item.getRadius();
    
    sf::Vector2f position = item.getPosition();

    int x_pos = position.x + radius - 25 % 44;
    int y_pos = position.y + radius -105  % 44;

    return sf::Vector2f( x_pos, y_pos );
}


sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile) {

    int x_pos = 25 + 22 + (44 * (x_tile - 1));
    int y_pos = 105 + 22 + (44 * (y_tile - 1));

    return sf::Vector2f(x_pos, y_pos);
}