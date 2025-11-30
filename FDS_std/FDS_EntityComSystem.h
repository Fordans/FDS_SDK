/*
		Copyright(C) 2025 Fordans
						This source follows the GPL licence
						See https://www.gnu.org/licenses/gpl-3.0.html for details
*/

#pragma once

#include <vector>
#include <bitset>
#include <memory>
#include <array>
#include <algorithm>

class FDS_Component;
class FDS_Entity;

constexpr std::size_t FDS_MAX_COM = 32;
using FDS_ComBitSet = std::bitset<FDS_MAX_COM>;
using FDS_ComArray = std::array<FDS_Component*, FDS_MAX_COM>;

using FDS_ComID = std::size_t;

inline FDS_ComID getComTypeID()
{
	static FDS_ComID lastID = 0;
	return lastID++;
}

template<typename T>
inline FDS_ComID getComTypeID() noexcept
{
	static FDS_ComID typeID = getComTypeID();
	return typeID;
}

class FDS_Component
{
public:
	FDS_Entity* owner = nullptr;
public:
	virtual ~FDS_Component() {}
	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}
};

class FDS_Entity
{
public:
	void update()
	{
		for (auto& c : m_components) c->update();
	}

	void draw() 
	{
		for (auto& c : m_components) c->draw();
	}

	bool isActive() const noexcept
	{
		return m_isActive;
	}

	void destroy() noexcept
	{
		m_isActive = false;
	}

	template<typename T>
	bool hasComponent() const noexcept
	{
		return m_comBitSet[getComTypeID<T>()];
	}

	template<typename T,typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* com( new T( std::forward<TArgs>(mArgs)... ) );
		com->owner = this;

		std::unique_ptr<FDS_Component> uPtr{ com };
		m_components.emplace_back(std::move(uPtr));

		m_comArray[getComTypeID<T>()] = com;
		m_comBitSet[getComTypeID<T>()] = true;

		com->init();
		return *com;
	}

	template<typename T>
	T& getComponent() const noexcept
	{
		auto ptr( m_comArray[getComTypeID<T>()] );
		return *static_cast<T*>(ptr);
	}

private:
	bool m_isActive = true;
	std::vector<std::unique_ptr<FDS_Component>> m_components = {};
	FDS_ComArray m_comArray = {};
	FDS_ComBitSet m_comBitSet = {};
};

class FDS_EntityManager
{
public:
	void update()
	{
		for (auto& e : m_entities) e->update();
	}

	void draw()
	{
		for (auto& e : m_entities) e->draw();
	}

	void refresh()
	{
		m_entities.erase(
			std::remove_if
			(
				std::begin(m_entities),
				std::end(m_entities),
				[](const std::unique_ptr<FDS_Entity>& mEntity) {return !mEntity->isActive();}
			),
			std::end(m_entities)
		);
	}

	FDS_Entity& addEntity()
	{
		FDS_Entity* e = new FDS_Entity();
		std::unique_ptr<FDS_Entity> uPtr{ e };
		m_entities.emplace_back(std::move(uPtr));
		return *e;
	}
private:
	std::vector<std::unique_ptr<FDS_Entity>> m_entities;
};