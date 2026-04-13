#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>

class Paleontologist;
class Map;
namespace sf { class Time; }

enum class BTStatus
{
    Success,
    Failure,
    Running
};

class BTNode
{
public:
    virtual ~BTNode() = default;
    virtual BTStatus tick(Paleontologist* agent, sf::Time dt, Map& map) = 0;
};

// Composite: Sequence (AND)
class SequenceNode : public BTNode
{
public:
    void addChild(std::unique_ptr<BTNode> child) { m_children.push_back(std::move(child)); }

    BTStatus tick(Paleontologist* agent, sf::Time dt, Map& map) override;

private:
    std::vector<std::unique_ptr<BTNode>> m_children;
    std::size_t m_current = 0;
};

// Composite: Selector (OR)
class SelectorNode : public BTNode
{
public:
    void addChild(std::unique_ptr<BTNode> child) { m_children.push_back(std::move(child)); }

    BTStatus tick(Paleontologist* agent, sf::Time dt, Map& map) override;

private:
    std::vector<std::unique_ptr<BTNode>> m_children;
    std::size_t m_current = 0;
};

// Leaf: Condition
class ConditionNode : public BTNode
{
public:
    using Func = std::function<bool(Paleontologist*, Map&)>;

    ConditionNode(Func f) : m_func(std::move(f)) {}

    BTStatus tick(Paleontologist* agent, sf::Time, Map& map) override
    {
        return m_func(agent, map) ? BTStatus::Success : BTStatus::Failure;
    }

private:
    Func m_func;
};

// Leaf: Action
class ActionNode : public BTNode
{
public:
    using Func = std::function<BTStatus(Paleontologist*, sf::Time, Map&)>;

    ActionNode(Func f) : m_func(std::move(f)) {}

    BTStatus tick(Paleontologist* agent, sf::Time dt, Map& map) override
    {
        return m_func(agent, dt, map);
    }

private:
    Func m_func;
};

class BehaviorTree
{
public:
    void setRoot(std::unique_ptr<BTNode> root) { m_root = std::move(root); }
    void tick(Paleontologist* agent, sf::Time dt, Map& map);

private:
    std::unique_ptr<BTNode> m_root;
};
