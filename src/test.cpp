#include "../include/Channel.hpp"
#include <string>

int main()
{
	Client	a;
	Client	b;
	Client	c;
	Client	d;
	Client	e;
	Client	f;

	(void)d;
	(void)b;
	(void)c;
	Channel	x("ok");
	Channel	y("and", 1, a);
	if (x.putUsers(6, f) == 1)
		std::cout << "1: ";
	if (y.putUsers(1, a) != 1)
		std::cout << y.getName();
	if (x.putUsers(6, f) == 0 && x.putUsers(5, e) == 1)
		std::cout << " OK!!";
	std::cout << std::endl;
	std::cout << x.numClients() << ", " << y.numClients() << std::endl;
	std::cout << x.numUsers() << ", " << y.numUsers() << std::endl;
	std::cout << x.numOpers() << ", " << y.numOpers() << std::endl;
	if (x.putOpers(6) == 1 && x.putOpers(1) != 1 && x.putOpers(6) != 0)
		std::cout << "OK!!" << std::endl;
	std::cout << x.numUsers() << ", " << x.numOpers() << ", " << x.numClients() << std::endl;
	if (x.delOpers(1) != 1 && x.delOpers(6) == 1 && x.delOpers(6) != 1)
		std::cout << "OK!!!" << std::endl;
	std::cout << x.numUsers() << ", " << x.numOpers() << ", " << x.numClients() << std::endl;
	std::cout << "bef: " << x.getKey() << std::endl;
	x.setKey("aaa");
	std::cout << "aft: " << x.getKey() << std::endl;
	std::cout << "bef: " << x.getTopic() << std::endl;
	x.setTopic("123456789012345677890");
	std::cout << "aft: " << x.getTopic() << std::endl;
	std::cout << "bef: " << y.getMaxNumClients() << std::endl;
	for (int i = 10; i < 20; ++ i)
	{
		if (y.putUsers(i, a) != 1)
		{
			std::cout << i << ": cannot put in" << y.numClients() << std::endl;
			break ;
		}
	}
	y.setMaxNumClients(10);
	std::cout << "aft: " << y.getMaxNumClients() << std::endl;
	if (y.putUsers(22, a) == 1)
		std::cout << "OK!!!!" << std::endl;
	x.changeT('-');
	x.changeT('+');
	x.changeT('+');
	if (x.getT() == true)
		std::cout << "OK!!!!!" << std::endl;
	if (x.getI() == false)
		std::cout << "OKKKKKK" << std::endl;
	x.changeI('+');
	x.changeI('-');
	if (x.getI() == false)
		std::cout << "ooooooooooooooo" << std::endl;
	x.putInvite(1, a);
	if (x.isInvite(1) == 1)
		std::cout << "a" << std::endl;
	x.delInvite(1);
	if (x.isInvite(1) == 0)
		std::cout << "no a" << std::endl;
	x.putInvite(6, f);
	if (x.isInvite(6) == 0)
		std::cout << "no f" << std::endl;
	x.putInvite(1, a);
	x.putUsers(1, a);
	if (x.isInvite(1) != 1 && x.isClientIn(1) == 1)
		std::cout << "a in Users" << std::endl;
}
