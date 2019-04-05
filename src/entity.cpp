#include "entity.h"
#include "entitysystem.h"

namespace ou {

Entity::Entity(std::vector<Component>&& components)
{
    for (Component& comp : components) {
        m_components.insert({ comp.type(), std::move(comp) });
    }
}

bool Entity::has(std::type_index idx) const
{
    return m_components.count(idx);
}

std::type_index Component::type() const
{
    return m_self->type();
}

const std::unordered_map<std::type_index, Component>& Entity::components() const
{
    return m_components;
}

Component::Interface::~Interface() = default;
}
