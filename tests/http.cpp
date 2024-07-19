#include <simple/http.hpp>
#include <simple/json.hpp>

#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>

boost::asio::io_context m_ioc;
boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard = boost::asio::make_work_guard(m_ioc);
boost::asio::io_context::strand m_strand(m_ioc);

int n = 0;

void do_sth(int i)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	int t = n;
	n = t + 1;
	printf("%d\n", i);
}

void strand_do_sth(int i)
{
	m_ioc.post([i]() {
		do_sth(i); 
	});
}

int main_strand()
{
	std::thread([]() {
		printf("run() start\n");
		m_ioc.run();
		printf("run() stop\n");
	}).detach();


	for (int i = 0; i < 10; i++)
	{
		std::thread(strand_do_sth, i).detach();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	printf("\n%d\n", n);

	work_guard.reset();

	return 0;
}

int main_multi_thread()
{

	for (int i = 0; i < 10; i++)
	{
		std::thread(do_sth, i).detach();
	}

	std::thread([]() {
		m_ioc.run();
		}).detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));

	printf("\n%d\n", n);

	return 0;
}

int main_t()
{
	//return main_multi_thread();

	return main_strand();
}


int main()
{
	std::string res;
	//simple::http::get("www.baidu.com", "443", "/", "1.1", res);

	auto  http_req = std::make_shared<simple::http_request>("https://www.baidu.com");

	simple::http_manager http_mngr;

	http_mngr.start_work_thread();

	http_mngr.execute(http_req);

	while (!http_req->finished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	http_mngr.stop_work_thread();

	return 0;
}
