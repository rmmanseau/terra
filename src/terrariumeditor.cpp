#include "terrariumeditor.h"
#include "glbl_helpers.h"

TerrariumEditor::TerrariumEditor(const std::string& blueprintName)
{
    loadBlueprint(blueprintName);
    loadRenderComponents();
    int winW = blueprint.width * blueprint.tileSize;
    int winH = blueprint.height * blueprint.tileSize;
    background.init(winW, winH, blueprint.dirtTexturePath, blueprint.dirtColor);
    spriteMap.init(blueprint.tileSize, blueprint.spriteSheetPath);
    state = State::idle;

    cursor.type = (EntityType)3;
    cursor.locker.xAxisLocked = false;
    cursor.locker.yAxisLocked = false;
    cursor.size = 1;
}

void TerrariumEditor::loadBlueprint(const std::string& blueprintName)
{
    try
    {
        YAML::Node blueprintNode = glbl::assets.loadTerrariumBlueprint(blueprintName);

        blueprint.width = blueprintNode["width"].as<int>();
        blueprint.height = blueprintNode["height"].as<int>();
        blueprint.tileSize = blueprintNode["tile_size"].as<int>();
        blueprint.spriteSheetPath = blueprintNode["sprite_sheet"].as<std::string>();
        blueprint.dirtTexturePath = blueprintNode["dirt_texture"].as<std::string>();
        blueprint.dirtColor = sf::Color(blueprintNode["dirt_color"][0].as<int>(),
                                        blueprintNode["dirt_color"][1].as<int>(),
                                        blueprintNode["dirt_color"][2].as<int>());

        YAML::const_iterator itr = blueprintNode["initial_entities"].begin();
        YAML::const_iterator end = blueprintNode["initial_entities"].end();
        for (itr; itr != end; ++itr)
        {
            Vec2i position((*itr)["pos"][0].as<int>(), (*itr)["pos"][1].as<int>());
            EntityType type = glbl::constants.eType((*itr)["type"].as<std::string>());

            blueprint.entities.insert(std::make_pair(position, type));
        }
    }
    catch (const YAML::Exception &e)
    {
        std::cout << "Error with terrarium blueprint: " << blueprintName << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Starting editor with basic values." << std::endl;

        Blueprint defaultBlueprint;
        defaultBlueprint.width = 20;
        defaultBlueprint.height = 20;
        defaultBlueprint.tileSize = 24;
        defaultBlueprint.spriteSheetPath = "sprite_sheet_24.png";
        defaultBlueprint.dirtTexturePath = "dirt.png";
        defaultBlueprint.dirtColor = sf::Color(79, 49, 10);

        blueprint = defaultBlueprint;
    }
}

void TerrariumEditor::saveBlueprint(const std::string& blueprintName)
{
    YAML::Emitter out;
    out.SetIndent(4);
    out << YAML::BeginMap
        << YAML::Key << "width" << YAML::Value << blueprint.width
        << YAML::Key << "height" << YAML::Value << blueprint.height 
        << YAML::Key << "tile_size" << YAML::Value << blueprint.tileSize 
        << YAML::Key << "sprite_sheet" << YAML::Value << blueprint.spriteSheetPath 
        << YAML::Key << "dirt_texture" << YAML::Value << blueprint.dirtTexturePath
        << YAML::Key << "dirt_color" << YAML::Value
            << YAML::Flow << YAML::BeginSeq
            << (int)blueprint.dirtColor.r
            << (int)blueprint.dirtColor.g
            << (int)blueprint.dirtColor.b
            << YAML::EndSeq
        << YAML::Key << "initial_entities" << YAML::Value
            << YAML::BeginSeq;

    for (auto itr = blueprint.entities.begin();
         itr != blueprint.entities.end(); ++itr)
    {
        Vec2i pos = (itr->first);
        if (pos.x >= 0 && pos.x < blueprint.width &&
            pos.y >= 0 && pos.y < blueprint.height)
        {
            out << YAML::Flow << YAML::BeginMap
                << YAML::Key << "type"
                << YAML::Value << glbl::constants.eName(itr->second)
                << YAML::Key << "pos"
                << YAML::Value
                    << YAML::Flow << YAML::BeginSeq
                    << pos.x
                    << pos.y
                    << YAML::EndSeq
                << YAML::EndMap;
        }
    }

    out << YAML::EndSeq << YAML::EndMap;

    glbl::assets.writeTerrariumBlueprint(blueprintName, out);

    std::cout << "Terrarium saved!" << std::endl;
}

void TerrariumEditor::loadRenderComponents()
{
    try
    {
        YAML::Node entitiesNode = glbl::assets.loadEntities();

        for (YAML::const_iterator itr = entitiesNode.begin();
             itr != entitiesNode.end(); ++itr)
        {
            EntityType type = glbl::constants.eType(itr->first.as<std::string>());
            CRender render;
            render.init((itr->second)["CRender"]);
            renderComponents.insert(std::make_pair((int)type, render));
        }
    }
    catch (const YAML::Exception &e)
    {
        std::cout << "Error with entities yaml file" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Exiting editor" << std::endl;

        throw std::invalid_argument("Terrarium blueprint editor failed to load entity blueprints");
    }
}

void TerrariumEditor::addCursorToSpriteMap()
{
    Vec2i pos = cursor.position;
    CRender& render = renderComponents.at((int)cursor.type);

    for (int i = 0; i < render.layers.size(); ++i)
    {
        CRender::Layer& layer = render.layers[i];
        spriteMap.addSprite(pos, layer.texCoords, layer.color);
    }

    spriteMap.addSprite(pos, Vec2i(11, 11), sf::Color::White);
}

void TerrariumEditor::addEntitiesToSpriteMap()
{
    for (auto itr = blueprint.entities.begin();
         itr != blueprint.entities.end(); ++itr)
    {
        Vec2i pos = (itr->first);

        if (pos == cursor.position)
            continue;

        CRender& render = renderComponents.at((int)(itr->second));

        for (int i = 0; i < render.layers.size(); ++i)
        {
            CRender::Layer& layer = render.layers[i];
            spriteMap.addSprite(pos, layer.texCoords, layer.color);
        }
    }
}

void TerrariumEditor::resize(Vec2i delta, sf::RenderWindow& window)
{
    blueprint.width += delta.x;
    blueprint.height += delta.y;

    int winW = blueprint.width * blueprint.tileSize;
    int winH = blueprint.height * blueprint.tileSize;

    background.setSize(winW, winH);

    sf::FloatRect visibleArea(0, 0, winW, winH);
    window.setView(sf::View(visibleArea));
    window.setSize(sf::Vector2u(winW, winH));
}

void TerrariumEditor::updateSpriteMap()
{
    spriteMap.clearSprites();
    addEntitiesToSpriteMap();
    addCursorToSpriteMap();
}

void TerrariumEditor::drawBlueprint(sf::RenderWindow& window)
{
    window.draw(background);
    window.draw(spriteMap);
}

void TerrariumEditor::changeCursorType(int change)
{
    int type = glbl::helpers.clamp((int)cursor.type + change,
                                   3,
                                   (int)glbl::constants.eAmount() - 1);
    
    cursor.type = (EntityType)type;
}

void TerrariumEditor::changeCursorSize(int change)
{
    int size = glbl::helpers.clamp(cursor.size + change, 1, 10);
    cursor.size = size;
}

void TerrariumEditor::setState(State newState)
{
    state = newState;
}

void TerrariumEditor::placeEntityAt(Vec2i pos)
{
    blueprint.entities[pos] = cursor.type;
}

void TerrariumEditor::placeEntitiesAtCursor()
{

    int start = (-cursor.size/2) + !(cursor.size%2);
    int end = start + cursor.size;

    for (int i = start; i < end; ++i)
    {
        for (int j = start; j < end; ++j)
        {
            placeEntityAt(cursor.position + Vec2i(i, j));
        }
    }
}

void TerrariumEditor::removeEntitiesAtCursor()
{
    int start = (-cursor.size/2) + !(cursor.size%2);
    int end = start + cursor.size;

    for (int i = start; i < end; ++i)
    {
        for (int j = start; j < end; ++j)
        {
            blueprint.entities.erase(cursor.position + Vec2i(i, j));
        }
    }
}

void TerrariumEditor::lockXAxis()
{
    cursor.locker.xAxisLocked = true;
    cursor.locker.lockPosition = cursor.position;
}

void TerrariumEditor::lockYAxis()
{
    cursor.locker.yAxisLocked = true;
    cursor.locker.lockPosition = cursor.position;
}

void TerrariumEditor::unlockAxis()
{
    cursor.locker.yAxisLocked = false;
    cursor.locker.xAxisLocked = false;
    cursor.locker.lockBias = Vec2i(0, 0);
}

void TerrariumEditor::update(sf::RenderWindow& window, sf::Event& event)
{
    if (event.type == sf::Event::Closed)
    {
        window.close();
    }

    sf::Vector2i sfmlRawPos = sf::Mouse::getPosition(window);
    cursor.previousRawPosition = cursor.rawPosition;
    cursor.rawPosition = Vec2i(sfmlRawPos.x, sfmlRawPos.y);
    cursor.previousPosition = cursor.position;
    cursor.position = cursor.rawPosition / blueprint.tileSize;

    if (cursor.locker.xAxisLocked)
    {
        cursor.position.y = cursor.locker.lockPosition.y;
    }
    else if (cursor.locker.yAxisLocked)
    {
        cursor.position.x = cursor.locker.lockPosition.x;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
        && !(cursor.locker.xAxisLocked || cursor.locker.yAxisLocked))
    {
        Vec2i cursorDelta = cursor.rawPosition - cursor.previousRawPosition;
        cursor.locker.lockBias += cursorDelta;

        if (abs(cursor.locker.lockBias.x) >= 10)
        {
            lockXAxis();
        }
        else if (abs(cursor.locker.lockBias.y) >= 10)
        {
            lockYAxis();
        }
    }

    switch (state)
    {
        case State::idle:
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                    sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    setState(State::resizing);
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    placeEntitiesAtCursor();
                    setState(State::drawing);
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    removeEntitiesAtCursor();
                    setState(State::erasing);
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::S)
                {
                    std::string blueprintName;
                    glbl::helpers.getInput(blueprintName);
                    saveBlueprint(blueprintName);
                }
                else if (event.key.code == sf::Keyboard::P)
                {
                    changeCursorType(1);
                }
                else if (event.key.code == sf::Keyboard::O)
                {
                    changeCursorType(-1);
                }
                else if (event.key.code == sf::Keyboard::RBracket)
                {
                    changeCursorSize(1);
                    std::cout << "cursor size: " << cursor.size << "\n";
                }
                else if (event.key.code == sf::Keyboard::LBracket)
                {
                    changeCursorSize(-1);
                    std::cout << "cursor size: " << cursor.size << "\n";

                }
            }

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::LShift)
                {
                    unlockAxis();
                }
            }

        } break;

        case State::resizing:
        {
            if (event.type == sf::Event::MouseButtonReleased)
            {
                setState(State::idle);
            }

            if (event.type == sf::Event::MouseMoved)
            {
                Vec2i delta = cursor.position - cursor.previousPosition;
                resize(delta, window);
            }

        } break;

        case State::drawing:
        {
            if (event.type == sf::Event::MouseButtonReleased)
            {
                setState(State::idle);
            }

            if (event.type == sf::Event::MouseMoved)
            {
                placeEntitiesAtCursor();
            }

        } break;

        case State::erasing:
        {
            if (event.type == sf::Event::MouseButtonReleased)
            {
                setState(State::idle);
            }

            if (event.type == sf::Event::MouseMoved)
            {
                removeEntitiesAtCursor();
            }

        } break;
    }
    
    updateSpriteMap();
    drawBlueprint(window);

    window.display();  
}
