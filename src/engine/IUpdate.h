#pragma once

#include <vector>

#include <SFML/System.hpp>

namespace CW_E {

	class IUpdate
	{
	public:
		virtual ~IUpdate() = default;
		virtual void update(sf::Time deltaTime) = 0;
	};


	/*class UpdateHandler
	{
	public:
		UpdateHandler(size_t reserve);

		size_t subscribe(IUpdate* target);
		void unsubscribe(size_t index);
		void handleUpdates(sf::Time deltaTime);

	private:
		std::vector<IUpdate*> m_UpdateTargets;
		bool m_HasUnsubs = false;
	};


	class UpdateHandlerWrapper
	{
	public:
		UpdateHandlerWrapper(UpdateHandler* handler);

		size_t subscribe(IUpdate* target);
		void unsubscribe(size_t index);

	private:
		UpdateHandler* m_UpdateHandler;
	};*/
}