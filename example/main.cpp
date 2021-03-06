/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include <cstdio>
#include <random>
#include <vector>
#include <BinPacking.h>
#include <SFML\Graphics.hpp>

using namespace std;
using namespace binpacking;

int main(int argc, char* argv[])
{
    std::mt19937 gen_rect;
    std::uniform_int_distribution<> disRect(10, 70);

    std::mt19937 gen_col;
    std::uniform_int_distribution<> chanDist(50, 255);

    std::mt19937 gen_cnt;
    std::uniform_int_distribution<> countDist(50, 200);

    gen_rect.seed(0);
    gen_cnt.seed(0);

    const int padding = 2;
    const int maxSize = 256;
    vector<Size> sizes;
    vector<RectMapping> mappings;

    BinPacker packer;
    packer.StartDynamicPacking(maxSize, padding, true);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Bin Packing");
    sf::RectangleShape rc(sf::Vector2f(100, 100));

    while (window.isOpen())
    {
        window.clear();

        gen_col.seed(0);

        auto* bins = &packer.GetBins();

        for (auto i = 0U; i < bins->size(); ++i)
        {
            int binX = (i >= 3) ? (i - 3) * (maxSize + 10) : i * (maxSize + 10);
            int binY = (i >= 3) ? (maxSize + 10) : 0;
            
            for (auto& mapping : bins->at(i).mappings)
            {
                Rect rect = mapping.mappedRect;
                rect.x += binX;
                rect.y += binY;

                rc.setFillColor(sf::Color((uint8_t)chanDist(gen_col), (uint8_t)chanDist(gen_col), (uint8_t)chanDist(gen_col)));
                rc.setOutlineColor(sf::Color::Transparent);
                rc.setOutlineThickness(0);
                rc.setPosition(sf::Vector2f((float)rect.x, (float)rect.y));
                rc.setSize(sf::Vector2f((float)rect.w, (float)rect.h));
                window.draw(rc);
            }

            rc.setFillColor(sf::Color::Transparent);
            rc.setOutlineColor(sf::Color::Red);
            rc.setOutlineThickness(-1);
            rc.setPosition(sf::Vector2f((float)binX, (float)binY));
            rc.setSize(sf::Vector2f((float)maxSize, (float)maxSize));
            window.draw(rc);
        }

        window.display();

        sf::Event event;
        window.waitEvent(event);

        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            auto size = Size(disRect(gen_rect), disRect(gen_rect));
            auto mapping = packer.PackBox(size);

            sizes.push_back(size);
            mappings.push_back(mapping);
        }
    }

    return 0;
}

//int main(int argc, char* argv[])
//{
//    std::mt19937 gen_rect;
//    std::uniform_int_distribution<> disRect(10, 70);
//    
//    std::mt19937 gen_col;
//    std::uniform_int_distribution<> chanDist(50, 255);
//    
//    std::mt19937 gen_cnt;
//    std::uniform_int_distribution<> countDist(50, 200);
//
//    const int padding = 2;
//    const int maxSize = 256;
//    vector<Size> sizes;
//
//    gen_rect.seed((unsigned)time(NULL));
//    gen_cnt.seed((unsigned)time(NULL));
//
//    for(int i = 0, sz = countDist(gen_cnt); i < sz; ++i)
//        sizes.push_back(Size(disRect(gen_rect), disRect(gen_rect)));
//    
//    BinPacker packer;
//    packer.PackBoxes(sizes, maxSize, padding);
//    const vector<Bin>* bins = &packer.GetBins();
//    
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Bin Packing");
//    sf::RectangleShape rc(sf::Vector2f(100, 100));
//    
//    while(window.isOpen())
//    {
//        window.clear();
//        
//        gen_col.seed(0);
//
//        for(auto i = 0U; i < bins->size(); ++i)
//        {
//            int binX = (i >= 3) ? (i - 3) * (maxSize + 10) : i * (maxSize + 10);
//            int binY = (i >= 3) ? (maxSize + 10) : 0;
//
//            auto& bin = bins->at(i).mappings;
//            
//            for(auto& mapping : bin)
//            {
//                Rect rect = mapping.mappedRect;
//                rect.x += binX;
//                rect.y += binY;
//
//                rc.setFillColor(sf::Color((uint8_t)chanDist(gen_col), (uint8_t)chanDist(gen_col), (uint8_t)chanDist(gen_col)));
//                rc.setOutlineColor(sf::Color::Transparent);
//                rc.setOutlineThickness(0);
//                rc.setPosition(sf::Vector2f((float)rect.x, (float)rect.y));
//                rc.setSize(sf::Vector2f((float)rect.w, (float)rect.h));
//                window.draw(rc);
//            }
//
//            rc.setFillColor(sf::Color::Transparent);
//            rc.setOutlineColor(sf::Color::Red);
//            rc.setOutlineThickness(-1);
//            rc.setPosition(sf::Vector2f((float)binX, (float)binY));
//            rc.setSize(sf::Vector2f((float)bins->at(i).size.x, (float)bins->at(i).size.y));
//            window.draw(rc);
//        }
//        
//        window.display();
//
//        sf::Event event;
//        window.waitEvent(event);
//
//        if(event.type == sf::Event::Closed)
//        {
//            window.close();
//        }
//        else if(event.type == sf::Event::KeyPressed)
//        {
//            gen_rect.seed((unsigned)time(NULL));
//            gen_cnt.seed((unsigned)time(NULL));
//
//            sizes.clear();
//            for(int i = 0, sz = countDist(gen_cnt); i < sz; ++i)
//                sizes.push_back(Size(disRect(gen_rect), disRect(gen_rect)));
//
//            packer.PackBoxes(sizes, maxSize, padding);
//            bins = &packer.GetBins();
//        }
//    }
//
//    return 0;
//}
