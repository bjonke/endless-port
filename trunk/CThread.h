#ifndef __THREAD_H__
#define __THREAD_H__


class CThread
{
	static DWORD WINAPI ThreadFunc(LPVOID pv)
	{
		(reinterpret_cast<CThread *> (pv))->run();
		return 0;
	}
public:
	typedef DWORD threadid;

	CThread(){};
	virtual ~CThread()
	{
	};
	static threadid getthreadid()
	{
		return ::GetCurrentThreadId();
	}
	static void sleep(long milliseconds = 1)
	{
		::Sleep(milliseconds);
	}

	threadid start()
	{
		threadid id;
		::CreateThread(NULL,0,ThreadFunc,this,0,&id);
		return id;
	}
	virtual void run()= 0;
};

class Thread
{
   public:
      Thread();
      int Start(void * arg);
   protected:
      int Run(void * arg);
      static void * EntryPoint(void*);
      virtual void Setup();
      virtual void Execute(void*);
      void * Arg() const {return Arg_;}
      void Arg(void* a){Arg_ = a;}
   private:
      typedef DWORD ThreadId_;
      void * Arg_;

};


#endif //__THREAD_H__
