#include "game.hpp"

Options resolveOption(std::string &input)
{
    if (input == "Window")
        return Options::Window;
    if (input == "Font")
        return Options::Font;
    if (input == "Player")
        return Options::Player;
    if (input == "Enemy")
        return Options::Enemy;
    if (input == "Bullet")
        return Options::Bullet;
    return Options::Invalid;
}

void Game::loadConfigFromFile(const std::string &filename)
{
    std::ifstream fin(filename);
    std::string type;

    while (fin >> type)
    {
        switch (resolveOption(type))
        {
        case Options::Window:
        {
            fin >> window_width >> window_height >> frame_rate >> full_screen;
            break;
        }
        case Options::Font:
        {
            std::string font_source;
            fin >> font_source;

            font = std::make_shared<sf::Font>();
            if (!font->loadFromFile(font_source))
            {
                std::cerr << "Error while trying to load font! \n";
            }
            fin >> font_size;

            int r, g, b;

            fin >> r >> g >> b;
            font_color = sf::Color(r, g, b);
            break;
        }
        case Options::Player:
        {
            // SX SY FR FG FB OR OG OB OT SR CR V
            fin >> m_playerConfig.SX >> m_playerConfig.SY >>
                m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >>
                m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >>
                m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.V;
            break;
        }
        case Options::Enemy:
        {
            //  RS1 RS2 FR FG FB OR OG OB OT SR CR SF RV1 RV2 L
            fin >> m_enemyConfig.RS1 >> m_enemyConfig.RS2 >>
                m_enemyConfig.FR >> m_enemyConfig.FG >> m_enemyConfig.FB >>
                m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >>
                m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SF >>
                m_enemyConfig.RV1 >> m_enemyConfig.RV2 >> m_enemyConfig.L;

            break;
        }
        case Options::Bullet:
        {
            // SX SY FR FG FB OR OG OB OT SR CR V L
            fin >> m_bulletConfig.SX >> m_bulletConfig.SY >>
                m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >>
                m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >>
                m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.V >> m_bulletConfig.L;
            break;
        }
        case Options::Invalid:
            break;
        default:
            break;
        }
    }
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::init()
{
    m_window.create(sf::VideoMode(window_width, window_height), "2D shooter", full_screen ? sf::Style::Fullscreen : sf::Style::Default);
    m_window.setFramerateLimit(frame_rate);
    ImGui::SFML::Init(m_window);

    spawnPlayer();
}

void Game::update()
{
    sf::Clock delta_clock;
    while (m_window.isOpen() && m_running)
    {
        ImGui::SFML::Update(m_window, delta_clock.restart());

        updateUI();
        m_entities.update();

        sUserInput();

        if (!m_paused)
        {
            sCollisions(m_entities.getEntities());
            sDeath(m_entities.getEntities());
            sEnemySpawner();
            sMovement(m_entities.getEntities());
            sLifespan(m_entities.getEntities());
            m_currentFrame++;
        }

        m_window.clear(sf::Color(0, 0, 0));

        sRender(m_entities.getEntities(), m_window);

        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

void Game::sMovement(EntityVec &entities)
{
    for (auto &e : entities)
    {
        if (e->cTransform)
        {
            if (e->cInput)
            {
                m_player->cTransform->speed = {0, 0};
                if (m_player->cInput->up)
                    m_player->cTransform->pos.y -= m_playerConfig.SY;
                if (m_player->cInput->down)
                    m_player->cTransform->pos.y += m_playerConfig.SY;
                if (m_player->cInput->left)
                    m_player->cTransform->pos.x -= m_playerConfig.SX;
                if (m_player->cInput->right)
                    m_player->cTransform->pos.x += m_playerConfig.SX;
            }
            else
            {
                e->cTransform->pos += e->cTransform->speed;
            }
        }
    }
}

std::pair<int, int> Game::isOutOfBounds(auto &e)
{
    std::pair<int, int> result = {0, 0};
    if (!e->cTransform || !e->cCollision)
        return result;

    float min_bound = 0 + e->cCollision->radius;
    float max_h_bound = window_height - e->cCollision->radius;
    float max_w_bound = window_width - e->cCollision->radius;
    if (e->cTransform->pos.x < min_bound)
        result.first = -1;
    if (e->cTransform->pos.x > max_w_bound)
        result.first = 1;
    if (e->cTransform->pos.y < min_bound)
        result.second = -1;
    if (e->cTransform->pos.y > max_h_bound)
        result.second = 1;

    return result;
}

void Game::sCollisions(EntityVec &entities)
{
    for (auto &e : entities)
    {
        if (e->cTransform && e->cShape && e->cCollision)
        {
            auto [x_ob, y_ob] = isOutOfBounds(e);
            if (e->tag() == "Player")
            {
                if (x_ob != 0 || y_ob != 0)
                {
                    if (x_ob > 0)
                        e->cTransform->pos.x = window_width - e->cCollision->radius - 1;
                    if (x_ob < 0)
                        e->cTransform->pos.x = e->cCollision->radius + 1;
                    if (y_ob > 0)
                        e->cTransform->pos.y = window_height - e->cCollision->radius - 1;
                    if (y_ob < 0)
                        e->cTransform->pos.y = e->cCollision->radius + 1;
                }
            }
            else if (e->tag() == "Enemy")
            {
                if (x_ob != 0)
                {
                    e->cTransform->speed.x *= -1;
                }
                if (y_ob != 0)
                {
                    e->cTransform->speed.y *= -1;
                }
                EntityVec &bullets = m_entities.getEntities("Bullet");
                EntityVec &specialBullets = m_entities.getEntities("SpecialBullet");
                EntityVec targets;
                targets.reserve(bullets.size() + specialBullets.size());
                targets.insert(targets.end(), bullets.begin(), bullets.end());
                targets.insert(targets.end(), specialBullets.begin(), specialBullets.end());
                targets.push_back(m_player);
                for (auto &t : targets)
                {
                    if (t->cCollision && e->cCollision->collidesWith(*t->cCollision) && e->isActive())
                    {
                        e->destroy();
                        t->destroy();

                        if (t->tag() != "Player" && t->cScore)
                        {
                            m_score += e->cScore->score;
                        } else {    
                            if (t->cScore)
                                m_score += t->cScore->score;
                        }               
                        if (m_score < 0)
                            m_score = 0;
                    }
                }
            }
            else
            {
                if (x_ob != 0 || y_ob != 0)
                {
                    e->destroy();
                }
            }
        }
    }
}

void Game::sRender(EntityVec &entities, sf::RenderWindow &win)
{
    for (auto &e : entities)
    {
        if (e->cTransform && e->cShape)
        {
            e->cShape->shape.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
            e->cShape->shape.setRotation(0.0f);
            if (e->cRotate)
            {
                e->cShape->shape.setRotation(e->cRotate->angle * 180 / M_PI);
                e->cRotate->angle += 0.01f;
            }
            win.draw(e->cShape->shape);
        }
    }
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_running = false;
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                m_running = false;
            if (event.key.code == sf::Keyboard::Space)
                setPaused(!m_paused);
            if (event.key.code == sf::Keyboard::E)
                spawnEnemy();
            if (event.key.code == sf::Keyboard::W)
                m_player->cInput->up = true;
            if (event.key.code == sf::Keyboard::S)
                m_player->cInput->down = true;
            if (event.key.code == sf::Keyboard::A)
                m_player->cInput->left = true;
            if (event.key.code == sf::Keyboard::D)
                m_player->cInput->right = true;
        }
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::W)
                m_player->cInput->up = false;
            if (event.key.code == sf::Keyboard::S)
                m_player->cInput->down = false;
            if (event.key.code == sf::Keyboard::A)
                m_player->cInput->left = false;
            if (event.key.code == sf::Keyboard::D)
                m_player->cInput->right = false;
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                if (m_currentFrame - m_lastSpecialTime > 500)
                {
                    spawnSpecialBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                }
            }
        }
    }
}

void Game::sLifespan(EntityVec &entities)
{
    for (auto &e : entities)
    {
        if (e->cLifespan)
        {
            e->cLifespan->remaining -= 1 / (float)frame_rate;
            if (e->cLifespan->remaining <= 0.0f)
                e->destroy();
        }
    }
}

void Game::sDeath(EntityVec &entities)
{
    for (auto &e : entities)
    {
        if (e->tag() == "Enemy")
        {
            if (!e->isActive())
            {
                spawnSmallEnemies(e);
            }
        }    
    }
    if (!m_player->isActive())
    {
        spawnPlayer();
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime > 150)
        spawnEnemy();
}

void Game::updateUI()
{
    ImGui::Begin("Game");
    ImGui::Text("Score: %d", m_score);
    if (!m_paused)
    {
        if (ImGui::Button("Pause"))
            setPaused(true);
    }
    else
    {
        if (ImGui::Button("Resume"))
            setPaused(false);
    }
    if (ImGui::Button("Spawn enemy"))
        spawnEnemy();
    ImGui::End();
}

void Game::spawnPlayer()
{
    auto e = m_entities.addEntity("Player");
    sf::Color fill = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
    sf::Color outline = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
    e->cShape = std::make_shared<CCShape>(m_playerConfig.SR, m_playerConfig.V, fill, outline, m_playerConfig.OT);
    Vec2 i_speed = {m_playerConfig.SX, m_playerConfig.SY};
    Vec2 i_pos = {window_width / 2.f, window_height / 2.f};
    e->cTransform = std::make_shared<CTransform>(i_pos, i_speed, 0.0f);
    e->cRotate = std::make_shared<CRotate>(0.0f);
    e->cCollision = std::make_shared<CCollision>(m_playerConfig.CR, e->cTransform);
    e->cInput = std::make_shared<CInput>();
    e->cScore = std::make_shared<CScore>(-100);
    m_player = e;
}

void Game::spawnEnemy()
{
    auto e = m_entities.addEntity("Enemy");
    size_t r_num_sides = m_enemyConfig.RV1 + (std::rand() % (1 + m_enemyConfig.RV2 - m_enemyConfig.RV1));
    int red = 0 + (std::rand() % (1 + 255 - 0));
    int green = 0 + (std::rand() % (1 + 255 - 0));
    int blue = 0 + (std::rand() % (1 + 255 - 0));
    sf::Color fill = sf::Color(red, green, blue);
    sf::Color outline = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);
    e->cShape = std::make_shared<CCShape>(m_enemyConfig.SR, r_num_sides, fill, outline, m_enemyConfig.OT);
    float r_speed_value = (float)(m_enemyConfig.RS1 + (std::rand() % (1 + m_enemyConfig.RS2 - m_enemyConfig.RS1)));
    
    // make sure enemy is not spawned on top of player and outside of screen or too close to the edge
    Vec2 i_pos = {0, 0};
    while ((i_pos.x < 50 || i_pos.x > window_width - 50 || i_pos.y < 50 || i_pos.y > window_height - 50) 
        || (i_pos.x - m_player->cTransform->pos.x < 100 && i_pos.y - m_player->cTransform->pos.y < 100))
    {
        i_pos = {float(std::rand() % window_width), float(std::rand() % window_height)};
    }
    // make random direction
    Vec2 target = {float(std::rand() % window_width), float(std::rand() % window_height)};
    Vec2 dir = target - m_player->cTransform->pos;
    float angle = std::atan2(dir.y, dir.x);
    Vec2 i_speed = {std::cos(angle) * r_speed_value, std::sin(angle) * r_speed_value};
    e->cTransform = std::make_shared<CTransform>(i_pos, i_speed, angle);
    e->cRotate = std::make_shared<CRotate>(0.0f);
    e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR, e->cTransform);
    e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    e->cScore = std::make_shared<CScore>(r_num_sides * 10);
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemy)
{
    for (size_t i = 0; i < enemy->cShape->shape.getPointCount(); i++)
    {
        float smallEnemiesRadius = m_enemyConfig.SR / 3;
        float translocationX = enemy->cShape->shape.getPoint(i).x + enemy->cTransform->pos.x - smallEnemiesRadius;
        float translocationY = enemy->cShape->shape.getPoint(i).y + enemy->cTransform->pos.y - smallEnemiesRadius;

        auto e = m_entities.addEntity("SmallEnemies");

        int red = 0 + (std::rand() % (1 + 255 - 0));
        int green = 0 + (std::rand() % (1 + 255 - 0));
        int blue = 0 + (std::rand() % (1 + 255 - 0));
        sf::Color fill = sf::Color(red, green, blue);
        sf::Color outline = sf::Color::White;
        e->cShape = std::make_shared<CCShape>(smallEnemiesRadius, enemy->cShape->shape.getPointCount(), fill, outline, m_enemyConfig.OT);

        Vec2 p_pos = {translocationX, translocationY};
        Vec2 dir = p_pos - enemy->cTransform->pos;

        float cur_angle = (360 / enemy->cShape->shape.getPointCount()) * i;

        float angle = std::atan2(dir.y, dir.x);
        Vec2 speed = {std::cos(angle) * m_enemyConfig.SF / 2, std::sin(angle) * m_enemyConfig.SF / 2};
        e->cTransform = std::make_shared<CTransform>(p_pos, speed, cur_angle);
        e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR, e->cTransform);
        e->cLifespan = std::make_shared<CLifespan>(0.2f);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> player, const Vec2 &target)
{
    auto e = m_entities.addEntity("Bullet");
    sf::Color fill = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
    sf::Color outline = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);
    e->cShape = std::make_shared<CCShape>(m_bulletConfig.SR, m_bulletConfig.V, fill, outline, m_bulletConfig.OT);
    Vec2 dir = target - m_player->cTransform->pos;
    float angle = std::atan2(dir.y, dir.x);
    Vec2 speed = {std::cos(angle) * m_bulletConfig.SX, std::sin(angle) * m_bulletConfig.SY};
    e->cTransform = std::make_shared<CTransform>(player->cTransform->pos, speed, angle);
    e->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR, e->cTransform);
    e->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialBullet(std::shared_ptr<Entity> player, const Vec2 &target)
{
    auto e = m_entities.addEntity("SpecialBullet");
    sf::Color fill = sf::Color(100, m_bulletConfig.FG, m_bulletConfig.FB);
    sf::Color outline = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);
    e->cShape = std::make_shared<CCShape>(m_bulletConfig.SR * 10, m_bulletConfig.V * 10, fill, outline, m_bulletConfig.OT);
    Vec2 dir = target - m_player->cTransform->pos;
    float angle = std::atan2(dir.y, dir.x);
    Vec2 speed = {std::cos(angle) * m_bulletConfig.SX / 2, std::sin(angle) * m_bulletConfig.SY / 2};
    e->cTransform = std::make_shared<CTransform>(player->cTransform->pos, speed, angle);
    e->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR * 10, e->cTransform);
    e->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 10);
    m_lastSpecialTime = m_currentFrame;
}
