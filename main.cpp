#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>

sf::Texture apple_texture;

int main()
{
    // identifiers
    sf::Sprite rand_apple(sf::Sprite the_apple_texture);

    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "My window");

    /*Map::Start*/
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
    /*Map::End*/


    //make apple
    
    if (!apple_texture.loadFromFile("apple.png")) {
        return -1; // Error loading texture
    }
    sf::Sprite the_apple_sprite;
    the_apple_sprite.setTexture(apple_texture);

    the_apple_sprite.setSmooth(true);


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

        // map 
        // for tile (immutable) in{ : } map_tiles -> loop
        for (const sf::RectangleShape& tile : map_tiles) {
            window.draw(tile);
        }
        rand_apple();
        window.draw(rand_apple(apple_texture));

        

        // end the current frame
        window.display();
    }
}

sf::Sprite rand_apple(sf::Texture the_apple_texture) {
   /* fancy fucking random shit*/
    // Create a random device and seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range [0, 15]
    std::uniform_int_distribution<> distrib(0, 15);

    // Generate and print a random number in the range [0, 15]
    int random_number = distrib(gen);


    float random_x = 12;
       float random_y = 12;
    
    float starting_x = -40; // adjust
    float starting_y = 70; 

    float center_x = 22;
    float center_y = -20;
    
    

    sf::Vector2u textureSize = the_apple_texture.getSize();

    // Calculate the scale factors
    float scaleX = 44.f / textureSize.x;

    // Use the smaller scale factor to maintain the aspect ratio
    the_apple_sprite.setScale(scaleX, scaleX);



    the_apple_sprite.setPosition({ starting_x + center_x + (random_x * 44), starting_y + center_y + (random_y * 44) });
}

