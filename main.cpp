// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE


/* 
multi que movement strat.
*/


#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>
#include <cmath>


//global
/*apple*/
sf::Texture apple_texture;

/*snake*/
float x_velocity = 0;
float y_velocity = 0;
sf::Vector2f need_turn = {};

/*tail*/
struct TailVectorData {
    sf::CircleShape shape;
    sf::Vector2f current_velocity;
    std::vector<sf::Vector2f> turn_position;
    std::vector<sf::Vector2f> next_velocty_at_pos;
};

std::vector<TailVectorData> snake_tail_vec = {};



int apples_eaten = 0;


sf::Vector2f last_direction_change_location = sf::Vector2f(-1.f, -1.f);

//  function declarations
sf::Sprite rand_apple(sf::Sprite& the_apple_sprite);
std::vector<sf::Vector2f> move_snake(sf::CircleShape& the_snake_circle);
sf::Vector2f get_current_tile(sf::CircleShape item);
sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);
void add_tail(sf::CircleShape& head);
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity, sf::Vector2f new_position_of_turn);




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
    


    /*Snake_head--Start*/
    sf::CircleShape snake_head(20.f);
    snake_head.setFillColor(sf::Color::Blue);
    snake_head.setPosition({ -20 + 25 + 22 + (44 * (4 - 1)), -20 + 105 + 22 + (44 * (8 - 1)) });
    
    /*Snake_head--End*/


    std::vector<sf::Vector2f> tail_values;

    
    add_tail(snake_head);


    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            float ing_donuts = 0.1f;

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
                    need_turn = sf::Vector2f(0.f,-ing_donuts);
                }
                if (key->code == sf::Keyboard::Key::A && x_velocity != ing_donuts) {
                    need_turn = sf::Vector2f(-ing_donuts, 0.f);
                }
                if (key->code == sf::Keyboard::Key::S && y_velocity != -ing_donuts) {
                    need_turn = sf::Vector2f(0.f, ing_donuts);
                }
                if (key->code == sf::Keyboard::Key::D && x_velocity != -ing_donuts) {
                    need_turn = sf::Vector2f(ing_donuts, 0.f);
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



        /*window.draw(apple_sprite);
        once snake == apple{
            window.draw(rand_apple(apple_sprite));
        } */
        

        tail_values = move_snake(snake_head);
        window.draw(snake_head);

        int iteration_value = 0;

        TailVectorData& first_data = snake_tail_vec[iteration_value];
        move_first_tail(iteration_value, first_data.shape, tail_values[0], tail_values[1]);
        window.draw(first_data.shape);

        /*for (auto& tail : snake_tail_vec) {
            move_first_tail(tail.shape, tail_values[0], tail_values[1]);
            window.draw(tail.shape);
        }*/
        


        // end the current frame
        window.display();
    }
}



/*function definitions*/


sf::Sprite rand_apple(sf::Sprite& the_apple_sprite) {
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



std::vector<sf::Vector2f> move_snake(sf::CircleShape &the_snake_circle) {
    /*direction: TSA-Screening*/
    if (need_turn == sf::Vector2f(x_velocity, y_velocity)) {
        need_turn = sf::Vector2f(-69.f, -69.f);
    }

    /*direction: Boarding*/
   
    //top-left corner
    sf::Vector2f current_position = the_snake_circle.getPosition();

    sf::Vector2f current_tile = get_current_tile(the_snake_circle);

    sf::Vector2f center_position = get_center_position_of_tile(current_tile.x, current_tile.y);
    
    // epsilon tolerance
    float epsilon = 1.0f;
    bool at_center_turn_point =
        std::abs(center_position.x - (current_position.x + 20)) < epsilon &&
        std::abs(center_position.y - (current_position.y + 20)) < epsilon;



    sf::Vector2f current_velocity = sf::Vector2f(x_velocity, y_velocity);
    

    /*direction: Lift - off*/ 
    sf::Vector2f where_tail_turn = sf::Vector2f(-1.f, -1.f);
    if (at_center_turn_point && !(need_turn == sf::Vector2f(-69.f, -69.f))) {
        if (!(x_velocity == -need_turn.x) && !(y_velocity == -need_turn.y) || (x_velocity == 0.f && y_velocity == 0.f)) {
            x_velocity = need_turn.x;
            y_velocity = need_turn.y;
            where_tail_turn = the_snake_circle.getPosition();
        }
        need_turn = sf::Vector2f(-69.f, -69.f);
    }
    
    
     the_snake_circle.move(sf::Vector2f(x_velocity, y_velocity));
    
     // velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
     std::vector<sf::Vector2f> bruh = { sf::Vector2f(x_velocity, y_velocity), where_tail_turn };

     return bruh;
    }
    

    




sf::Vector2f get_current_tile(sf::CircleShape item) {

    sf::Vector2f position = item.getPosition();


    float x_tile = ceil((position.x + 20.0f - 25.0f) / 44.0f);
    float y_tile = ceil((position.y + 20.0f - 105.0f) / 44.0f);

    

    return sf::Vector2f(x_tile, y_tile);
}


sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile) {

    int x_pos = 25 + 22 + (44 * (x_tile - 1));
    int y_pos = 105 + 22 + (44 * (y_tile - 1));

    return sf::Vector2f(x_pos, y_pos);
}



//only for staarting amount
void add_tail(sf::CircleShape& head) {

    sf::CircleShape new_snake_tail(20.f);
    new_snake_tail.setFillColor(sf::Color::Red);

    sf::Vector2f head_pos = head.getPosition();
    new_snake_tail.setPosition(sf::Vector2f(-20 + 25 + 22 + (44 * ((3 - snake_tail_vec.size()) - 1)), -20 + 105 + 22 + (44 * (8 - 1))));

     



    TailVectorData data;
    data.shape = new_snake_tail;

    data.current_velocity = sf::Vector2f(0.f, 0.f);

    data.turn_position = {};

    data.next_velocty_at_pos = {};

    snake_tail_vec.push_back(data);


    apples_eaten += 1;
}

// velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity, sf::Vector2f new_position_of_turn) {


    TailVectorData& data = snake_tail_vec[iteration_int];

    if (new_position_of_turn != sf::Vector2f(-1.f, -1.f)) {
        std::cout << "record new pos " << new_position_of_turn.x << ", " << new_position_of_turn.y << " | Amount of pos: " << data.turn_position.size()+1 << " \n";
        data.turn_position.push_back(new_position_of_turn);
        data.next_velocty_at_pos.push_back(new_velocity);
        std::cout << "check2\n";

    }



    /* check if able to turn */
    // epsilon tolerance
    float epsilon = 1.0f;
    sf::Vector2f current_position = iteratee_tail.getPosition();

    // checks if tail is on the one of the same axis as the head
    bool amlost_same_position =
        (!(data.turn_position.empty()) && std::abs(data.turn_position[0].x - (current_position.x)) < epsilon && (std::abs(data.turn_position[0].x - (current_position.x)) * y_velocity != 0)) ||
        (!(data.turn_position.empty()) && std::abs(data.turn_position[0].y - (current_position.y)) < epsilon && (std::abs(data.turn_position[0].y - (current_position.y)) * x_velocity != 0));

    if (amlost_same_position) { //current_position == data.turn_position
        std::cout << "turned at " << data.turn_position[0].x << " , " << data.turn_position[0].y << " \n";
        if (data.turn_position.size() >1) {
            std::cout << "looking for " << data.turn_position[1].x << " , " << data.turn_position[1].y << " \n";

        }

        //iteratee_tail.move(sf::Vector2f(data.next_velocty_at_pos.x, data.next_velocty_at_pos.y));

        //if (y_velocity != 0) {
        //    iteratee_tail.setPosition(sf::Vector2f(data.turn_position.x, data.turn_position.y - y_velocity * 50)); // set position to tile before
        //}
        //else {
        //    iteratee_tail.setPosition(sf::Vector2f(data.turn_position.x - x_velocity * 50, data.turn_position.y)); // set position to tile before
        //}

        // position it -> tile width - radius of ball = 44 - 20 = 22
        int fuck_off_coefficient = 0;
        iteratee_tail.setPosition(sf::Vector2f(data.turn_position[0].x - fuck_off_coefficient * (x_velocity * 10), data.turn_position[0].y - fuck_off_coefficient * (y_velocity * 10)));
        data.turn_position.erase(data.turn_position.begin());

        //std::cout << data.current_velocity.x << ", " << data.current_velocity.y << " vs " << data.next_velocty_at_pos[0].x << ", " << data.next_velocty_at_pos[0].y << " \n" << data.next_velocty_at_pos.size();
        data.current_velocity = data.next_velocty_at_pos[0];
        data.next_velocty_at_pos.erase(data.next_velocty_at_pos.begin());


        iteratee_tail.move(sf::Vector2f(data.current_velocity.x, data.current_velocity.y));

        return;
    }





    //jumpstart
    if ((x_velocity != 0 || y_velocity != 0) && data.current_velocity == sf::Vector2f(0.f, 0.f)) {
        data.current_velocity = sf::Vector2f(0.1f, 0.f);
        std::cout << "check3\n";
        /*data.next_velocty_at_pos = {};
        data.turn_position = {};*/

    }



    // nothing new; just keep swimming
    if (new_position_of_turn == sf::Vector2f(-1.f, -1.f)) {
        //std::cout << "nooo\n";
        //std::cout << data.current_velocity.x <<  ", " << data.current_velocity.y << "\n";
        iteratee_tail.move(sf::Vector2f(data.current_velocity.x, data.current_velocity.y));
        //std::cout << "check1\n";
    }

}


