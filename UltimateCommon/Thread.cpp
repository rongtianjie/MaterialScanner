#include "Thread.h"
#include <time.h>



Thread::Thread() 
{
	execThread = new std::thread(std::bind(functionExec, std::placeholders::_1, this));
}

Thread::~Thread()
{
	stop();
	wait();
	delete execThread;
}

void Thread::start()
{
	isStop = false;
	execThread->detach();
}

void Thread::stop()
{
	isStop = true;
}

bool Thread::wait(uint32_t time)
{
	bool rlt = false;
	auto t1 = clock();

	while (true)
	{
		if (!isRuning)
		{
			rlt = true;
			break;
		}

		auto t2 = clock();

		if (t2 - t1 > time)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return rlt;
}

uint32_t Thread::getId() const
{
	return (execThread->get_id());
}

void Thread::run()
{
}

void functionExec(Thread* thread)
{
	thread->m_isRuning = true;
	thread->run();
	thread->m_isRuning = false;
}
