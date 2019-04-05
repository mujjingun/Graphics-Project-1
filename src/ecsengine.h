#ifndef ECSENGINE_H
#define ECSENGINE_H

#include "entity.h"
#include "entitysystem.h"

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace ou {

class ECSEngine {
    using ListIter = std::list<Entity>::iterator;

    struct ListIterHash {
        size_t operator()(ListIter it) const
        {
            return std::hash<Entity*>()(&*it);
        }
    };
    using Mapping = std::unordered_set<ListIter, ListIterHash>;
    using MappingIter = Mapping::iterator;

    std::list<Entity> m_entities;
    std::unordered_map<std::type_index, Mapping> m_mappings;
    std::multimap<int, std::unique_ptr<EntitySystem>, std::greater<>> m_systems;

    class Iterator {
        friend class ECSEngine;
        MappingIter it;
        MappingIter end;
        std::type_index prime_key = typeid(void);
        std::vector<std::type_index> other_keys;

        Iterator() = default;
        Iterator(MappingIter it, MappingIter end, std::type_index prime_key,
            std::vector<std::type_index> const& other_keys);
        void moveToNext();

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entity;
        using difference_type = std::ptrdiff_t;
        using pointer = Entity*;
        using reference = Entity&;

        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(Iterator other) const;
        bool operator!=(Iterator other) const;
        Entity& operator*() const;
        Entity* operator->() const;
    };

    class Range {
        friend class ECSEngine;

        Iterator m_begin, m_end;

        Range(ECSEngine* engine, std::vector<std::type_index>&& keys);

    public:
        Iterator begin();
        Iterator end();
    };

public:
    ECSEngine();

    void addEntity(Entity&& entity);

    void removeEntity(Iterator first, Iterator last, std::function<bool(Entity&)> pred);

    template <typename T0, typename... Ts>
    void removeEntity(std::function<bool(Entity&)> pred)
    {
        Range range = components<T0, Ts...>();
        removeEntity(range.begin(), range.end(), pred);
    }

    std::size_t countEntity() const;

    template <typename T>
    T& getOne()
    {
        Range range = components<T>();
        if (range.begin() == range.end()) {
            throw std::runtime_error("No such entity");
        }
        Entity& ent = *range.begin();
        return ent.get<T>();
    }

    template <typename T0, typename... Ts>
    Entity& getOneEnt()
    {
        Range range = components<T0, Ts...>();
        if (range.begin() == range.end()) {
            throw std::runtime_error("No such entity");
        }
        return *range.begin();
    }

    void addSystem(std::unique_ptr<EntitySystem>&& system, int priority = 0);

    void update(float deltaTime);

    template <typename T0, typename... Ts>
    Range components() { return Range(this, { typeid(T0), typeid(Ts)... }); }
};
}

#endif // ECSENGINE_H
