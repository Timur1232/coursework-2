#pragma once

#include <vector>

namespace CW {

	class IUpdate
	{
	public:
		virtual ~IUpdate() = default;

		virtual void update() = 0;
	};


	class UpdateHandler
	{
	public:
		UpdateHandler(size_t reserve);

		void subscribe(IUpdate* target);
		void handleUpdates();

	private:
		std::vector<IUpdate*> m_UpdateTargets;
	};


	class UpdateHandlerWrapper
	{
	public:
		UpdateHandlerWrapper(UpdateHandler* handler);

		void subscribe(IUpdate* target);

	private:
		UpdateHandler* m_UpdateHandler;
	};
}