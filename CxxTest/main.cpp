#include <iostream>

using namespace std;

class my_test {
public:
	my_test() {
		cout << "enter my_test" <<endl;
	}
	virtual ~my_test() {
		cout << "leave my_test" <<endl;
	}
	virtual void vtest1() {
		cout << "is " << __func__ <<endl;
	}
	
	virtual void vtest2() = 0;

	void test_func();
};

void my_test::test_func(void) {
	cout << "enter " << __func__ <<endl;
}

class testA: public my_test {
public:
	testA() {
		cout << "enter testA" <<endl;
	}
	~testA() {
		cout << "leave testA" <<endl;
	}

	void vtest1 () override {
		cout << "is [testA] " << __func__ <<endl;
	}

	void vtest2(void) {
		cout << "is [testA] " << __func__ <<endl;
	}
};

int main(void)
{
	my_test *tmp;

	tmp = (my_test*)new testA();

	tmp->vtest1();
	tmp->vtest2();
	tmp->test_func();

	delete tmp;
}
